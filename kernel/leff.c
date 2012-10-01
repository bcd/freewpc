/*
 * Copyright 2006-2012 by Brian Dominy <brian@oddchange.com>
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
 * - The logic of two distinct classes of leffs is gone.
 * As long as all of the objects needed can be allocated, the
 * leff can be started.  This also simplifies the lamp
 * twiddling functions themselves, as they can operate on the
 * same buffer every time without needing to check the leff type.
 *
 * - Using a lampset as the allocation unit rather than the lamplist
 * simplifies the alloc/free procedure greatly.
 *
 * - leff_restart() is slightly more optimal if the leff was already
 * running.
 */

extern const U8 * lampset_table[];

#ifndef PINIO_GI_STRINGS
#define PINIO_GI_STRINGS 0
#endif

#undef GID_NULL
#define GID_NULL 0

/** Indicates in a leff definition that it does not allocate any GI */
#define L_NOGI			0
#define L_ALL_GI		PINIO_GI_STRINGS
#define L_ALL_LAMPS  LAMPLIST_ALL

enum { L_NORMAL, L_RUNNING, L_SHARED };

/* Declare externs for all of the deff functions */
#define DECL_LEFF(num, flags, prio, llid, gi, fn, fnpage) \
	extern void fn (void);
#ifdef MACHINE_LAMP_EFFECTS
MACHINE_LAMP_EFFECTS
#endif
/* Now declare the deff table itself */
#undef DECL_LEFF
#define DECL_LEFF(num, flags, prio, llid, gi, fn, fnpage) \
	[num] = { prio, fn, fnpage, llid, gi },

static const leff_t leff_table[] = {
#define null_leff leff_exit
#ifdef MACHINE_LAMP_EFFECTS
	MACHINE_LAMP_EFFECTS
#endif
};

/**
 * A list of the IDs (LEFF_xxx values) of the running lamp effects.
 * This array is indexed by the effect task's GID, minus an offset.
 */
leffnum_t leff_running_list[MAX_RUNNING_LEFFS];


/**
 * The lamp_set that denotes which lamps are free (1)
 * to be allocated, and which are in use (0).
 */
lamp_set leff_free_set;


/**
 * The physical state (on/off) of the lamps which is
 * currently allocated.  For unallocated lamps, the
 * values must be zero.
 */
lamp_set leff_data_set;


/**
 * The GID to be assigned to the next lamp effect task.
 * GIDs are assigned sequentially; if the next GID happens
 * to be in use still, then the next one will be tried, etc.
 */
task_gid_t leff_next_gid;

/************************************************************/


/************************************************************/

/* Define 8 consecutive GIDs to be used for lamp effects.
The ID-generator will ensure that these are consecutive just
by us listing them here together. */
//#define GID_LEFF_BASE
//#define GID_LEFF1
//#define GID_LEFF2
//#define GID_LEFF3
//#define GID_LEFF4
//#define GID_LEFF5
//#define GID_LEFF6
//#define GID_LEFF7

//#define DEBUG_LEFFS

static const U8 *leff_get_set (const leff_t *leff)
{
	return lampset_table[leff->llid];
}

#if defined(DEBUGGER) || defined(DEBUG_LEFFS)

/**
 * Dump a lamp set for debugging.
 */
static void leff_res_dump (const lamp_set set)
{
	U8 i;
	for (i=0; i < sizeof (lamp_set); i++)
	{
		dbprintf ("%02X ", set[i]);
	}
	dbprintf ("\n");
}


/**
 * Dump the entire state of the lamp effect module.
 */
void leff_dump (void)
{
	U8 idx;
	page_push (MD_PAGE);
	dbprintf ("Running leffs:\n");
	for (idx = 0 ; idx < MAX_RUNNING_LEFFS; idx++)
	{
		U8 id = leff_running_list[idx];
		if (id != LEFF_NULL)
		{
			const leff_t *leff = &leff_table[id];
			dbprintf ("%d: ID %02d:  GI %02X   LM ", idx, id, leff->gi);
			leff_res_dump (leff_get_set (leff));
		}
	}
	dbprintf ("Total free:  ");
	leff_res_dump (leff_free_set);
	dbprintf ("Total state: ");
	leff_res_dump (leff_data_set);
	page_pop ();
}
#endif /* DEBUGGER */

/**
 * Allocate a GID for a new leff, that is not in use now.
 */
static task_gid_t leff_gid_alloc (void)
{
	task_gid_t gid = leff_next_gid;
	task_gid_t first_gid = gid;

	/* Starting from the next GID to be tried, loop through
	all 8 GIDs and return the first one that is free. */
	while (task_find_gid (gid))
	{
		gid++;
		if (gid == GID_LEFF_BASE + MAX_RUNNING_LEFFS)
			gid = GID_LEFF_BASE;
		if (gid == first_gid)
			return GID_NULL;
	}

	/* Set the next GID to be tried to be one higher than the one
	we are allocating.  This is an optimization; we could always
	scan from the beginning, but in the common case where only a few
	leffs are running, this is more likely to succeed faster. */
	leff_next_gid = gid+1;
	if (leff_next_gid == GID_LEFF_BASE + MAX_RUNNING_LEFFS)
		leff_next_gid = GID_LEFF_BASE;

	return gid;
}


/**
 * Find the GID for a given lamp effect that is running.
 * Return GID_NULL if that effect isn't running.
 */
static task_gid_t leff_gid_find_by_id (leffnum_t id)
{
	U8 idx;
	for (idx = 0; idx < MAX_RUNNING_LEFFS; idx++)
	{
		if (leff_running_list[idx] == id)
			return GID_LEFF_BASE+idx;
	}
	return GID_NULL;
}


/**
 * Allocate resources to a new lamp effect.
 */
static void leff_res_alloc (const lamp_set res)
{
	lamp_set_subtract (leff_free_set, res);
}


/**
 * Free resources for an exiting lamp effect.
 */
static void leff_res_free (const lamp_set res)
{
	lamp_set_add (leff_free_set, res);
}


/**
 * Test if the given resources can be allocated.
 */
static bool leff_res_can_alloc (const lamp_set res)
{
	return lamp_set_can_be_added (leff_free_set, res);
}


/**
 * Return whether a lamp effect that needs already-allocated resources
 * will be able to run, because it has higher priority than the
 * leffs that are holding its required resources.
 *
 * When this returns FALSE, the new leff can't be started at all.
 * When this returns TRUE, any lower priority effects will have been
 * stopped, and the caller is allowed to continue starting the new
 * effect.
 */
static bool leff_can_preempt (const leff_t *leff)
{
	U8 idx;
	leffnum_t rid;
	const leff_t *rleff;

	/* Scan all running leffs to see which ones are holding our lamps. */
	for (idx = 0; idx < MAX_RUNNING_LEFFS; idx++)
	{
		rid = leff_running_list[idx];
		if (rid != LEFF_NULL)
		{
			rleff = &leff_table[rid];
			if (!lamp_set_disjoint (leff_get_set (rleff), leff_get_set (leff))
				&& rleff->prio >= leff->prio)
			{
				/* This running leff has some of the lamps we need, but
				it is higher priority than us.  So the new effect cannot
				be started. */
				return FALSE;
			}
		}
	}

	/* We scanned the whole table and found no overlapping leffs with
	higher priority.  Therefore, our leff is allowed to run.  We can stop
	everything that is overlapping and the caller may start the new leff. */
	for (idx = 0; idx < MAX_RUNNING_LEFFS; idx++)
	{
		rid = leff_running_list[idx];
		if (rid != LEFF_NULL)
		{
			rleff = &leff_table[rid];
			if (!lamp_set_disjoint (leff_get_set (rleff), leff_get_set (leff))
				&& rleff->prio < leff->prio)
			{
				leff_stop (rid);
			}
		}
	}
	return TRUE;
}


/**
 * Update internal state to note that a leff is no longer running.
 *
 * This is invoked from both leff_exit() on its own, and from
 * an external leff_stop() call.
 */
static void leff_close (const leff_t *leff, U8 idx)
{
	dbprintf ("Closing leff %d (idx %d)\n", leff_running_list[idx], idx);

	/* Turn off the resources that were in use, and free them */
	page_push (MD_PAGE);
	rtt_disable ();
	lamp_set_subtract (leff_data_set, leff_get_set (leff));
	leff_res_free (leff_get_set (leff));
	rtt_enable ();
	page_pop ();
#ifdef CONFIG_GI
	gi_leff_free (leff->gi);
#endif
#ifdef CONFIG_TRIAC
	triac_update ();
#endif

	/* And mark the leff as not running anymore */
	leff_running_list[idx] = LEFF_NULL;
}


/**
 * Create a task to run the lamp effect.
 */
static void leff_create_task (const leff_t *leff, task_gid_t gid)
{
	task_pid_t tp = task_create_gid1 (gid, leff->fn);
	if (leff->page != 0)
		task_set_rom_page (tp, leff->page);
}


/**
 * Start a lamp effect.
 */
void leff_start (leffnum_t id)
{
	const leff_t *leff = &leff_table[id];
	task_gid_t gid;

	/* See if the leff is already running.  If so, get out now. */
	gid = leff_gid_find_by_id (id);
	if (gid != GID_NULL)
		return;

	/* Allocate a new GID for the lamp effect process.
	If there are too many leffs already running, then this will fail. */
	gid = leff_gid_alloc ();
	if (gid == GID_NULL)
		return;

	/* If the lamps can't all be allocated, then return.
	If the same leff is already running, this will also fail.
	If this does fail, note that we do not explicitly free up the GID
	allocated, but it will get reused implicitly since they cycle. */
	page_push (MD_PAGE);
	if (!leff_res_can_alloc (leff_get_set (leff)) && !leff_can_preempt (leff))
	{
		goto conflict;
	}

	/* Mark resources as in use. */
	leff_res_alloc (leff_get_set (leff));
#ifdef CONFIG_GI
	gi_leff_allocate (leff->gi);
#ifdef CONFIG_TRIAC
	triac_update ();
#endif
#endif

	/* Associate the GID with the lamp effect number. */
	leff_running_list[gid - GID_LEFF_BASE] = id;

	/* Start the task to run the effect */
	/* TODO - it won't start in the same page as caller! */
	leff_create_task (leff, gid);

	dbprintf ("Started leff %d\n", id);
#ifdef DEBUG_LEFFS
	leff_dump ();
#endif
conflict:
	page_pop ();
}


/**
 * Stop a lamp effect.
 */
void leff_stop (leffnum_t id)
{
	task_gid_t gid;

	/* Find the GID for the task running the effect.  If GID_NULL,
	the effect is not running. */
	gid = leff_gid_find_by_id (id);
	if (gid == GID_NULL)
		return;

	/* Stop it */
	dbprintf ("Stopping leff %d\n", id);
	task_kill_gid (gid);

	/* Free up the resources it was using */
	leff_close (&leff_table[id], gid - GID_LEFF_BASE);
#ifdef DEBUG_LEFFS
	leff_dump ();
#endif
}


const leff_t *leff_get_current (void)
{
	task_gid_t gid;
	U8 idx;
	U8 id;

	gid = task_getgid ();
	idx = gid - GID_LEFF_BASE;
	id = leff_running_list[idx];
	return &leff_table[id];
}


/**
 * Exit from a running lamp effect.
 */
void leff_exit (void)
{
	task_gid_t gid;
	U8 idx;
	U8 id;

	/* Find the GID (cannot be GID_NULL) */
	gid = task_getgid ();

	/* Free up the resources */
	idx = gid - GID_LEFF_BASE;
	id = leff_running_list[idx];
	leff_close (&leff_table[id], idx);
#ifdef DEBUG_LEFFS
	leff_dump ();
#endif

	/* Exit from the task */
	task_exit ();
}


/**
 * An optimized form of leff_stop() followed by leff_start().
 *
 * If the task is already running, then the lamps do not need to be
 * freed and re-allocated; the task just needs to be restarted.
 */
void leff_restart (leffnum_t id)
{
	task_gid_t gid;
	const leff_t *leff;

	gid = leff_gid_find_by_id (id);
	if (gid == GID_NULL)
	{
		leff_start (id);
		return;
	}

	dbprintf ("leff_restart %d\n", id);
	leff = &leff_table[id];
	task_kill_gid (gid);
	page_push (MD_PAGE);
	lamp_set_subtract (leff_data_set, leff_get_set (leff));
	page_pop ();
	leff_create_task (leff, gid);
#ifdef DEBUG_LEFFS
	leff_dump ();
#endif
}


/**
 * Reset the state of the lamp effects.
 * This does not kill processes.
 */
void leff_reset (void)
{
	/* Note that no leffs are running */
	memset (leff_running_list, LEFF_NULL, sizeof (leff_running_list));

	/* Free up all resources (remember 1 = free).  Also clear the
	lamp matrix bits */
	memset (&leff_free_set, 0xFF, sizeof (leff_free_set));
	memset (&leff_data_set, 0, sizeof (leff_data_set));
}


/**
 * Stop all running lamp effects.
 * This is stronger than leff_reset(), in that it actively kills processes.
 */
void leff_stop_all (void)
{
	U8 idx;
	dbprintf ("leff_stop_all\n");
	for (idx = 0; idx < MAX_RUNNING_LEFFS; idx++)
		task_kill_gid (GID_LEFF_BASE + idx);
	leff_reset ();
}


/**
 * Check if a lamp effect is running or not.
 */
bool leff_running_p (leffnum_t id)
{
	return (leff_gid_find_by_id (id) != GID_NULL) ? TRUE : FALSE;
}


/**
 * Allocate a single lamp to a quick leff.
 *
 * Quick leffs are not true lamp effects; they are not in the range of
 * GID_LEFF_BASE.
 */
bool leff_quick_alloc (lampnum_t lamp)
{
	if (bit_test (leff_free_set, lamp))
	{
		bit_off (leff_free_set, lamp);
		return TRUE;
	}
	return FALSE;
}


/**
 * Free a single lamp from a quick leff.
 */
void leff_quick_free (lampnum_t lamp)
{
	bit_on (leff_free_set, lamp);
}


/**
 * Initialize the lamp effect subsystem.
 */
void leff_init (void)
{
	leff_next_gid = GID_LEFF_BASE;
	leff_reset ();
}

