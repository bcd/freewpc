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

extern void start_spiralaward_timer (void);

#define ALL_INLANES_OFF 0x0
#define ALL_INLANES_ON 0x7
#define LEFT_INLANE1 0x1
#define LEFT_INLANE2 0x2
#define RIGHT_INLANE 0x4

/* Bitmask containing which rollovers are lit */
__local__ U8 rollovers_lit;

void rollover_completed_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 8, "ROLLOVER");
	font_render_string_center (&font_fixed6, 64, 16, "COMPLETED");
	dmd_show_low ();
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
	if (rollovers_lit == ALL_INLANES_ON)
		return TRUE;
	else
		return FALSE;
}

void award_rollover_completed (void)
{
	/* Show animation */
	deff_start (DEFF_ROLLOVER_COMPLETED);
	/* TODO Score it */
}

void check_rollover (U8 rollover_switch)
{
	switch (rollover_switch)
	{
		case LEFT_INLANE1:
			rollovers_lit |= LEFT_INLANE1;
			break;
		case LEFT_INLANE2:
			rollovers_lit |= LEFT_INLANE2;
			break;
		case RIGHT_INLANE:
			rollovers_lit |= RIGHT_INLANE;
			break;
	}
	
	/* TODO Update lamps here? */
	callset_invoke (rollover_lamp_update);
	/* Check to see if rollover has been completed */
	if (rollover_completed ())
		award_rollover_completed ();
}

CALLSET_ENTRY (lanes, rollover_lamp_update)
{
	/* Check and light lamps */
	if (rollovers_lit & LEFT_INLANE1)
		lamp_on (LM_LEFT_INLANE1);
	else
		lamp_off (LM_LEFT_INLANE1);

	if (rollovers_lit & LEFT_INLANE2)
		lamp_on (LM_LEFT_INLANE2);
	else
		lamp_off (LM_LEFT_INLANE2);

	if (rollovers_lit & RIGHT_INLANE)
		lamp_on (LM_RIGHT_INLANE);
	else
		lamp_off (LM_RIGHT_INLANE);
}

void shift_rollover_lamps_left (void)
{
	rollovers_lit = (rollovers_lit << 1) || (rollovers_lit >> 2);
}

void shift_rollover_lamps_right (void)
{
	rollovers_lit = (rollovers_lit >> 1) || (rollovers_lit << 2);
}

/* Flipper button handlers */
CALLSET_ENTRY (lanes, sw_left_button)
{
	/* TODO Update lamps here? */
	shift_rollover_lamps_left ();
	callset_invoke (rollover_lamp_update);
}

CALLSET_ENTRY (lanes, sw_right_button)
{
	/* TODO Update lamps here? */
	shift_rollover_lamps_right ();
	callset_invoke (rollover_lamp_update);
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
	event_can_follow (left_inlane_1, right_loop, TIME_3S);
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
	rollovers_lit = ALL_INLANES_OFF;
	lamp_off (LM_LEFT_INLANE1);
	lamp_off (LM_LEFT_INLANE1);
	lamp_off (LM_RIGHT_INLANE);
}

