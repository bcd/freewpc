/*
 * Copyright 2006, 2007, 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>

/**
 * \file
 * \brief Lamp effect management
 *
 * A leff, or lamp effect, is a temporary override of the lamp matrix
 * bits for a "light show".  At present, the leff module operates
 * independently, but identically to the display effect module.
 * See deff.c for details.
 *
 * Multiple lamp effects are allowed to run concurrently.
 * (TODO : as long as they do not use the same lamps!.)   There are
 * really two different kinds of lamp effects: temporary "light shows" and
 * long-running effects.  Light shows can be skipped if possible,
 * but other effects indicate critical information to the player;
 * e.g. ballsaver is active, jets at max, etc.  The latter usually
 * do not overlap.  The strategy should be to treat these separately.
 * Temporary light shows act more like display effects and can only
 * be one at a time.  Other lamp effects "share" one of the leff
 * matrices, and can run concurrently.
 */

#ifndef PINIO_GI_STRINGS
#define PINIO_GI_STRINGS 0
#endif

/** Indicates in a leff definition that it does not allocate any lamps */
#define L_NOLAMPS		0x0

/** Indicates in a leff definition that it allocates all lamps */
#define L_ALL_LAMPS	0xFF

/** Indicates in a leff definition that it does not allocate any GI */
#define L_NOGI			0

/** Indicates in a leff definition that it allocates all GI */
#define L_ALL_GI		PINIO_GI_STRINGS

/* Declare externs for all of the deff functions */
#define DECL_LEFF(num, flags, pri, b1, b2, fn, fnpage) \
	extern void fn (void);
#ifdef MACHINE_LAMP_EFFECTS
MACHINE_LAMP_EFFECTS
#endif

/* Now declare the deff table itself */
#undef DECL_LEFF
#define DECL_LEFF(num, flags, pri, b1, b2, fn, fnpage) \
	[num] = { flags, pri, b1, b2, fn, fnpage },

static const leff_t leff_table[] = {
#define null_leff leff_exit
	[LEFF_NULL] = { L_NORMAL, 0, 0, 0, NULL, 0 },
#ifdef MACHINE_LAMP_EFFECTS
	MACHINE_LAMP_EFFECTS
#endif
};


/* Declare externs for lamp bit matrices used by leffs */
extern __fastram__ lamp_set lamp_leff1_allocated;
extern __fastram__ lamp_set lamp_leff1_matrix;
extern __fastram__ lamp_set lamp_leff2_allocated;
extern __fastram__ lamp_set lamp_leff2_matrix;


/** Indicates the priority of the exclusive leff currently running */
U8 leff_prio;

/** A bitarray in which a '1' means that said leff is running, '0'
 * otherwise. */
U8 leffs_running[BITS_TO_BYTES (MAX_LEFFS)];

void leff_dump (void)
{
}

/** Test if a lamp effect is running. */
bool leff_running_p (leffnum_t dn)
{
	return (bitarray_test (leffs_running, dn) != 0);
}


/** Returns the ID of the highest priority exclusive lamp effect
 * still queued to run.  If none exist, LEFF_NULL is returned.
 * This function also updates the global leff_prio to the
 * selected leff's priority value.
 */
static leffnum_t leff_get_highest_priority (void)
{
	U8 i;
	U8 prio = 0;
	U8 best = LEFF_NULL;

	for (i=0; i < MAX_LEFFS; i++)
	{
		if (leff_running_p (i))
		{
			const leff_t *leff = &leff_table[i];
			if (leff->flags & L_RUNNING && leff->prio > prio)
			{
				prio = leff->prio;
				best = i;
			}
		}
	}

	/* Save the priority of the best leff here */
	leff_prio = prio;

	/* Return the leffnum of the best leff to the caller */
	return best;
}


/** Start the lamp effect function, but do a little
 * housekeeping before starting the task...
 *
 * If the leff has declared a lamplist, then those lamps
 * are allocated.  Likewise, if it needs GI, then those
 * strings are allocated.  Allocation disables the normal
 * outputs and gives the effect priority.
 */
task_pid_t leff_create_handler (const leff_t *leff)
{
	task_pid_t tp;
	leff_data_t *cdata;

	/* Allocate lamps needed by the lamp effect */
	if (leff->lamplist != L_NOLAMPS)
	{
		/* L_ALL_LAMPS is equivalent to LAMPLIST_ALL and will cause
		 * all lamps to be allocated.  Other values will only
		 * allocate a subset of the lamps */
		if (leff->lamplist == L_ALL_LAMPS)
		{
			if (leff->flags & L_SHARED)
				fatal (ERR_INVALID_LEFF_CONFIG);	
			else
			{
				lamp_leff1_erase ();
				lamp_leff1_allocate_all ();
			}
		}
		else
		{
			/* Allocate specific lamps. */
			/* Apply the allocation function to each element of the lamplist.
			 * Ensure the apply delay is zero first. */
			if (leff->flags & L_SHARED)
			{
				lamplist_apply_nomacro (leff->lamplist, lamp_leff2_allocate);
			}
			else
			{
				/* Start by freeing up any allocations that are lingering. */
				task_kill_gid (GID_LEFF);
				lamp_leff1_erase ();
				lamp_leff1_free_all ();
				lamplist_apply_nomacro (leff->lamplist, lamp_leff_allocate);
			}
		}
	}


	/* Now all allocations are in place, start the lamp effect task.
	 * This implicitly stops whatever leff was previously running
	 * for a running task. */
	if (leff->flags & L_SHARED)
		tp = task_create_gid (GID_SHARED_LEFF, leff->fn);
	else
	{
#ifdef CONFIG_GI
		/* Free any existing GI allocations. */
		gi_leff_free (PINIO_GI_STRINGS);

		/* Allocate general illumination needed by the lamp effect */
		if (leff->gi != L_NOGI)
			gi_leff_allocate (leff->gi);
#endif
		/* Start the task */
		tp = task_recreate_gid (GID_LEFF, leff->fn);
	}

	/* Initialize the new leff's private data before it runs */
	cdata = task_init_class_data (tp, leff_data_t);
	cdata->apply_delay = 0;
	cdata->data = 0;
	cdata->flags = leff->flags;

	/* If it resides outside of the system page, set that up. */
	if (leff->page != 0xFF)
		task_set_rom_page (tp, leff->page);

	return tp;
}


/** Start a lamp effect.  dn indicates the LEFF_xxx code for which one to start. */
void leff_start (leffnum_t dn)
{
	const leff_t *leff = &leff_table[dn];

	/* If the leff is already running, there is nothing to do. */
	if (leff_running_p (dn))
		return;

	/* Mark the leff as running now.  This is done regardless of its type. */
	dbprintf ("Leff start %d\n", dn);
	log_event (SEV_INFO, MOD_LAMP, EV_LEFF_START, dn);

	/* If this is an exclusive leff, and it lacks priority to run,
	 * then return.  If marked RUNNING, it can be started later
	 * so mark it pending. */
	if (!(leff->flags & L_SHARED) && (leff->prio < leff_prio))
	{
		if (leff->flags & L_RUNNING)
			bitarray_set (leffs_running, dn);
		return;
	}

	/* Either it is shared, or the highest priority exclusive
	 * effect.  In either way, we can start it up now. */
	bitarray_set (leffs_running, dn);
	task_pid_t tp = leff_create_handler (leff);
	(task_class_data (tp, leff_data_t))->id = dn;
}


/** Find the task that is running the specified lamp effect.
Returns NULL if the task can't be found. */
static task_pid_t leff_find_shared (leffnum_t dn)
{
	/* TODO - this is slow */
	task_pid_t tp = task_find_gid (GID_SHARED_LEFF);
	while (tp && ((task_class_data (tp, leff_data_t))->id != dn))
	{
		tp = task_find_gid_next (tp, GID_SHARED_LEFF);
	}
	return tp;
}


/** Stops a lamp effect from running. */
void leff_stop (leffnum_t dn)
{
	const leff_t *leff = &leff_table[dn];

	/* If the leff is not already running, there is nothing to do. */
	if (!leff_running_p (dn))
		return;

	/* Mark the leff as stopped. */
	bitarray_clear (leffs_running, dn);

	/* The leff is certainly running and needs to be stopped.
	 * Find the task running the leff and  kill it. */
	if (leff->flags & L_SHARED)
	{
		/* Search through all shared leffs that are
		running for the one we want to stop.
		Its allocations must be freed and the task stopped. */
		task_pid_t tp = leff_find_shared (dn);
		if (tp)
		{
			dbprintf ("Leff stop %d\n", dn);
			log_event (SEV_INFO, MOD_LAMP, EV_LEFF_STOP, dn);
			task_kill_pid (tp);
			lamplist_apply_nomacro (leff->lamplist, lamp_leff2_free);
		}
	}
	else
	{
		lamp_leff1_erase (); /* TODO : these two functions go together */
		lamp_leff1_free_all ();
#ifdef CONFIG_GI
		if (leff->gi != L_NOGI)
			gi_leff_free (leff->gi);
#endif
		leff_start_highest_priority ();
	}
}


/** Restart a lamp effect.  If already running, its task should
 * be stopped and started again.  If not, then an ordinary start
 * will do. */
void leff_restart (leffnum_t dn)
{
	leff_stop (dn);
	leff_start (dn);
}


/** Restarts the lamp effect handler for the highest priority
 * exclusive leff that has been started. */
void leff_start_highest_priority (void)
{
	U8 leff_active = leff_get_highest_priority ();
	if (leff_active != LEFF_NULL)
	{
		const leff_t *leff = &leff_table[leff_active];
		dbprintf ("leff_start_highest_priority returned %d\n", leff_active);
		leff_create_handler (leff);
	}
	else
	{
		task_kill_gid (GID_LEFF);
		lamp_leff1_erase ();
		lamp_leff1_free_all ();
	}
}


/** Called from a lamp effect that wants to exit.
 * It will check to see if any other lamp effects are queued and
 * if so, start the highest priority one, after removing itself
 * from the queue.
 */
__noreturn__ void leff_exit (void)
{
	const leff_t *leff;
	
	dbprintf ("Exiting leff %d\n", leff_self_id);
	log_event (SEV_INFO, MOD_LAMP, EV_LEFF_EXIT, leff_self_id);

	bitarray_clear (leffs_running, leff_self_id);
	leff = &leff_table[leff_self_id];

	if (leff_running_flags & L_SHARED)
	{
		lamplist_apply_nomacro (leff->lamplist, lamp_leff2_free);
	}
	else
	{
		/* Note: global leffs can do leff_exit with peer
		tasks still running ... they will eventually be
		stopped, too */
#ifdef CONFIG_GI
		if (leff->gi != L_NOGI)
			gi_leff_free (leff->gi);
#endif
		/* Change the GID so that we are no longer
		 * considered a leff. */
		task_setgid (GID_LEFF_EXITING);

		leff_start_highest_priority ();
	}
	task_exit ();
}


/** Initialize the lamp effect subsystem at startup */
void leff_init (void)
{
	leff_prio = 0;
	memset (leffs_running, 0, sizeof (leffs_running));
}


/** Stops all lamp effects.  This is similar to
 * leff_init but takes care of stopping anything that might
 * be running. */
void leff_stop_all (void)
{
	task_kill_gid (GID_LEFF);
	task_kill_gid (GID_SHARED_LEFF);
#ifdef CONFIG_GI
	gi_leff_free (PINIO_GI_STRINGS);
#endif
	lamp_leff1_free_all ();
	lamp_leff1_erase ();
	lamp_leff2_free_all ();
	lamp_leff2_erase ();
	leff_init ();
}

