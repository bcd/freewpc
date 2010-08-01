/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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

/* How many times the rollovers have been completed */
U8 rollover_count;

void rollover_completed_deff (void)
{
	seg_alloc_clean ();
	psprintf ("1 ROLLOVER", "%d ROLLOVERS", rollover_count);
	seg_write_row_center (0, sprintf_buffer);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();

}
static void handle_outlane (void)
{
	/* Start a timer to tell the difference between an outlane
	 * drain and a center drain when the ball reaches the trough. */
	event_can_follow (any_outlane, center_drain, TIME_7S);
	sound_send (SND_BALL_DRAIN);
//	deff_start (DEFF_BALL_DRAIN_OUTLANE);
	
//	if (!multi_ball_play ())
//		leff_start (LEFF_STROBE_DOWN);
}

static bool rollover_completed (void)
{
	if (lamp_test (LM_LEFT_OUTLANE)
		&& lamp_test (LM_LEFT_INLANE)
		&& lamp_test (LM_RIGHT_INLANE)
		&& lamp_test (LM_RIGHT_OUTLANE))
		return TRUE;
	else
		return FALSE;
}

static void award_rollover_completed (void)
{
	bounded_increment (rollover_count, 99);
	score (SC_1M);
	
	/* Show animation */
	deff_start (DEFF_ROLLOVER_COMPLETED);
	/* Flash and turn off inlane lamps */
	lamplist_apply (LAMPLIST_LANES, lamp_off);
	lamplist_apply (LAMPLIST_LANES, lamp_flash_on);
	task_sleep_sec (1);
	lamplist_apply (LAMPLIST_LANES, lamp_flash_off);
}

static void check_rollover (void)
{
	/* Check to see if rollover has been completed 
	 * and start the spiralaward timer if a set has been
	 * completed */
	if (rollover_completed () == TRUE)
	{
		award_rollover_completed ();
	}
}

/* Flipper button handlers */
CALLSET_ENTRY (lanes, sw_left_button)
{
	if (in_game)
		lamplist_rotate_previous (LAMPLIST_LANES, lamp_matrix);
}

CALLSET_ENTRY (lanes, sw_right_button)
{
	if (in_game)
		lamplist_rotate_next (LAMPLIST_LANES, lamp_matrix);
}

CALLSET_ENTRY (lanes, sw_left_outlane)
{
	lamp_on (LM_LEFT_OUTLANE);
	check_rollover ();
	score (SC_10K);
	handle_outlane ();
}

CALLSET_ENTRY (lanes, sw_right_outlane)
{
	lamp_on (LM_RIGHT_OUTLANE);
	check_rollover ();
	score (SC_10K);
	handle_outlane ();
}

CALLSET_ENTRY (lanes, sw_left_inlane)
{
	lamp_on (LM_LEFT_INLANE);
	check_rollover ();
	score (SC_1K);
}

CALLSET_ENTRY (lanes, sw_right_inlane)
{
	lamp_on (LM_RIGHT_INLANE);
	check_rollover ();
	score (SC_1K);
}

CALLSET_ENTRY (lanes, start_ball)
{
	/* Turn off all inlanes at start of ball */
	lamplist_apply (LAMPLIST_LANES, lamp_off);
	rollover_count = 0;
}
