/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

void enter_loop (void)
{
	if (in_live_game)
	{
		score (SC_1K);
		sound_send (SND_LOOP_ENTER);
	}
}


void award_loop (void)
{
	if (flag_test (FLAG_POWERBALL_IN_PLAY))
	{
		deff_start (DEFF_PB_LOOP);
		/* TODO : count powerball loops and award bonuses
		at certain levels */
		sound_send (SND_SPIRAL_BREAKTHRU);
		sound_send (SND_POWERBALL_QUOTE);
		score (SC_10M);
	}

	if (lamp_test (LM_PANEL_SPIRAL))
	{
		sound_send (SND_SPIRAL_ROUND_AWARD_3);
		score (SC_10M);
	}
	else
	{
		score (SC_100K);
		sound_send (SND_SPIRAL_AWARDED);
	}
}


void abort_loop (void)
{
	if (in_live_game)
	{
		score (SC_1K);
		sound_send (SND_SPIRAL_SAME_SIDE_EXIT);
	}
}


void award_left_loop (void)
{
	if (in_live_game)
	{
		leff_start (LEFF_LEFT_LOOP);
		award_loop ();
	}
}


void award_right_loop (void)
{
	if (in_live_game)
	{
		leff_start (LEFF_RIGHT_LOOP);
		award_loop ();
	}
}



CALLSET_ENTRY (loop, sw_left_magnet)
{
	if (task_kill_gid (GID_LEFT_LOOP_ENTERED))
	{
		/* Left loop aborted */
		abort_loop ();
	}
	else if (task_kill_gid (GID_RIGHT_LOOP_ENTERED))
	{
		/* Right loop completed */
		award_right_loop ();
	}
	else
	{
		/* Left loop started */
		timer_restart_free (GID_LEFT_LOOP_ENTERED, TIME_3S);
		enter_loop ();
	}
}


CALLSET_ENTRY (loop, sw_upper_right_magnet)
{
	/* Could autodetect present of third magnet using this switch? */
}


CALLSET_ENTRY (loop, sw_lower_right_magnet)
{
	extern void sw_gumball_right_loop_entered (void);

	if (event_did_follow (dev_lock_kick_attempt, right_loop))
	{
		return;
	}
	else if (event_did_follow (autolaunch, right_loop))
	{
		/* Ignore right loop switch after an autolaunch */
		enter_loop ();
	}
	else if (task_kill_gid (GID_LEFT_LOOP_ENTERED))
	{
		/* Left loop completed */
		award_left_loop ();
	}
	else if (task_kill_gid (GID_RIGHT_LOOP_ENTERED))
	{
		/* Right loop aborted */
		abort_loop ();
	}
	else
	{
		/* Right loop started */
		timer_restart_free (GID_RIGHT_LOOP_ENTERED, TIME_3S);
		enter_loop ();
	}

	/* Inform gumball module that a ball may be approaching */
	sw_gumball_right_loop_entered ();
}


CALLSET_ENTRY (loop, end_ball)
{
	timer_kill_gid (GID_LEFT_LOOP_ENTERED);
	timer_kill_gid (GID_RIGHT_LOOP_ENTERED);
}

