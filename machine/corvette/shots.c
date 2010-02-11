/*
 * Copyright 2010 by Dominic Clifton <me@dominicclifton.name>
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

void left_outer_loop_enter (void)
{
	if (!single_ball_play ())
		return;

	if (lamp_flash_test (LM_LITE_LOCK)) {
		lamp_flash_on (LM_ZR_1_RAMP_ARROW);
		lamp_on (LM_ZR_1_RAMP_LOCK);
		lamp_flash_off (LM_LITE_LOCK);
	}
}

void right_outer_loop_enter (void)
{
}

void left_orbit_task (void)
{
	task_sleep_sec (5);
	task_exit ();
}

void right_orbit_task (void)
{
	task_sleep_sec (5);
	task_exit ();
}


CALLSET_ENTRY (shot, sw_left_outer_loop)
{
	left_outer_loop_enter ();
	if (!single_ball_play ())
		return;

	if (task_kill_gid (GID_RIGHT_ORBIT_MADE)) {
		callset_invoke (right_orbit_shot);
		return;
	}

	task_create_gid1 (GID_LEFT_ORBIT_MADE, left_orbit_task);
}

CALLSET_ENTRY (shot, sw_right_outer_loop)
{
	right_outer_loop_enter ();
	if (!single_ball_play ())
		return;

	if (task_kill_gid (GID_LEFT_ORBIT_MADE)) {
		callset_invoke (left_orbit_shot);
		return;
	}

	task_create_gid1 (GID_RIGHT_ORBIT_MADE, right_orbit_task);
}


CALLSET_ENTRY (shot, sw_left_rollover, sw_middle_rollover, sw_right_rollover)
{
	if (task_kill_gid (GID_LEFT_ORBIT))
		callset_invoke (left_orbit_to_rollover_shot);
	else if (task_kill_gid (GID_RIGHT_ORBIT))
		callset_invoke (right_orbit_to_rollover_shot);
}


CALLSET_ENTRY (shot, left_orbit_shot) {
	sound_start (ST_SAMPLE, SND_TRAFFIC_ZOOM_01, SL_2S, PRI_GAME_QUICK1);
	score (SC_50K);
}

CALLSET_ENTRY (shot, right_orbit_shot) {
	sound_start (ST_SAMPLE, SND_TRAFFIC_ZOOM_02, SL_2S, PRI_GAME_QUICK1);
	score (SC_50K);
}

CALLSET_ENTRY (shot, left_orbit_to_rollover_shot) {
	sound_start (ST_SAMPLE, SND_ENGINE_REV_04, SL_1S, PRI_GAME_QUICK1);
	score (SC_25K);
}

CALLSET_ENTRY (shot, right_orbit_to_rollover_shot) {
	sound_start (ST_SAMPLE, SND_ENGINE_REV_04, SL_1S, PRI_GAME_QUICK1);
	score (SC_25K);
}

