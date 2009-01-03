/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#define DEFF_DEBUG

#ifdef DEFF_DEBUG
#define deff_debug(fmt, rest...) dbprintf(fmt, ## rest)
#else
#define deff_debug(fmt, rest...)
#endif


/** Optional feature for use by deffs that need to manage different
parts of the display independently. */
void (*deff_component_table[4]) (void);

/** The current background display effect */
U8 deff_background;

/** The current running display effect, which could be a
background or foreground effect. */
U8 deff_running;

/** The priority of the running display effect */
U8 deff_prio;



void dump_deffs (void)
{
	dbprintf ("Background: %d\n", deff_background);
	dbprintf ("Running: %d\n", deff_running);
	dbprintf ("Priority: %d\n", deff_prio);
	dbprintf ("Waiters: %s\n", task_find_gid (GID_DEFF_WAITING) ? "yes" : "no");
}


/** Returns the ID of the currently active display effect. */
U8 deff_get_active (void)
{
	return deff_running;
}


/**
 * Common processing that must occur when stopping a running deff.
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
	deff_debug ("deff_stop_task\n");
	/* if (!task_find_gid (GID_DEFF_EXITING)) -- not working yet */
		dmd_reset_transition ();
	kickout_unlock (KLOCK_DEFF);
}


/** Starts the thread for the currently running display effect. */
static void deff_start_task (const deff_t *deff)
{
	task_pid_t tp;

	deff_debug ("deff_start_task\n");

	/* Stop whatever deff is running now */
	task_kill_gid (GID_DEFF);
	deff_stop_task ();

	/* If this deff pauses kickouts, handle that now */
	if (deff->flags & D_PAUSE)
		kickout_lock (KLOCK_DEFF);

	/* Create a task for the new deff */
	tp = task_create_gid (GID_DEFF, deff->fn);
	if (tp == NULL)
	{
		deff_debug ("could not spawn deff task!\n");
	}

	if (deff->page != 0xFF)
		task_set_rom_page (tp, deff->page);
}


struct waiting_deff
{
	U8 id;
	U8 timeout;
};


/**
 * A background task that repeatedly tries to start a display
 * effect for some time.
 */
static void deff_retry_task (void)
{
	struct waiting_deff *args = task_current_class_data (struct waiting_deff);
	const deff_t *deff = &deff_table[args->id];
	while (args->timeout > 0)
	{
		task_sleep_sec (1);
		args->timeout--;
		if (deff_prio < deff->prio)
		{
			deff_start_task (deff);
			break;
		}
	}
	task_exit ();
}


/**
 * Like deff_start(), but does retries if the effect can't be
 * started right now.  It is slightly more efficient to call
 * this than deff_start() for a D_TIMEOUT effect.  It is also
 * possible to set the timeout delay.
 */
void deff_start_retry (deffnum_t id, U8 timeout)
{
	task_pid_t tp = task_create_gid (GID_DEFF_WAITING, deff_retry_task);
	struct waiting_deff *args = task_init_class_data (tp, struct waiting_deff);
	args->id = id;
	args->timeout = timeout;
}


/** Start a statically defined display effect */
void deff_start (deffnum_t id)
{
	const deff_t *deff = &deff_table[id];

	deff_debug ("deff_start\n");
	log_event (SEV_INFO, MOD_DEFF, EV_DEFF_START, id);

	/* Nothing to do if it's already running, unless it's
	 * marked RESTARTABLE. */
	if (id == deff_running)
	{
		if (deff->flags & D_RESTARTABLE)
			deff_start_task (deff);
		else
			return;
	}

	/* This effect can take the display now if it has priority.
	Otherwise, if it wants to wait, a background retry task is
	started. */
	if (deff_prio < deff->prio)
	{
		deff_prio = deff->prio;
		deff_running = id;
		deff_start_task (deff);
	}
	else if (deff->flags & D_TIMEOUT)
	{
		deff_debug ("no priority\n");
		deff_start_retry (id, 5);
	}
	else
	{
		deff_debug ("prio requested = %d, now = %d\n", deff->prio, deff_prio);
	}
}


/** Stop any display effect.
 * If the effect is currently running, the task is killed and something else
 * restarted.
 * If it is queued up, waiting to run, then that request is cancelled.
 */
void deff_stop (deffnum_t dn)
{
	deff_debug ("deff_stop\n");
	log_event (SEV_INFO, MOD_DEFF, EV_DEFF_STOP, dn);
	if (dn == deff_running)
		deff_update ();
}


/** Restart a deff.  If the deff is already running, its thread is stopped
and then restarted.  If it is queued but not active, nothing happens.
If it isn't even in the queue, then it is treated just like deff_start(). */
void deff_restart (deffnum_t dn)
{
	deff_debug ("deff_restart\n");
	log_event (SEV_INFO, MOD_DEFF, EV_DEFF_RESTART, dn);

	if (deff_running == dn)
	{
		deff_start_task (&deff_table[dn]);
	}
	else if (0) /* deff_pending_p (dn) */
	{
		/* TODO : See if the request is pending and cancel it. */
	}
	else
	{
		deff_start (dn);
	}
}


/** Called directly from a deff when it wants to exit */
__noreturn__ void deff_exit (void)
{
	deff_debug ("deff_exit\n");
	log_event (SEV_INFO, MOD_DEFF, EV_DEFF_EXIT, deff_running);

	/* Change the task group ID so that a new task can be started
	with GID_DEFF in the same context. */
	task_setgid (GID_DEFF_EXITING);

	deff_running = 0;
	deff_update ();
	music_refresh ();
	task_exit ();
}


/** Called from a deff when it wants to exit after a certain delay */
__noreturn__ void deff_delay_and_exit (task_ticks_t ticks)
{
	task_sleep (ticks);
	deff_exit ();
}


/** Called from a deff when it wants to toggle between two images
 * on the low and high mapped pages, both in mono mode.
 * COUNT is the number of times to toggle.
 * DELAY is how long to wait between each change. */
void deff_swap_low_high (S8 count, task_ticks_t delay)
{
	dmd_show_low ();
	while (--count >= 0)
	{
		dmd_show_other ();
		task_sleep (delay);
	}
}


/** Lower the priority of the currently running display effect.
This may cause it to be preempted by something more important. */
void deff_nice (enum _priority prio)
{
	/* TODO - verify that this is only being called from
	a display effect */

	/* If increasing the priority, then accept the change but
	nothing else really happens at the moment. */
	if (prio >= deff_prio)
	{
		deff_prio = prio;
		return;
	}

	/* TODO */
	/* Find the highest priority effect that is queued, and
	see if its priority is greater than 'prio'.  If it is,
	make it the new effect (the current one is cancelled).
	Otherwise, the current effect stays but with a lower priority. */
	if (0)
	{
	}
	else
	{
		deff_prio = prio;
	}
}


/** Initialize the display effect subsystem. */
void deff_init (void)
{
	deff_background = 0;
	deff_running = 0;
	deff_prio = 0;
}


/** Stop all running deffs, and cancel any waiting deffs. */
void deff_stop_all (void)
{
	deff_debug ("deff_stop_all\n");
	task_kill_gid (GID_DEFF);
	deff_stop_task ();
	deff_running = deff_prio = 0;
	task_kill_gid (GID_DEFF_WAITING);

	dmd_alloc_low_clean ();
	dmd_show_low ();

	deff_init ();
}


/**
 * Called by display_update functions to signal that
 * display effect DN is a candidate for running, because
 * the necessary conditions for it are all true.
 * PRIO says what priority it has.
 */
void deff_start_bg (deffnum_t dn, enum _priority prio)
{
	deff_debug ("deff_start_bg %d, prio %d\n", dn, prio);
	if (prio == 0)
	{
		const deff_t *deff = &deff_table[dn];
		prio = deff->prio;
	}

	if (prio > deff_prio)
	{
		deff_prio = prio;
		deff_background = dn;
	}
}


/**
 * Request that the background display effect be updated.
 */
void deff_update (void)
{
	U8 previous;

	deff_debug ("deff_update\n");

	/* If there is a transient effect running, then
	don't try anything.  We'll update the background automatically
	when the foreground exits. */
	if (deff_running && (deff_running != deff_background))
	{
		deff_debug ("no update: running=%d, bg=%d\n",
			deff_running, deff_background);
		return;
	}

	/* Recalculate which display effect should run in the
	background */
	previous = deff_running;
	deff_prio = 0;
	deff_background = DEFF_NULL;
	callset_invoke (display_update);

	/* Nothing to do if it's already running */
	if (deff_background == previous)
		return;

	/* Switch to the new effect */
	deff_debug ("deff_update: %d-%d\n", previous, deff_background);
	if (previous != DEFF_NULL)
		deff_stop (previous);
	if (deff_background != DEFF_NULL)
	{
		const deff_t *bgdeff = &deff_table[deff_background];
		deff_running = deff_background;
		deff_start_task (bgdeff);
	}
}


/** If both flippers are pressed and the current running deff
 * is abortable, then stop it. */
CALLSET_ENTRY (deff, flipper_abort)
{
	if (deff_running)
	{
		const deff_t *deff = &deff_table[deff_running];
		if (deff->flags & D_ABORTABLE)
		{
			dbprintf ("Deff %d aborted.\n", deff_running);
			deff_stop (deff_running);
			deff_update ();
		}
	}
}

