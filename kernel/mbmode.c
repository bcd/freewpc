/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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

U8 mb_mode_count;

/*	Called when the mode state changes.
	Updates the total count of multiball modes and invokes the mode's callback
	function. */
static void mb_mode_update (struct mb_mode_ops *ops, enum mb_mode_state state)
{
	/* Update the mode state if necessary */
	if (*(ops->state) == state)
		return;
	*(ops->state) = state;

	/* Inform the mode */
	if (ops->update)
		ops->update (state);

	/* Do common handling. */
	switch (state)
	{
		case MB_INACTIVE:
			mb_mode_count--;
			if (ops->deff_ending)
				deff_queue_add (ops->deff_ending, TIME_10S);
			break;

		case MB_ACTIVE:
			mb_mode_count++;
			break;

		default:
			break;
	}
}

/*	The default task that runs when the multiball mode is active
   (running, not in grace).  It doesn't do anything other than
	keep the GID in use.  Users can substitute their own task
	instead if someone needs to happen constantly while it's
	running (e.g. move a jackpot shot around or adjust shot values). */
void mb_mode_active_task (void)
{
	for (;;)
		task_sleep (TIME_4S);
	task_exit ();
}

/*	A task that runs when the multiball mode is in its grace period. */
static void mb_mode_grace_task (void)
{
	struct mb_mode_task_config *cfg =
		task_current_class_data (struct mb_mode_task_config);
	struct mb_mode_ops *ops = cfg->ops;
	/* TBD: implement grace-grace periods like TSPP/LOTR. */
	task_sleep (ops->grace_period);
	mb_mode_update (ops, MB_INACTIVE);
	task_exit ();
}

/*	Return the number of running multiball modes */
U8 mb_mode_running_count (void)
{
	return mb_mode_count;
}

/*	Start a multiball mode */
void mb_mode_start (struct mb_mode_ops *ops)
{
	if (mb_mode_running_p (ops))
		return;
	task_create_gid1 (ops->gid_running, ops->active_task);
	if (ops->deff_starting)
		deff_start (ops->deff_starting);
	mb_mode_update (ops, MB_ACTIVE);
}

/*	Restart a multiball mode that is in its grace period */
void mb_mode_restart (struct mb_mode_ops *ops)
{
	if (!mb_mode_in_grace_p (ops))
		return;
	task_create_gid1 (ops->gid_running, ops->active_task);
	task_kill_gid (ops->gid_in_grace);
	deff_stop (ops->deff_ending);
	mb_mode_update (ops, MB_ACTIVE);
}

/* This function should be called by a mode driver when the
	'single_ball_play' event occurs.  If the mode is active,
	this transitions it into its grace period.

	This function can also be called to implement 1-ball
	multiball.  If a machine only has 1 ball installed, then
	attempts to add balls for multiball play will all fail,
	but the mode will be started OK.  The rules should define
	some criteria for when this "fake multiball" should exit.
	When this occurs, you can check for the number of balls
	in play and if it is 1, then call mb_mode_single_ball()
	to stop the mode.  Note that it still gets a grace period! */
void mb_mode_single_ball (struct mb_mode_ops *ops)
{
	if (task_find_gid (ops->gid_running))
	{
		task_pid_t tp;
		struct mb_mode_task_config *cfg;

		task_kill_gid (ops->gid_running);

		tp = task_create_gid1 (ops->gid_in_grace, mb_mode_grace_task);
		cfg = task_init_class_data (tp, struct mb_mode_task_config);
		cfg->ops = ops;

		mb_mode_update (ops, MB_IN_GRACE);
	}
}

/* This function should be called by a mode driver when the
	'start_ball' event occurs.  It ensures that the mode state
	is set to INACTIVE, but does not schedule any effects as
	when it is stopped while it is running. */
void mb_mode_start_ball (struct mb_mode_ops *ops)
{
	*(ops->state) = MB_INACTIVE;
}

/* This function should be called by a mode driver when the
	'end_ball' event occurs.  It forcefully stops the mode
	without allowing for any grace period. */
void mb_mode_end_ball (struct mb_mode_ops *ops)
{
	task_kill_gid (ops->gid_running);
	task_kill_gid (ops->gid_in_grace);
	mb_mode_update (ops, MB_INACTIVE);
}

void mb_mode_music_refresh (struct mb_mode_ops *ops)
{
	if (ops->prio && mb_mode_effect_running_p (ops))
	{
		music_request (ops->music, ops->prio);
	}
}

void mb_mode_display_update (struct mb_mode_ops *ops)
{
	if (ops->prio && mb_mode_effect_running_p (ops))
		deff_start_bg (ops->deff_running, ops->prio);
}

CALLSET_ENTRY (mbmode, start_ball, end_ball)
{
	mb_mode_count = 0;
}

