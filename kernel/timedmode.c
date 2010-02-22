/*
 * Copyright 2008-2010 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief A generic timed mode driver
 *
 * This module implements a driver for timed modes.  Callers create
 * a structure 'timed_mode_ops' which describes the mode.  They should also
 * implement several event callbacks so that the mode is properly updated
 * on key events.
 */

#include <freewpc.h>


/**
 * Stop a mode immediately.
 */
static void timed_mode_exit_handler (struct timed_mode_ops *ops)
{
	if (in_live_game && ops->deff_ending)
		deff_queue_add (ops->deff_ending, TIME_5S);
	*ops->timer = 0;
	ops->exit ();
}


/**
 * A task that runs and implements a mode.
 */
void timed_mode_monitor (void)
{
	struct timed_mode_task_config *cfg;
	struct timed_mode_ops *ops;

	/* Get the mode operations / parameters structure */
	cfg = task_current_class_data (struct timed_mode_task_config);
	ops = cfg->ops;

#define the_timer (*(ops->timer))

	/* Request effect update immediately, now that the mode is started. */
	effect_update_request ();

	/* Run the mode timer.  Sleep a little more than 1s between loops,
	since task timing is a little inaccurate.  Run the user pause()
	hook to determine when the timer should stop.  Use system_timer_pause()
	in most cases. */
	while (the_timer > 0)
	{
		do {
			task_sleep (TIME_1S + TIME_33MS);
		} while (ops->pause ());
		the_timer--;
	}

	/* The timer has reached zero.  Call the timeout() routine right away. */
	ops->timeout ();

	/* Implement the grace period.  There's a minimum 1s delay before the
	display/music will be updated, even when the grace period is defined to be
	zero, to avoid an abrupt cutoff.  Any additional grace period defined is
	then also executed. */
	task_sleep_sec (1);
	effect_update_request ();
	if (ops->grace_timer > 1)
		task_sleep_sec (ops->grace_timer - 1);

	/* The mode is now officially over and cannot be extended.   From here
	on, no task switching is allowed, that could produce race conditions. */
	timed_mode_exit_handler (ops);
	task_exit ();
}


/**
 * Start a mode.
 */
void timed_mode_begin (struct timed_mode_ops *ops)
{
	task_pid_t tp;
	struct timed_mode_task_config *cfg;

#ifdef PARANOID
	/* Verify that init_timer, timer, prio are defined; there are
	no defaults for these */
#endif

	*ops->timer = ops->init_timer;
	if (ops->deff_starting)
		deff_start (ops->deff_starting);
	tp = task_create_gid1 (ops->gid, timed_mode_monitor);
	cfg = task_init_class_data (tp, struct timed_mode_task_config);
	cfg->ops = ops;
	ops->init ();
}


/**
 * Finish a mode.
 */
void timed_mode_finish (struct timed_mode_ops *ops)
{
	ops->finish ();
	timed_mode_exit_handler (ops);
	task_kill_gid (ops->gid);
}


/**
 * Get the current timer for a mode.  This can be zero for a mode that is in
 * its grace period.
 */
U8 timed_mode_get_timer (struct timed_mode_ops *ops)
{
	return *(ops->timer);
}


/**
 * Return whether or not a mode is running.  This can return TRUE if the mode
 * is in its grace period.
 */
bool timed_mode_running_p (struct timed_mode_ops *ops)
{
	return in_live_game && task_find_gid (ops->gid);
}


/**
 * Reset a running timed mode to its original timer value.
 * This is tricky because of grace periods.
 */
void timed_mode_reset (struct timed_mode_ops *ops, U8 time)
{
	/* If the mode isn't running, this call is a no-op. */
	if (!task_find_gid (ops->gid))
		return;

	/* If the timer is nonzero, we aren't in a grace period and can just
	update the timer directly.  But at zero, the mode task is probably not
	in its main loop anymore (it is at timeout() or later).  We need to
	restart the mode task from the beginning. */
	if (*ops->timer == 0)
	{
		/* Note: much of this is copied directly from timed_mode_begin() */
		task_pid_t tp;
		struct timed_mode_task_config *cfg;

		tp = task_recreate_gid (ops->gid, timed_mode_monitor);
		cfg = task_init_class_data (tp, struct timed_mode_task_config);
		cfg->ops = ops;
	}

	*ops->timer = time;
}


/**
 * Add to the mode timer.
 * The same dilemma for grace periods applies as with timed_mode_reset(), thus
 * this routine is just a frontend to that one.
 */
void timed_mode_add (struct timed_mode_ops *ops, U8 time)
{
	time += timed_mode_get_timer (ops);
	/* TODO - may need a maxval to prevent overflow.  Default should
	be the original mode time. */
	timed_mode_reset (ops, time);
}


/**
 * Enable the mode's music if it is active and not in its grace period.
 */
void timed_mode_music_refresh (struct timed_mode_ops *ops)
{
	if (ops->music && *ops->timer > 0 && task_find_gid (ops->gid))
		music_request (ops->music, ops->prio);
}


/**
 * Enable the mode's display effect if it is active and not in its grace period.
 */
void timed_mode_deff_update (struct timed_mode_ops *ops)
{
	if (ops->deff_running && *ops->timer > 0 && task_find_gid (ops->gid))
		deff_start_bg (ops->deff_running, ops->prio);
}


/* Global event handlers.  TODO: these handlers should iterate over all
defined mode objects (how to get this list?) */

CALLSET_ENTRY (all_timed_mode, music_refresh)
{
	/* timed_mode_music_refresh (&ops); */
}

CALLSET_ENTRY (all_timed_mode, display_update)
{
	/* timed_mode_deff_update (&ops); */
}

CALLSET_ENTRY (all_timed_mode, start_ball, end_ball, tilt)
{
	/* timed_mode_finish (&ops); */
}

