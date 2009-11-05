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
#include <eb.h>
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

bool steps_exited (void)
{
	if (!timer_find_gid (GID_STEPS_EXITED))
	{
		flag_off (FLAG_STEPS_RAMP_LIT);
		flag_off (FLAG_STEPS_OPEN);
		flag_off (FLAG_BALL_AT_STEPS);
		timer_start_free (GID_STEPS_EXITED, TIME_5S);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
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
	lamp_on_if (LM_RAMP_STEPS, steps_available_p () && flag_test (FLAG_STEPS_RAMP_LIT));

	if (flag_test (FLAG_BALL_AT_STEPS))
		lamp_tristate_flash (LM_STEPS_GATE_OPEN);
	else if (steps_available_p () && flag_test (FLAG_STEPS_OPEN))
		lamp_tristate_on (LM_STEPS_GATE_OPEN);
	else
		lamp_tristate_off (LM_STEPS_GATE_OPEN);
}


CALLSET_ENTRY (steps, sw_step_track_upper)
{
}

CALLSET_ENTRY (steps, sw_step_track_lower)
{
}

CALLSET_ENTRY (steps, sw_step_exit)
{
}


CALLSET_ENTRY (steps, sw_step_extra_ball)
{
	if (steps_exited ())
	{
		if (lamp_flash_test (LM_STEPS_EB))
		{
			lamp_flash_off (LM_STEPS_EB);
			light_easy_extra_ball ();
		}
	}
}

CALLSET_ENTRY (steps, sw_step_500k)
{
	if (steps_exited ())
	{
		if (lamp_flash_test (LM_STEPS_500K))
		{
			lamp_flash_off (LM_STEPS_500K);
			score (SC_500K);
		}
	}
}

CALLSET_ENTRY (steps, sw_steps_frenzy)
{
	if (steps_exited ())
	{
		if (lamp_flash_test (LM_STEPS_FRENZY))
		{
			lamp_flash_off (LM_STEPS_FRENZY);
			flag_on (FLAG_FRENZY_LIT);
			sample_start (SND_FRENZY_LIT, SL_2S);
		}
	}
}


CALLSET_ENTRY (steps, sw_left_plunger)
{
	flag_on (FLAG_BALL_AT_STEPS);
	ramp_div_stop ();
}


CALLSET_ENTRY (steps, ramp_entered)
{
	if (steps_available_p () && flag_test (FLAG_STEPS_RAMP_LIT))
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

CALLSET_ENTRY (steps, sw_lower_right_hole)
{
	flag_on (FLAG_STEPS_RAMP_LIT);
}

CALLSET_ENTRY (steps, start_player)
{
	flag_off (FLAG_STEPS_OPEN);
	lamp_flash_on (LM_STEPS_FRENZY);
	lamplist_apply (LAMPLIST_STEPS_AWARDS, lamp_off);
}


CALLSET_ENTRY (steps, tilt, end_ball)
{
	ramp_div_stop ();
	flag_off (FLAG_STEPS_OPEN);
}

/* need ball search feature to open steps gate
in case ball is stuck in there */
