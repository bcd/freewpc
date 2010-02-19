/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#define LEFT_INLANE1 0x1
#define LEFT_INLANE2 0x2
#define RIGHT_INLANE 0x4

/* How many times the rollovers have been completed */
/* TODO does this have to be __local__ ? */
__local__ U8 rollover_count;
__local__ U8 rollover_level;

extern void start_spiralaward_timer (void);

void rollover_completed_deff (void)
{
	sound_send (SND_GLASS_BREAKS);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 8, "ROLLOVER");
	font_render_string_center (&font_fixed6, 64, 17, "COMPLETED");
	dmd_show_low ();
	task_sleep_sec (1);
	deff_exit ();
}

static void handle_outlane (void)
{
	/* Start a timer to tell the difference between an outlane
	 * drain and a center drain when the ball reaches the trough. */
	event_can_follow (any_outlane, center_drain, TIME_7S);
}

bool rollover_completed (void)
{
	if (lamp_test (LM_LEFT_INLANE1)
		&& lamp_test (LM_LEFT_INLANE2)
		&& lamp_test (LM_RIGHT_INLANE))
	{	
		return TRUE;
		rollover_count++;
	}
	else
		return FALSE;
}

void award_rollover_completed (void)
{
	/* Show animation */
	deff_start (DEFF_ROLLOVER_COMPLETED);
	/* Turn off inlane lamps */
	lamplist_apply (LAMPLIST_INLANES, lamp_off);
	/* Score it */
	score (SC_1M);
}

void check_rollover (U8 rollover_switch)
{
	switch (rollover_switch)
	{
		case LEFT_INLANE1:
			lamp_on (LM_LEFT_INLANE1);
			break;
		case LEFT_INLANE2:
			lamp_on (LM_LEFT_INLANE2);
			break;
		case RIGHT_INLANE:
			lamp_on (LM_RIGHT_INLANE);
			break;
	}
	
	/* Check to see if rollover has been completed */
	if (rollover_completed ())
		award_rollover_completed ();
}

/* Flipper button handlers */
CALLSET_ENTRY (lanes, sw_left_button)
{
	lamplist_rotate_previous (LAMPLIST_INLANES, lamp_matrix);
}

CALLSET_ENTRY (lanes, sw_right_button)
{
	lamplist_rotate_next (LAMPLIST_INLANES, lamp_matrix);
}

/* 'Extra Ball' outlane */
CALLSET_ENTRY (lanes, sw_left_outlane)
{
	score (SC_10K);
	handle_outlane ();
}

/* 'Special' outlane */
CALLSET_ENTRY (lanes, sw_right_outlane)
{
	score (SC_10K);
	handle_outlane ();
}

/* 'Light Spiral' Lane */
CALLSET_ENTRY (lanes, sw_left_inlane_1)
{
	score (SC_1K);
	//timer_restart_free (GID_TIMED_RIGHT_LOOP_2X, TIME_3S);
	start_spiralaward_timer ();
	//event_can_follow (left_inlane_1, right_loop, TIME_3S);
	check_rollover (LEFT_INLANE1);
}

/* 'Light Slot Machine' Lane */
CALLSET_ENTRY (lanes, sw_left_inlane_2)
{
	score (SC_1K);
	start_spiralaward_timer ();
	check_rollover (LEFT_INLANE2);
}

/* 'Dead End' Lane */
CALLSET_ENTRY (lanes, sw_right_inlane)
{
	score (SC_1K);
//	timer_restart_free (GID_TIMED_LEFT_RAMP_2X, TIME_6S);
//	timer_restart_free (GID_TIMED_LEFT_LOOP_2X, TIME_3S);
	
	/* Light Dead end if not lit */
	if (!lamp_test (LM_DEAD_END))
		lamp_on (LM_DEAD_END);
	/* Start the timer for the left ramp */
	timer_restart_free (GID_LEFT_RAMP, TIME_3S);
	event_can_follow (right_inlane, left_loop, TIME_3S);
	check_rollover (RIGHT_INLANE);
}


CALLSET_ENTRY(lanes, start_ball)
{
	/* Turn off all inlanes at start of ball */
	lamplist_apply (LAMPLIST_INLANES, lamp_off);
	rollover_count = 0;
	rollover_level = 1;
}

