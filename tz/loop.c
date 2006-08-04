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

void enter_loop (void)
{
	score (SC_1K);
	sound_send (SND_LOOP_ENTER);
}


void award_loop (void)
{
	if (lamp_test (LM_PANEL_SPIRAL))
	{
		sound_send (SND_SPIRAL_ROUND_AWARD_3);
		score (SC_250K);
	}
	else
	{
		score (SC_50K);
		sound_send (SND_SPIRAL_AWARDED);
	}
}


void abort_loop (void)
{
	score (SC_1K);
	sound_send (SND_SPIRAL_SAME_SIDE_EXIT);
}


void award_left_loop (void)
{
	award_loop ();
}


void award_right_loop (void)
{
	award_loop ();
}



void sw_left_loop_handler (void)
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


void sw_right_loop_top_handler (void)
{
}


void sw_right_loop_handler (void)
{
	/* Tell gumball module that ball is present */
	extern void sw_gumball_right_loop_entered (void);

	if (in_live_game)
	{
		if (event_did_follow (lock_exit, right_loop))
		{
			/* Ignore right loop switch after lock kickout */
			enter_loop ();
			return;
		}
		else if (event_did_follow (autolaunch, right_loop))
		{
			/* Ignore right loop switch after an autolaunch */
			enter_loop ();
			return;
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
			sw_gumball_right_loop_entered ();
		}
	}
}


DECLARE_SWITCH_DRIVER (sw_left_loop)
{
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.fn = sw_left_loop_handler,
};


DECLARE_SWITCH_DRIVER (sw_right_loop_top)
{
	.flags = SW_PLAYFIELD,
	.fn = sw_right_loop_top_handler,
};


DECLARE_SWITCH_DRIVER (sw_right_loop)
{
	.flags = SW_PLAYFIELD,
	.fn = sw_right_loop_handler,
};

