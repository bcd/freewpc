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
	flag_off (FLAG_STEPS_RAMP_LIT);
	flag_off (FLAG_STEPS_OPEN);
	flag_off (FLAG_BALL_AT_STEPS);
}

bool steps_available_p (void)
{
	return in_live_game && !multiball_mode_running_p ();
}


CALLSET_ENTRY (steps, device_update)
{
	if (steps_available_p () && flag_test (FLAG_STEPS_OPEN))
	{
		steps_gate_start ();
	}
	else
	{
		steps_gate_stop ();
	}
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
	lamp_on_if (LM_RAMP_STEPS, flag_test (FLAG_STEPS_RAMP_LIT));

	if (flag_test (FLAG_BALL_AT_STEPS))
		lamp_tristate_flash (LM_STEPS_GATE_OPEN);
	else if (flag_test (FLAG_STEPS_OPEN))
		lamp_tristate_on (LM_STEPS_GATE_OPEN);
	else
		lamp_tristate_off (LM_STEPS_GATE_OPEN);
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
	ramp_div_stop ();
}

CALLSET_ENTRY (steps, sw_left_outlane)
{
}

CALLSET_ENTRY (steps, ramp_entered)
{
	if (flag_test (FLAG_STEPS_RAMP_LIT))
	{
		ramp_div_start ();
		flag_on (FLAG_STEPS_OPEN);
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
	flag_on (FLAG_STEPS_RAMP_LIT);
}

CALLSET_ENTRY (steps, start_player)
{
	flag_off (FLAG_STEPS_OPEN);
	lamplist_apply (LAMPLIST_STEPS_AWARDS, lamp_off);
}


CALLSET_ENTRY (steps, tilt, end_ball)
{
	ramp_div_stop ();
	flag_off (FLAG_STEPS_OPEN);
}

/* need ball search feature to open steps gate
in case ball is stuck in there */
