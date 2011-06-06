/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (lanes, __machine3__) */

/* How many times the rollovers have been completed */
U8 rollover_count;
extern __local__ bool spiralaward_set_completed;
extern __local__  U8 cameras_lit;

void rollover_completed_deff (void)
{
	dmd_alloc_low_clean ();
	if (rollover_count % 4 == 0)
	{
		font_render_string_center (&font_fixed6, 64, 8, "CAMERA");
		font_render_string_center (&font_fixed6, 64, 18, "LIT");
		sound_send (SND_CAMERA_PICTURE_EJECT_2);
	}
	else
	{
		font_render_string_center (&font_fixed6, 64, 8, "ROLLOVER");
		font_render_string_center (&font_fixed6, 64, 18, "COMPLETED");
		sound_send (SND_GLASS_BREAKS);
	}
	dmd_show_low ();
	task_sleep_sec (1);
	deff_exit ();
}

static void handle_outlane (void)
{
	/* Start a timer to tell the difference between an outlane
	 * drain and a center drain when the ball reaches the trough. */
	event_can_follow (any_outlane, center_drain, TIME_7S);
	deff_start (DEFF_BALL_DRAIN_OUTLANE);
	
	if (!multi_ball_play ())
		leff_start (LEFF_STROBE_DOWN);
}

static bool rollover_completed (void)
{
	if (lamp_test (LM_LEFT_INLANE1)
		&& lamp_test (LM_LEFT_INLANE2)
		&& lamp_test (LM_RIGHT_INLANE))
		return TRUE;
	else
		return FALSE;
}

static inline void award_rollover_completed (void)
{
	bounded_increment (rollover_count, 99);
	score (SC_1M);
	/* Hack as I can't be bothered to fix the sw_right_inlane on my table */	
	if (!lamp_test (LM_DEAD_END))
		lamp_on (LM_DEAD_END);
	/* Increment cameras_lit every 4 rollovers */
	if (rollover_count % 4 == 0)
		bounded_increment (cameras_lit, 99);
	/* Show animation */
	deff_start (DEFF_ROLLOVER_COMPLETED);
	/* Turn off inlane lamps */
	lamplist_apply (LAMPLIST_INLANES, lamp_off);
	lamplist_apply (LAMPLIST_INLANES, lamp_flash_on);
	task_sleep_sec (1);
	lamplist_apply (LAMPLIST_INLANES, lamp_flash_off);
}

static void check_rollover (void)
{
	/* Check to see if rollover has been completed 
	 * and start the spiralaward timer if a set has been
	 * completed */
	if (rollover_completed () == TRUE)
	{
		award_rollover_completed ();
		if (spiralaward_set_completed == TRUE)
			callset_invoke (start_spiralaward_timer);
	}
}

/* Flipper button handlers */
CALLSET_ENTRY (lanes, sw_left_button)
{
	if (in_game)
		lamplist_rotate_previous (LAMPLIST_INLANES, lamp_matrix);
}

CALLSET_ENTRY (lanes, sw_right_button)
{
	if (in_game)
		lamplist_rotate_next (LAMPLIST_INLANES, lamp_matrix);
}

/* 'Extra Ball' outlane */
CALLSET_ENTRY (lanes, sw_left_outlane)
{
	timer_restart_free (GID_BALL_DRAIN_OUTLANE, TIME_5S);
	score (SC_10K);
	handle_outlane ();
}

/* 'Special' outlane */
CALLSET_ENTRY (lanes, sw_right_outlane)
{
	timer_restart_free (GID_BALL_DRAIN_OUTLANE, TIME_5S);
	score (SC_10K);
	handle_outlane ();
}

/* 'Light Spiral' Lane */
CALLSET_ENTRY (lanes, sw_left_inlane_1)
{
	event_can_follow (left_or_right_inlane, either_outlane, TIME_1S);
	/* Start the spiralaward timer only if the lane has 
	 * just been lit */
	if (!lamp_test (LM_LEFT_INLANE1) && spiralaward_set_completed == FALSE)
	{
		callset_invoke (start_spiralaward_timer);
	}
	lamp_on (LM_LEFT_INLANE1);
	check_rollover ();
	score (SC_1K);
	event_can_follow (left_inlane_1, right_loop, TIME_3S);
}

/* 'Light Slot Machine' Lane */
CALLSET_ENTRY (lanes, sw_left_inlane_2)
{
	if (!lamp_test (LM_LEFT_INLANE2) && spiralaward_set_completed == FALSE)
	{	
		callset_invoke(start_spiralaward_timer);
	}
	lamp_on (LM_LEFT_INLANE2);
	check_rollover ();
	score (SC_1K);
	event_can_follow (left_inlane_2, right_loop, TIME_3S);
}

/* 'Dead End' Lane */
CALLSET_ENTRY (lanes, sw_right_inlane)
{
	event_can_follow (left_or_right_inlane, either_outlane, TIME_1S);
	lamp_on (LM_RIGHT_INLANE);
	check_rollover ();
	score (SC_1K);
	
	/* Light Dead end if not lit */
	lamp_on (LM_DEAD_END);
	timer_restart_free (GID_TNF_READY, TIME_4S);
	//event_can_follow (right_inlane, left_ramp, TIME_4S);
}

CALLSET_ENTRY (lanes, start_ball)
{
	/* Turn off all inlanes at start of ball */
	lamplist_apply (LAMPLIST_INLANES, lamp_off);
	rollover_count = 0;
}
