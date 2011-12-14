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
#include <trap_door.h>

/* This file implements the shot detection logic.  Its main purpose is to
	receive switch inputs and generate shot events.  The complexity arises
	from:
	1. Some shots require multiple switch closures to detect properly.
	2. Some switches are shared by multiple shots.
	3. Some shots are never to be generated during multiballs because
	   they cannot be detected reliably.
	4. Switch compensation can require a shot to be awarded from a totally
		different switch if the primary means of detection isn't reliable.

	Although possible to implement score rules here, it is discouraged
	except for the simplest possible circumstances: default switch score
	rules and default sounds.  For simple shots where there is only a
	single switch closure to generate it, nothing need be added here;
	the game rules logic can handle the switch event directly.

	For NTOM, calls to the combo rules are done directly from here
	because combos affect scoring of all other rules on the shot, and
	thus turning on the multiplier must be done first.  With regular
	callsets, order cannot be specified.
	*/

free_timer_id_t timer_left_ramp_entered;
free_timer_id_t timer_right_ramp_entered;
free_timer_id_t timer_loop_to_lock;
free_timer_id_t timer_subway_to_popper;

const char *shot_name;

void shot_deff (void)
{
	if (shot_name)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed6, 64, 16, shot_name);
		dmd_show_low ();
		task_sleep_sec (2);
	}
	shot_name = NULL;
	deff_exit ();
}

void combo_detect (U8 id);
void combo_reset (void);

void shot_define (const char *name)
{
	shot_name = name;
	deff_start (DEFF_SHOT);
}

CALLSET_ENTRY (shot, init, start_ball)
{
	shot_name = NULL;
}

CALLSET_ENTRY (shot, sw_left_lane_enter)
{
	/* TBD - debounce this, and don't generate it
	on a vanish kickout or right orbit passthrough. */
	shot_define ("LEFT ORBIT");
	combo_detect (0);
	callset_invoke (left_orbit_shot);
}

CALLSET_ENTRY (shot, sw_left_lane_exit)
{
	/* this switch is on the orbit return ramp */
}

CALLSET_ENTRY (shot, sw_center_ramp_enter)
{
	/* do sound/lamp effect */
}

CALLSET_ENTRY (shot, sw_center_ramp_exit)
{
	shot_define ("CENTER RAMP");
	combo_detect (1);
	callset_invoke (center_ramp_shot);
}

CALLSET_ENTRY (shot, sw_right_ramp_enter)
{
	if (!free_timer_test (timer_right_ramp_entered))
	{
		free_timer_start (timer_right_ramp_entered, TIME_4S);
	}
	else
	{
		free_timer_stop (timer_right_ramp_entered);
	}
}

CALLSET_ENTRY (shot, sw_right_ramp_exit_1, sw_right_ramp_exit_2)
{
	shot_define ("RIGHT RAMP");
	combo_detect (2);
	callset_invoke (right_ramp_shot);
}


/* Right orbit shot not awarded on falloff from the bumpers */
CALLSET_ENTRY (shot, sw_right_lane_enter)
{
	shot_define ("RIGHT ORBIT");
	combo_detect (3);
	callset_invoke (right_orbit_shot);
}

/* TBD - handle better */

CALLSET_ENTRY (shot, sw_captive_ball_top)
{
	score (SC_25K);
	callset_invoke (captive_ball_hard_shot);
}

CALLSET_ENTRY (shot, sw_captive_ball_rest)
{
	if (!switch_poll_logical (SW_CAPTIVE_BALL_REST))
	{
		score (SC_25K);
		shot_define ("CAPTIVE BALL");
		combo_detect (4);
		callset_invoke (captive_ball_shot);
	}
}

CALLSET_ENTRY (shot, sw_loop_left)
{
	if (!task_kill_gid (GID_RIGHT_LOOP_DEBOUNCE))
	{
		shot_define ("LEFT LOOP"); /* not working */
		combo_detect (5);
		callset_invoke (left_loop_shot);
		timer_restart_free (GID_LEFT_LOOP_DEBOUNCE, TIME_3S);
		free_timer_restart (timer_loop_to_lock, TIME_3S);
	}
}

CALLSET_ENTRY (shot, sw_spinner_start)
{
	/* TBD - Also require left loop switch to be seen before awarding
	the right loop - spinner by itself is not enough */
	if (!task_kill_gid (GID_LEFT_LOOP_DEBOUNCE) &&
			!task_find_gid (GID_RIGHT_LOOP_DEBOUNCE) &&
			!free_timer_test (timer_right_ramp_entered))
	{
		shot_define ("RIGHT LOOP");
		combo_detect (6);
		callset_invoke (right_loop_shot);
		timer_restart_free (GID_RIGHT_LOOP_DEBOUNCE, TIME_3S);
		free_timer_restart (timer_loop_to_lock, TIME_3S);
	}
}

CALLSET_ENTRY (shot, sw_spinner_slow)
{
	sound_send (SND_RIFFLE);
	score (SC_170);
}

CALLSET_ENTRY (shot, sw_trunk_hit)
{
	if (!task_kill_gid (GID_TRUNK_DEBOUNCE))
	{
		shot_define ("TRUNK WALL");
		combo_detect (7);
		sound_send (SND_TRUNK_HIT);
		callset_invoke (trunk_wall_shot);
		timer_restart_free (GID_TRUNK_DEBOUNCE, TIME_2S);
	}
}

CALLSET_ENTRY (shot, sw_subway_opto)
{
	free_timer_start (timer_loop_to_lock, TIME_4S);
}

CALLSET_ENTRY (shot, sw_subway_micro)
{
	if (free_timer_test (timer_loop_to_lock))
		return;
	else if (task_find_gid (GID_TRUNK_DEBOUNCE))
	{
		shot_define ("TRUNK HOLE");
		combo_detect (7);
		callset_invoke (trunk_hole_shot);
		/* TBD - if back diverter pin is not working, then a front
		shot can be used to spot a back shot */
	}
}

CALLSET_ENTRY (shot, dev_subway_enter)
{
	if (free_timer_test (timer_loop_to_lock))
	{
		shot_define ("TRUNK BACK");
		callset_invoke (trunk_back_shot);
	}
}

CALLSET_BOOL_ENTRY (shot, dev_subway_kick_request)
{
	/* return FALSE if there's already a ball in the
	popper */
	return TRUE;
}

CALLSET_ENTRY (shot, dev_subway_kick_attempt)
{
	free_timer_restart (timer_subway_to_popper, TIME_3S);
}

CALLSET_ENTRY (shot, dev_popper_enter)
{
	if (!free_timer_test (timer_subway_to_popper))
	{
		shot_define ("BASEMENT");
		callset_invoke (basement_shot);
	}
}

CALLSET_ENTRY (shot, dev_popper_kick_attempt)
{
	trap_door_start ();
}

CALLSET_ENTRY (shot, dev_popper_kick_success, dev_popper_kick_failure)
{
	trap_door_stop ();
}

CALLSET_ENTRY (shot, sw_top_jet, sw_bottom_jet, sw_middle_jet)
{
	score (SC_170);
	free_timer_restart (TIM_PAUSE_TIMERS, TIME_250MS);
}

CALLSET_ENTRY (shot, sw_left_sling, sw_right_sling)
{
	score (SC_170);
}

CALLSET_ENTRY (shot, sw_left_inlane, sw_right_inlane)
{
	score (SC_1K);
}

