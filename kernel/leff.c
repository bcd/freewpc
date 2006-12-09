/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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
 * TODO : multiple lamp effects should be allowed to run concurrently,
 * as long as they do not use the same lamps.  There are really two
 * different kinds of lamp effects: temporary "light shows" and
 * long-running effects.  Light shows can be skipped if possible,
 * but other effects indicate critical information to the player;
 * e.g. ballsaver is active, jets at max, etc.  The latter usually
 * do not overlap.  The strategy should be to treat these separately.
 * Temporary light shows act more like display effects and can only
 * be one at a time.  Other lamp effects "share" one of the leff
 * matrices, and can run concurrently.  The caller is expected not
 * to start multiple of these which overlap (of course this can be
 * verified).
 */

#define L_NOLAMPS		0x0
#define L_ALL_LAMPS	0xFF
#define L_NOGI			0
#define L_ALL_GI		0x1F

/* Declare externs for all of the deff functions */
#define DECL_LEFF(num, flags, pri, b1, b2, fn) \
	extern void fn (void);

#ifdef MACHINE_LAMP_EFFECTS
MACHINE_LAMP_EFFECTS
#endif

/* Now declare the deff table itself */
#undef DECL_LEFF
#define DECL_LEFF(num, flags, pri, b1, b2, fn) \
	[num] = { flags, pri, b1, b2, fn },

static const leff_t leff_table[] = {
#ifdef USE_MD
#define null_leff leff_exit
#endif
	[LEFF_NULL] = { L_NORMAL, 0, 0, 0, NULL },
#ifdef MACHINE_LAMP_EFFECTS
	MACHINE_LAMP_EFFECTS
#endif
};


/* Declare externs for lamp bit matrices used by leffs */
extern __fastram__ U8 lamp_leff1_allocated[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_leff1_matrix[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_leff2_allocated[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_leff2_matrix[NUM_LAMP_COLS];


/** Indicates the id of the leff currently active */
static U8 leff_active;

/** Indicates the priority of the leff currently running */
U8 leff_prio;

/** Queue of all leffs that have been scheduled to run, even
 * if they are low in priority.  The actively running leff
 * is also in this queue.
 *
 * Each nonzero value in this array represents a leff number
 * for a lamp effect that is in the running state.  Zero
 * entries are ignored.  The leff that actually runs
 * depends on the priority settings of each leff, which are
 * statically configured. */
static U8 leff_queue[MAX_QUEUED_LEFFS];

#define MAX_SHARED_LEFFS 8
static U8 shared_leff_queue[MAX_SHARED_LEFFS];


/** Lamp effect function for a leff that turns all lights off.
 * Used by the system-defined tilt function. */
void no_lights_leff (void)
{
	triac_leff_disable (TRIAC_GI_MASK);
	for (;;)
		task_sleep_sec (5);
}


/** Returns the index of the current active lamp effect. */
U8 leff_get_active (void)
{
	return leff_active;
}


/** Adds a lamp effect number into the running queue. */
static void leff_add_queue (leffnum_t dn)
{
	U8 i;

	/* First scan the queue and see if it's already in there; if so
	 * then there's nothing to do */
	for (i=0; i < MAX_QUEUED_LEFFS; i++)
		if (leff_queue[i] == dn)
			return;

	/* Now scan the queue for the first free space. */
	for (i=0; i < MAX_QUEUED_LEFFS; i++)
		if (leff_queue[i] == 0)
		{
			leff_queue[i] = dn;
			return;
		}

	/* Throw an error if the queue ever gets full. */
	fatal (ERR_LEFF_QUEUE_FULL);
}


/** Removes a lamp effect number from the running queue. */
static void leff_remove_queue (leffnum_t dn)
{
	U8 i;
	for (i=0; i < MAX_QUEUED_LEFFS; i++)
		if (leff_queue[i] == dn)
			leff_queue[i] = 0;
}


/** Returns the lamp effect number that is the highest priority
 * in the queue right now.
 * This function also updates the global leff_prio to the
 * selected leff's priority value.
 */
static leffnum_t leff_get_highest_priority (void)
{
	U8 i;
	U8 prio = 0;
	U8 best = LEFF_NULL;

	for (i=0; i < MAX_QUEUED_LEFFS; i++)
	{
		if (leff_queue[i] != 0)
		{
			const leff_t *leff = &leff_table[leff_queue[i]];
			if (leff->prio > prio)
			{
				prio = leff->prio;
				best = leff_queue[i];
			}
		}
	}

	/* Save the priority of the best leff here */
	leff_prio = prio;

	/* Return the leffnum of the best leff to the caller */
	return best;
}


/* Start the lamp effect function, but do a little
 * housekeeping before starting the task...
 *
 * If the leff has declared a lampset, then those lamps
 * are allocated.  Likewise, if it needs GI, then those
 * strings are allocated.  Allocation disables the normal
 * outputs and gives the effect priority.
 */
task_t *leff_create_handler (const leff_t *leff)
{
	task_pid_t tp;

	/* Allocate lamps needed by the lamp effect */
	if (leff->lampset != L_NOLAMPS)
	{

		/* L_ALL_LAMPS is equivalent to LAMPSET_ALL and will cause
		 * all lamps to be allocated.  Other values will only
		 * allocate a subset of the lamps */
		if (leff->lampset == L_ALL_LAMPS)
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
			/* Apply the allocation function to each element of the lampset.
			 * Ensure the apply delay is zero first. */
			lampset_set_apply_delay (0);
			if (leff->flags & L_SHARED)
			{
				lampset_apply (leff->lampset, lamp_leff2_allocate);
			}
			else
			{
				/* Start by freeing up any allocations that are lingering. */
				lamp_leff1_erase ();
				lamp_leff1_free_all ();
				lampset_apply (leff->lampset, lamp_leff_allocate);
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
		/* Allocate general illumination needed by the lamp effect */
		if (leff->gi != L_NOGI)
			triac_leff_allocate (leff->gi);

		/* Start the task */
		tp = task_recreate_gid (GID_LEFF, leff->fn);
	}

	/* Initialize the new leff's private data before it runs */
	task_set_thread_data (tp, L_PRIV_APPLY_COUNT, 0);
	task_set_thread_data (tp, L_PRIV_DATA, 0);
	task_set_thread_data (tp, L_PRIV_FLAGS, leff->flags);
	return tp;
}


/** Start a lamp effect.  dn indicates the LEFF_xxx code for which one to start. */
void leff_start (leffnum_t dn)
{
	const leff_t *leff = &leff_table[dn];

	dbprintf ("Leff start request for #%d\n", dn);

	if (leff->flags & L_SHARED)
	{
		task_t *tp = leff_create_handler (leff);
		task_set_thread_data (tp, L_PRIV_ID, dn);
	}
	else if (leff->flags & L_RUNNING)
	{
		db_puts ("Adding running leff to queue\n");
		leff_add_queue (dn);
		if (dn == leff_get_highest_priority ())
		{
			/* This is the new active running leff */
			db_puts ("Requested leff is now highest priority\n");
			leff_active = dn;
			leff_create_handler (leff);
		}
		else
		{
			/* This leff cannot run now, because there is a
			 * higher priority leff running. */
			db_puts ("Can't run because higher priority active\n");
		}
	}
	else
	{
		if (leff->prio > leff_prio)
		{
			db_puts ("Restarting quick leff with high pri\n");
			leff_active = dn;
			leff_prio = leff->prio;
			leff_create_handler (leff);
		}
		else
		{
			db_puts ("Quick leff lacks pri to run\n");
		}
	}
}


/** Stops a lamp effect from running. */
void leff_stop (leffnum_t dn)
{
	const leff_t *leff = &leff_table[dn];

	if (leff->flags & L_SHARED)
	{
		/* Search through all shared leffs that are
		running for the one we want to stop.  No need to
		dequeue it, but its allocations must be freed and
		the task stopped. */
		task_pid_t tp = task_find_gid_data (GID_SHARED_LEFF, L_PRIV_ID, dn);
		if (tp)
		{
			dbprintf ("Stopping sharing leff %d, pid=%p\n", dn, tp);

			lampset_set_apply_delay (0);
			task_kill_pid (tp);
			lampset_apply (leff->lampset, lamp_leff2_free);
		}
		else
		{
			dbprintf ("Couldn't find shared leff %d\n", dn);
		}
	}
	else if ((leff->flags & L_RUNNING) || (dn == leff_active))
	{
		dbprintf ("Stopping leff #%d\n", dn);
		leff_remove_queue (dn);
		lamp_leff1_free_all ();
		if (leff->gi != L_NOGI)
			triac_leff_free (leff->gi);
		leff_start_highest_priority ();
	}
}


/** Restart a lamp effect. */
void leff_restart (leffnum_t dn)
{
	if (leff_table[dn].flags & L_SHARED)
	{
		dbprintf ("leff_restart shared\n");
	}
	else if (dn == leff_active)
	{
		const leff_t *leff = &leff_table[dn];
		leff_create_handler (leff);
	}
	else
	{
		leff_start (dn);
	}
}


/** The default (null) lamp effect function.  It never exits
 * once started and can only be stopped explicitly. */
void leff_default (void)
{
	for (;;)
		task_sleep_sec (10);
}


/** Restarts the lamp effect handler for the highest priority
 * leff that has been started. */
void leff_start_highest_priority (void)
{
	db_puts ("Restarting highest priority leff\n");

	leff_active = leff_get_highest_priority ();
	if (leff_active != LEFF_NULL)
	{
		const leff_t *leff = &leff_table[leff_active];
		db_puts ("Recreating leff task\n");
		leff_create_handler (leff);
	}
	else
	{
		lamp_leff1_erase ();
		lamp_leff1_free_all ();
		task_recreate_gid (GID_LEFF, leff_default);
	}
}


/** Called from a lamp effect that wants to exit.
 * It will check to see if any other lamp effects are queued and
 * if so, start the highest priority one, after removing itself
 * from the queue.
 */
__noreturn__ void leff_exit (void)
{
	const leff_t *leff = &leff_table[leff_active];

	db_puts ("Exiting leff\n");
	if (leff->gi != L_NOGI)
		triac_leff_free (leff->gi);
	task_setgid (GID_LEFF_EXITING);
	leff_remove_queue (leff_active);
	leff_start_highest_priority ();
	task_exit ();
}


/** Initialize the lamp effect subsystem at startup */
void leff_init (void)
{
	leff_prio = 0;
	leff_active = LEFF_NULL;
	memset (leff_queue, 0, MAX_QUEUED_LEFFS);
	memset (shared_leff_queue, 0, MAX_SHARED_LEFFS);
}


/** Stops all lamp effects.  This is similar to
 * leff_init but takes care of stopping anything that might
 * be running. */
void leff_stop_all (void)
{
	task_kill_gid (GID_LEFF);
	triac_leff_free (TRIAC_GI_MASK);
	lamp_leff1_free_all ();
	lamp_leff1_erase ();
	lamp_leff2_free_all ();
	lamp_leff2_erase ();
	leff_init ();
}

