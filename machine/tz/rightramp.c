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


extern __local__ U8 mpf_enable_count;

U8 right_ramps_entered;


void right_ramp_default_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();
	deff_exit ();
}


void sw_right_ramp_enter_task (void)
{
	/* Decide whether to let the ball onto the mini-playfield,
	or dump it.  Do this once for each balls that enters the
	ramp. */
	do {
		if (mpf_enable_count)
		{
			U8 n;
			for (n = 0; n < 6; n++)
			{
				sol_start (SOL_RIGHT_RAMP_DIV, SOL_DUTY_50, TIME_1S);
				task_sleep (TIME_500MS);
			}
		}
		else
		{
			task_sleep_sec (2);
			sound_send (SND_RIGHT_RAMP_EXIT);

			sol_start (SOL_RIGHT_RAMP_DIV, SOL_DUTY_50, TIME_500MS);
			task_sleep (TIME_200MS);
			sol_start (SOL_RIGHT_RAMP_DIV, SOL_DUTY_25, TIME_1S);
		}
	} while (--right_ramps_entered > 0);
	task_exit ();
}


CALLSET_ENTRY (right_ramp, sw_right_ramp)
{
	/* Handle all balls entering the ramp unconditionally, so that
	they are disposed of properly. */
	right_ramps_entered++;
	task_recreate_gid_while (GID_RIGHT_RAMP_ENTERED,
		sw_right_ramp_enter_task, TASK_DURATION_INF);

	/* Scoring functions only happen during a game */
	if (!in_live_game)
		return;

	score (SC_10K);
	if (mpf_enable_count > 0)
		sound_send (SND_RAMP_ENTERS_POWERFIELD);
	else
		sound_send (SND_RIGHT_RAMP_DEFAULT_ENTER);
}


CALLSET_ENTRY (right_ramp, start_ball)
{
	right_ramps_entered = 0;
}

