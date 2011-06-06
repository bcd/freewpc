/*
 * Copyright 2006-2010 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (bttz, __machine3__) */

#include <freewpc.h>

/* This being non-zero indicates that a BTTZ is running or has ran and we need
 * to fill the gumball back up again 
 * We can't use GLOBAL_FLAG_BTTZ_RUNNING as it gets unset when going back to single ball play */
U8 balls_needed_to_load;

score_t bttz_start_score;
score_t bttz_total_score;

extern bool gumball_enable_from_trough;

bool hold_balls_in_autofire;

void bttz_running_deff (void)
{
	for (;;)
	{
		score_update_start ();
		dmd_alloc_pair ();
		dmd_clean_page_low ();
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		sprintf ("LIVE BALLS %d", live_balls);
		//font_render_string_center (&font_var5, 64, 27, "HIT ANYTHING");
		font_render_string_center (&font_var5, 64, 27, sprintf_buffer);
		dmd_copy_low_to_high ();
		font_render_string_center (&font_fixed6, 64, 4, "BACK TO THE ZONE");
		dmd_show_low ();
		while (!score_update_required ())
		{
			task_sleep (TIME_133MS);
			dmd_show_other ();
		}
	}
}

void tbc_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	
	font_render_string_center (&font_fixed6, 64, 4, "TO BE CONTINUED");
	dmd_show_low ();
	task_sleep_sec (1);
	font_render_string_center (&font_var5, 64, 12, "FOR NOW, HAVE 100M");
	sound_send (SND_KACHING);
	dmd_sched_transition (&trans_bitfade_slow);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}
	
void bttz_end_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	
	font_render_string_center (&font_fixed6, 64, 4, "BTTZ OVER");
	sprintf_score (bttz_total_score);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	
	while (balls_needed_to_load)
		task_sleep (TIME_100MS);
	deff_exit ();
}

void bttz_start_task (void)
{
	global_flag_on (GLOBAL_FLAG_BTTZ_RUNNING);
	flag_off (FLAG_PIANO_DOOR_LIT);
	flag_off (FLAG_SLOT_DOOR_LIT);
	score_zero (bttz_start_score);
	score_copy (bttz_start_score, current_score);
	
	callset_invoke (empty_balls_test);
	device_request_empty (device_entry (DEVNO_LOCK));
	set_ball_count (6);
	balls_needed_to_load = 3;
	task_exit ();
}

CALLSET_ENTRY (bttz, door_start_bttz)
{
	//task_create_anon (bttz_start_task);
	deff_start (DEFF_TBC);
	score (SC_100M);
	callset_invoke (reset_door);
}

/* Once the ballsave has run out, start holding balls in the autofire */
CALLSET_ENTRY (bttz, sw_outhole)
{
	if (!ballsave_test_active () && balls_needed_to_load && in_live_game)
	{
		hold_balls_in_autofire = TRUE;
		if (!task_find_gid (GID_FAR_LEFT_TROUGH_MONITOR))
			callset_invoke (start_far_left_trough_monitor);
	}
}

void load_gumball_task (void)
{
	while (balls_needed_to_load)
	{
		hold_balls_in_autofire = FALSE;
		timer_restart_free (GID_LOAD_ATTEMPT, TIME_8S);
		magnet_disable_catch (MAG_RIGHT);
		/* Clear the autofire if found full */
		if (switch_poll_logical (SW_AUTOFIRE2))
		{
			gumball_enable_from_trough = TRUE;
			callset_invoke (clear_autofire);
		}
		else
			gumball_load_from_trough ();
		task_sleep_sec (8);
	}
	task_exit ();
}

CALLSET_ENTRY (bttz, sw_gumball_enter)
{
	if (task_kill_gid (GID_LOAD_ATTEMPT))
	{
		bounded_decrement (balls_needed_to_load, 0);
	}
}

CALLSET_ENTRY (bttz, music_refresh)
{
	if (global_flag_test (GLOBAL_FLAG_BTTZ_RUNNING))
		music_request (MUS_MULTIBALL, PRI_GAME_MODE1 + 12);
}

CALLSET_ENTRY (bttz, display_update)
{
	if (global_flag_test (GLOBAL_FLAG_BTTZ_RUNNING))
		deff_start_bg (DEFF_BTTZ_RUNNING, 0);
}

CALLSET_ENTRY (bttz, single_ball_play)
{
	global_flag_off (GLOBAL_FLAG_BTTZ_RUNNING);
	deff_stop (DEFF_BTTZ_RUNNING);
	lamplist_apply (LAMPLIST_DOOR_PANELS_AND_HANDLE, lamp_flash_off);
	flag_on (FLAG_SLOT_DOOR_LIT);
	callset_invoke (door_enable);
	music_refresh ();
}

CALLSET_ENTRY (bttz, end_ball)
{
	if (balls_needed_to_load)
	{
		task_create_gid (GID_LOAD_GUMBALL_TASK, load_gumball_task);
		score_copy (bttz_total_score, current_score);
		score_sub (bttz_total_score, bttz_start_score);
		deff_start_sync (DEFF_BTTZ_END);
	}
}

CALLSET_ENTRY (bttz, init)
{
	balls_needed_to_load = 0;
	hold_balls_in_autofire = FALSE;
}

CALLSET_ENTRY (bttz, end_game)
{
	balls_needed_to_load = 0;
	hold_balls_in_autofire = FALSE;
}

CALLSET_ENTRY (bttz, start_ball)
{
	balls_needed_to_load = 0;
	hold_balls_in_autofire = FALSE;
}

CALLSET_ENTRY (bttz, amode_start)
{
	balls_needed_to_load = 0;
	hold_balls_in_autofire = FALSE;
}
