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

static void handle_outlane (void)
{
	/* Start a timer to tell the difference between an outlane
	 * drain and a center drain when the ball reaches the trough. */
	event_can_follow (any_outlane, center_drain, TIME_7S);
}

CALLSET_ENTRY (lanes, sw_left_outlane)
{
	score (SC_10K);
	handle_outlane ();
}

CALLSET_ENTRY (lanes, sw_right_outlane)
{
	score (SC_10K);
	handle_outlane ();
}

CALLSET_ENTRY (lanes, sw_left_inlane_1)
{
	score (SC_1K);
	timer_restart_free (GID_TIMED_RIGHT_LOOP_2X, TIME_3S);
	event_can_follow (left_inlane_1, right_loop, TIME_3S);
}

CALLSET_ENTRY (lanes, sw_left_inlane_2)
{
	score (SC_1K);
}

CALLSET_ENTRY (lanes, sw_right_inlane)
{
	score (SC_1K);
	timer_restart_free (GID_TIMED_LEFT_RAMP_2X, TIME_6S);
	timer_restart_free (GID_TIMED_LEFT_LOOP_2X, TIME_3S);
	lamp_on (LM_DEAD_END);
	event_can_follow (right_inlane, left_loop, TIME_3S);
}


CALLSET_ENTRY(lanes, start_player)
{
	lamp_on (LM_LEFT_INLANE1);
	lamp_on (LM_RIGHT_INLANE);
}

