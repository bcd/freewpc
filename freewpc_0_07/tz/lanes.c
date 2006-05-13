/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

void handle_outlane (void)
{
	/* Reenable the ballsaver to its maximum timeout.
	 * The actual save is still triggered when the ball lands
	 * in the trough and calls endball.
	 */
	if (ballsave_test_active ())
	{
		ballsave_enable ();
		ball_search_timer_reset ();
	}
}

void sw_left_outlane_handler (void)
{
	score_add_current_const (SCORE_10K);
	handle_outlane ();
	if (flag_test (FLAG_LEFT_OUTLANE_LIT))
	{
	}
}

void sw_right_outlane_handler (void)
{
	score_add_current_const (SCORE_10K);
	handle_outlane ();
	if (flag_test (FLAG_RIGHT_OUTLANE_LIT))
	{
	}
}

void sw_left_inlane_1_handler (void)
{
	score_add_current_const (SCORE_1K);
	timer_restart_free (GID_TIMED_RIGHT_LOOP_2X, TIME_3S);
}

void sw_left_inlane_2_handler (void)
{
	extern void door_award_enable ();

	score_add_current_const (SCORE_1K);
	door_award_enable ();
	slot_update_lamps ();
}

void sw_right_inlane_handler (void)
{
	score_add_current_const (SCORE_1K);
	timer_restart_free (GID_TIMED_LEFT_RAMP_2X, TIME_6S);
	timer_restart_free (GID_TIMED_LEFT_LOOP_2X, TIME_3S);
}



DECLARE_SWITCH_DRIVER (sw_left_outlane)
{
	.fn = sw_left_outlane_handler,
	.flags = SW_PLAYFIELD,
	.sound = SND_DRAIN,
};

DECLARE_SWITCH_DRIVER (sw_right_outlane)
{
	.fn = sw_right_outlane_handler,
	.flags = SW_PLAYFIELD,
	.sound = SND_DRAIN,
};

DECLARE_SWITCH_DRIVER (sw_left_inlane_1)
{
	.fn = sw_left_inlane_1_handler,
	.flags = SW_PLAYFIELD,
	.sound = SND_INSIDE_LEFT_INLANE,
	.lamp = LM_LEFT_INLANE1,
};

DECLARE_SWITCH_DRIVER (sw_left_inlane_2)
{
	.fn = sw_left_inlane_2_handler,
	.flags = SW_PLAYFIELD,
	.sound = SND_INSIDE_LEFT_INLANE,
	.lamp = LM_LEFT_INLANE2,
};

DECLARE_SWITCH_DRIVER (sw_right_inlane)
{
	.fn = sw_right_inlane_handler,
	.flags = SW_PLAYFIELD,
	.sound = SND_INSIDE_LEFT_INLANE,
	.lamp = LM_RIGHT_INLANE,
};

