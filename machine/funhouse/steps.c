/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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
#include <steps_gate.h>
#include <ramp_div.h>

__machine__ bool multiball_mode_running_p (void);

void steps_monitor (void)
{
	task_sleep_sec (3);
	while (flag_test (FLAG_BALL_AT_STEPS))
		task_sleep (TIME_500MS);
	steps_gate_stop ();
	task_exit ();
}

void steps_exited (void)
{
	flag_off (FLAG_STEPS_LIT);
	flag_off (FLAG_BALL_AT_STEPS);
}

bool steps_lit_p (void)
{
	return flag_test (FLAG_STEPS_LIT) &&
		!multiball_mode_running_p ();
}


CALLSET_ENTRY (steps, music_refresh)
{
	if (flag_test (FLAG_BALL_AT_STEPS))
	{
		music_request (MUS_STEPS, PRI_GAME_MODE8);
	}
}

CALLSET_ENTRY (steps, lamp_update)
{
	if (flag_test (FLAG_LEFT_PLUNGER_OPEN))
	{
	}
	lamp_on_if (LM_RAMP_STEPS, steps_lit_p ());
}

CALLSET_ENTRY (steps, sw_step_track_upper)
{
	steps_exited ();
}

CALLSET_ENTRY (steps, sw_step_500k)
{
	steps_exited ();
}

CALLSET_ENTRY (steps, sw_step_track_lower)
{
	steps_exited ();
}

CALLSET_ENTRY (steps, sw_step_exit)
{
	steps_exited ();
}

CALLSET_ENTRY (steps, sw_steps_frenzy)
{
	steps_exited ();
	flag_on (FLAG_FRENZY_LIT);
}

CALLSET_ENTRY (steps, sw_left_plunger)
{
	flag_on (FLAG_BALL_AT_STEPS);
}

CALLSET_ENTRY (steps, sw_left_outlane)
{
}

CALLSET_ENTRY (steps, ramp_entered)
{
	if (lamp_test (LM_RAMP_STEPS))
	{
		ramp_div_start ();
		flag_on (FLAG_BALL_AT_STEPS);
		steps_gate_start ();
		task_recreate_gid (GID_STEPS_MONITOR, steps_monitor);
	}
}

CALLSET_ENTRY (steps, sw_upper_ramp)
{
	/* which switch is this? */
}

CALLSET_ENTRY (steps, sw_step_s)
{
}

CALLSET_ENTRY (steps, sw_step_t)
{
}

CALLSET_ENTRY (steps, sw_step_e)
{
}

CALLSET_ENTRY (steps, sw_step_p)
{
}

CALLSET_ENTRY (steps, sw_lower_right_hole)
{
	flag_on (FLAG_STEPS_LIT);
}

CALLSET_ENTRY (steps, start_player)
{
	flag_off (FLAG_STEPS_LIT);
}


CALLSET_ENTRY (steps, tilt, end_ball)
{
	ramp_div_stop ();
	steps_gate_stop ();
}

