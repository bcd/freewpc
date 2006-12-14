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
 * \brief A display effect -- or deff -- is a task responsible for drawing to
 * the display (DMD or alphanumeric).
 *
 * At any given time, there may be multiple deffs which have been
 * "started", or queued to run.  However, the display can only be granted
 * to one deff at a time, so only the one with the highest "priority"
 * actually executes.
 *
 * The single running deff always has the same task group ID,
 * GID_DEFF.  Whenever priorities change, the running deff can be
 * stopped just by killing GID_DEFF and then restarting whatever
 * deff needs to be running.
 *
 * There are two kinds of deffs: normal, and running.  Running deffs
 * are special in that they are long-lived and continue to run
 * until explicitly stopped by a call to deff_stop().  Normal deffs
 * are short-lived and stop on their own once the effect completes.
 * If a normal deff can't get the display immediately, it is
 * discarded.  If a running deff can't get the display, it is queued
 * and subject to get it later if priorities change.
 */


/** Declare externs for all of the deff functions */
#define DECL_DEFF(num, flags, pri, fn) extern void fn (void);
#define DECL_DEFF_FAST(num, pri, fn) DECL_DEFF (num, D_NORMAL, pri, fn)
#define DECL_DEFF_MODE(num, pri, fn) DECL_DEFF (num, D_RUNNING, pri, fn)

#ifdef MACHINE_DISPLAY_EFFECTS
MACHINE_DISPLAY_EFFECTS
#endif

/** Now declare the deff table itself */
#undef DECL_DEFF
#define DECL_DEFF(num, flags, pri, fn) \
	[num] = { flags, pri, fn },


static const deff_t deff_table[] = {
#define null_deff deff_exit
#ifdef MACHINE_DISPLAY_EFFECTS
	MACHINE_DISPLAY_EFFECTS
#endif
#ifndef MACHINE_CUSTOM_AMODE
	[DEFF_AMODE] = { D_RUNNING, PRI_AMODE, default_amode_deff },
#endif
};


/** Indicates the id of the deff currently active */
static uint8_t deff_active;

/** Indicates the priority of the deff currently running */
uint8_t deff_prio;

/** Queue of all deffs that have been scheduled to run, even
 * if they are low in priority.  The actively running deff
 * is also in this queue. */
static uint8_t deff_queue[MAX_QUEUED_DEFFS];


U8 deff_get_count (void)
{
	return sizeof (deff_table) / sizeof (deff_t);
}

uint8_t deff_get_active (void)
{
	return deff_active;
}


bool deff_is_running (deffnum_t dn)
{
	U8 i;

	for (i=0; i < MAX_QUEUED_DEFFS; i++)
		if (deff_queue[i] == dn)
			return TRUE;
	return FALSE;
}


static void deff_add_queue (deffnum_t dn)
{
	uint8_t i;

	if (deff_is_running (dn))
		return;

	for (i=0; i < MAX_QUEUED_DEFFS; i++)
		if (deff_queue[i] == 0)
		{
			deff_queue[i] = dn;
			return;
		}

	fatal (ERR_DEFF_QUEUE_FULL);
}


static void deff_remove_queue (deffnum_t dn)
{
	uint8_t i;
	for (i=0; i < MAX_QUEUED_DEFFS; i++)
		if (deff_queue[i] == dn)
			deff_queue[i] = 0;
}


static deffnum_t deff_get_highest_priority (void)
{
	uint8_t i;
	uint8_t prio = 0;
	uint8_t best = DEFF_NULL;

	for (i=0; i < MAX_QUEUED_DEFFS; i++)
	{
		if (deff_queue[i] != 0)
		{
			const deff_t *deff = &deff_table[deff_queue[i]];
			if (deff->prio > prio)
			{
				prio = deff->prio;
				best = deff_queue[i];
			}
		}
	}

	/* Save the priority of the best deff here */
	deff_prio = prio;

	/* Return the deffnum of the best deff to the caller */
	return best;
}

/**
 * Common processing that must occur when stopping a deff.
 * This function is called both when a deff exits and when
 * other code stops the deff.
 * In both cases kickout locks should be cleared.
 * The DMD transition should only be cancelled when a deff
 * is killed externally.  If a deff exits cleanly, it may
 * schedule a transition to be used when the new deff renders
 * its first frame.
 */
static void deff_stop_task (void)
{
	/* if (!task_find_gid (GID_DEFF_EXITING)) -- not working yet */
		dmd_reset_transition ();
	kickout_unlock (KLOCK_DEFF);
}

static void deff_start_task (const deff_t *deff)
{
	task_kill_gid (GID_DEFF);
	deff_stop_task ();
	task_create_gid (GID_DEFF, deff->fn);
}


void deff_start (deffnum_t dn)
{
	const deff_t *deff = &deff_table[dn];

	dbprintf ("Deff %d start request\n", dn);

	if (deff->flags & D_RUNNING)
	{
		dbprintf ("Adding running deff to queue\n");
		deff_add_queue (dn);
		if ((deff->prio > deff_prio)
			&& (dn == deff_get_highest_priority ()))
		{
			/* This is the new active running deff */
			dbprintf ("Requested deff is now highest priority\n");
			deff_active = dn;
			deff_start_task (deff);
		}
		else
		{
			/* This deff cannot run now, because there is a
			 * higher priority deff running. */
			dbprintf ("Can't run because higher priority active\n");
		}
	}
	else
	{
		if (deff->prio > deff_prio)
		{
			dbprintf ("Restarting quick deff with high pri\n");
			deff_active = dn;
			deff_prio = deff->prio;
			deff_start_task (deff);
		}
		else
		{
			dbprintf ("Quick deff lacks pri to run\n");
		}
	}
}


void deff_stop (deffnum_t dn)
{
	const deff_t *deff = &deff_table[dn];

	if (deff->flags & D_RUNNING)
	{
		dbprintf ("Stopping deff #%d\n", dn);
		deff_remove_queue (dn);
		if (dn == deff_active)
	 		deff_start_highest_priority ();
	}
}


void deff_restart (deffnum_t dn)
{
	if (dn == deff_active)
	{
		const deff_t *deff = &deff_table[dn];
		deff_start_task (deff);
	}
	else
	{
		deff_start (dn);
	}
}


void deff_default (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();
	for (;;)
	{
		task_sleep_sec (10);
	}
}


void deff_start_highest_priority (void)
{
	deff_stop_task ();
	deff_active = deff_get_highest_priority ();
	if (deff_active != DEFF_NULL)
	{
		const deff_t *deff = &deff_table[deff_active];
		task_recreate_gid (GID_DEFF, deff->fn);
	}
	else
	{
		task_recreate_gid (GID_DEFF, deff_default);
	}
}


__noreturn__ void deff_exit (void)
{
	dbprintf ("Exiting deff\n");
	task_setgid (GID_DEFF_EXITING);
	deff_remove_queue (deff_active);
	deff_start_highest_priority ();
	task_exit ();
}


void deff_delay_and_exit (task_ticks_t ticks)
{
	task_sleep (ticks);
	deff_exit ();
}


void deff_swap_low_high (int8_t count, task_ticks_t delay)
{
	while (--count >= 0)
	{
		dmd_show_other ();
		task_sleep (delay);
	}
}


void deff_init (void)
{
	deff_prio = 0;
	deff_active = DEFF_NULL;
	memset (deff_queue, 0, MAX_QUEUED_DEFFS);
}


void deff_stop_all (void)
{
	task_kill_gid (GID_DEFF);
	deff_stop_task ();
	dmd_alloc_low_clean ();
	dmd_show_low ();
	deff_init ();
}


