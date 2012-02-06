/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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

/** Display effect data management (see deffdata.h) */
U8 deff_data_pending[MAX_DEFF_DATA];
U8 deff_data_pending_count;
U8 deff_data_active[MAX_DEFF_DATA];
U8 deff_data_active_count;

#define MAX_QUEUED_DEFFS 8

struct deff_queue_entry
{
	U8 id;
	U16 timeout;
};

struct deff_queue_entry deff_queue[MAX_QUEUED_DEFFS];


void dump_deffs (void)
{
	dbprintf ("Background: %d\n", deff_background);
	dbprintf ("Running: %d\n", deff_running);
	dbprintf ("Priority: %d\n", deff_prio);
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
#ifdef CONFIG_DMD
	/* TODO : if (!task_find_gid (GID_DEFF_EXITING)) -- not working yet */
		dmd_reset_transition ();
#endif
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

	/* If this deff wants to show the last score, hold
	 * on to that value.  Newer deffs can use the deff data
	 * functions to load arbitrary data, not just scores. */
	if (deff->flags & D_SCORE)
		score_deff_set ();
	deff_data_load ();

	/* Create a task for the new deff */
	tp = task_create_gid (GID_DEFF, deff->fn);
	if (tp)
	{
		if (deff->page != 0xFF)
			task_set_rom_page (tp, deff->page);
		task_set_duration (tp, TASK_DURATION_INF);
	}
}


/**
 * Clear all queued display effects.
 */
void deff_queue_reset (void)
{
	struct deff_queue_entry *dq = deff_queue;
	do
	{
		dq->id = DEFF_NULL;
		dq->timeout = 0;
	} while (++dq < deff_queue + MAX_QUEUED_DEFFS);
}


/**
 * Find a display queue entry with the given id.
 *
 * This can be an actual deff number, or DEFF_NULL if you want to
 * find a free entry.
 */
struct deff_queue_entry *deff_queue_find (U8 id)
{
	struct deff_queue_entry *dq = deff_queue;
	do
	{
		if (dq->id == id)
			return dq;
	} while (++dq < deff_queue + MAX_QUEUED_DEFFS);
	return NULL;
}


/**
 * Find the display queue entry with the highest priority.
 */
struct deff_queue_entry *deff_queue_find_priority (void)
{
	struct deff_queue_entry *dq = deff_queue;
	struct deff_queue_entry *answer = NULL;
	U8 highest_prio = PRI_NULL;

	do
	{
		if (dq->id != DEFF_NULL)
		{
			const deff_t *deff;

			if (time_reached_p (dq->timeout))
			{
				/* This queued effect already expired, so we will
				ignore it and free it. */
				dq->id = dq->timeout = 0;
			}
			else
			{
				deff = &deff_table[dq->id];
				if (deff->prio > highest_prio)
				{
					answer = dq;
					highest_prio = deff->prio;
				}
			}
		}
	} while (++dq < deff_queue + MAX_QUEUED_DEFFS);
	return answer;
}


/**
 * Add a new request to the display queue.
 */
void deff_queue_add (U8 id, U16 timeout)
{
	struct deff_queue_entry *dq;

	/* Ensure that no entry is added to the queue twice.
	If it's already in there, just return. */
	dq = deff_queue_find (id);
	if (dq)
		return;

	/* Find and initialize a free entry.  If the queue is full, then
	the effect just won't ever happen. */
	dq = deff_queue_find (DEFF_NULL);
	if (dq)
	{
		dq->id = id;
		dq->timeout = get_sys_time() + timeout;
	}
}


/**
 * Delete a specific request from the display queue.
 */
void deff_queue_delete (U8 id)
{
	struct deff_queue_entry *dq = deff_queue_find (id);
	if (dq)
	{
		dq->id = 0;
		dq->timeout = 0;
	}
}


/**
 * Scan the display effect table to see if any pending requests
 * can now be satisfied.
 *
 * This is called anytime the running display effect exits or is
 * stopped.
 */
void deff_queue_service (void)
{
	/* Find the highest priority effect in the queue.
	If there is such, start it if its priority exceeds that
	of the currently display effect. */
	struct deff_queue_entry *dq = deff_queue_find_priority ();
	if (dq)
	{
		const deff_t *deff = &deff_table[dq->id];
		if (deff_prio < deff->prio)
		{
			dbprintf ("deff_queue_service starting %d\n", dq->id);
			deff_running = dq->id;
			dq->id = dq->timeout = 0;
			deff_start_task (deff);
			return;
		}
	}

	/* Delay updating background effect briefly, to allow
	synchronous callers to do something else */
	if (task_getgid () == GID_DEFF_EXITING)
	{
		dbprintf ("deff_update delayed on exit\n");
		task_sleep (TIME_133MS);
	}

	/* No queued effect can run now, so try a background update */
	deff_update ();
}


/** Start a statically defined display effect */
void deff_start (deffnum_t id)
{
	const deff_t *deff = &deff_table[id];

	deff_debug ("deff_start %d\n", id);
	log_event (SEV_INFO, MOD_DEFF, EV_DEFF_START, id);

	/* Nothing to do if it's already running, unless it's
	 * marked RESTARTABLE. */
	if (id == deff_running)
	{
		if (deff->flags & D_RESTARTABLE)
			deff_start_task (deff);
		return;
	}

	/* Nothing to do if it's already queued */
	if (deff_queue_find (id))
		return;

	/* This effect can take the display now if it has priority.
	Else, if it wants to wait (because later it might have
	priority), then it is queued.  Else, forget about it. */
	if (deff_prio < deff->prio)
	{
		deff_prio = deff->prio;
		deff_running = id;
		deff_start_task (deff);
	}
	else if (deff->flags & (D_QUEUED | D_TIMEOUT))
	{
		deff_debug ("queueing\n");
		deff_queue_add (id, TIME_7S);
	}
	else
	{
		deff_debug ("prio requested = %d, now = %d\n", deff->prio, deff_prio);
	}
}


/** Stop a display effect.
 * If the effect is currently running, the task is killed and something else
 * restarted.
 * If it is queued up, waiting to run, then that request is cancelled.
 */
void deff_stop (deffnum_t dn)
{
	deff_debug ("deff_stop\n");
	log_event (SEV_INFO, MOD_DEFF, EV_DEFF_STOP, dn);
	if (dn == deff_running)
	{
		task_kill_gid (GID_DEFF);
		deff_stop_task ();
		deff_running = 0;
		deff_queue_service ();
	}
	else
	{
		deff_queue_delete (dn);
	}
}


/**
 * Restart a deff.  If the deff is already running, its thread is stopped
 * and then restarted.  Otherwise, it is treated just like deff_start().
 *
 * This function is identical to deff_start(), but can be used on a deff
 * that is not ordinarily restartable.
 */
void deff_restart (deffnum_t dn)
{
	deff_debug ("deff_restart\n");
	log_event (SEV_INFO, MOD_DEFF, EV_DEFF_RESTART, dn);

	if (deff_running == dn)
		deff_start_task (&deff_table[dn]);
	else
		deff_start (dn);
}


/** Called directly from a deff when it wants to exit */
__noreturn__ void deff_exit (void)
{
	deff_debug ("deff_exit\n");
	log_event (SEV_INFO, MOD_DEFF, EV_DEFF_EXIT, deff_running);

	/* Change the task group ID so that a new task can be started
	with GID_DEFF in the same context. */
	task_setgid (GID_DEFF_EXITING);

	/* Drop priority and clear that we were running */
	deff_running = 0;
	deff_prio = 0;

	/* Check for pending effects that can be started now */
	deff_queue_service ();
	task_exit ();
}


/** Called from a deff when it wants to exit after a certain delay */
__noreturn__ void deff_delay_and_exit (task_ticks_t ticks)
{
	task_sleep (ticks);
	deff_exit ();
}


/** Lower the priority of the currently running display effect.
This may cause it to be preempted by something more important. */
void deff_nice (enum _priority prio)
{
	/* Verify that this is called from the running effect */
	if (task_getgid () != GID_DEFF)
		return;

	/* Change the current priority */
	deff_prio = prio;

	/* Force an update: this task may not be the best anymore */
	deff_queue_service ();
}


/** Initialize the display effect subsystem. */
void deff_init (void)
{
	deff_background = DEFF_NULL;
	deff_running = DEFF_NULL;
	deff_prio = 0;
	deff_queue_reset ();
}


/** Stop all running deffs, and cancel any waiting deffs. */
void deff_stop_all (void)
{
	deff_debug ("deff_stop_all\n");
	task_kill_gid (GID_DEFF);
	deff_stop_task ();
	deff_running = deff_prio = 0;
	deff_queue_reset ();

#ifdef CONFIG_DMD_OR_ALPHA
	dmd_alloc_low_clean ();
	dmd_show_low ();
#endif

	deff_init ();
}


/**
 * Called by display_update functions to signal that
 * display effect DN is a candidate for running, because
 * the necessary conditions for it are all true.
 * PRIO says what priority it has; zero means use what the
 * machine configuration says.
 */
void deff_start_bg (deffnum_t dn, enum _priority prio)
{
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
 *
 * This finds a display effect to run when nothing has been explicitly
 * started or is in the queue.  It ensures that something is always running.
 */
void deff_update (void)
{
	deffnum_t previous;

#ifdef CONFIG_NO_DEFFS
	return;
#endif

	/* If there is a transient effect running, then
	don't try anything.  We'll update the background automatically
	when the foreground exits. */
	if (deff_running && (deff_running != deff_background))
		return;

	/* Recalculate which display effect should run in the
	background */
	previous = deff_running;
	deff_prio = PRI_NULL;
	deff_background = DEFF_NULL;
	if (!in_test)
		callset_invoke (display_update);

	/* Nothing to do if it's already running */
	if (deff_background == previous)
		return;

	/* Switch to the new effect */
	deff_debug ("deff_update: %d -> %d\n", previous, deff_background);
	if (previous != DEFF_NULL)
		deff_stop (previous);
	if (deff_background != DEFF_NULL)
	{
		const deff_t *bgdeff = &deff_table[deff_background];
		deff_running = deff_background;
		deff_start_task (bgdeff);
	}
}


/** Start a display effect and wait for it to finish before returning. */
void deff_start_sync (deffnum_t dn)
{
	U8 n;
	for (n=0; n < 24; n++)
	{
		deff_start (dn);
		if (deff_get_active () == dn)
			break;
		task_sleep (TIME_166MS);
	}
	while (deff_get_active () == dn)
		task_sleep (TIME_66MS);
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
			deff_queue_service ();
		}
	}
}


CALLSET_ENTRY (deff, end_ball)
{
	/* At the beginning of end_ball, stop all display effects.
	But allow things like "round completion" screens play out. */
}

