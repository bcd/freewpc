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
	task_set_flags (TASK_PROTECTED);
	do {
		if (mpf_enable_count)
		{
			sol_on (SOL_RIGHT_RAMP_DIV);
			task_sleep_sec (4);
		}
		else
		{
			task_sleep_sec (2);
			sol_on (SOL_RIGHT_RAMP_DIV);
			sound_send (SND_RIGHT_RAMP_EXIT);
			task_sleep (TIME_100MS * 2);
			sol_off (SOL_RIGHT_RAMP_DIV);
		}
	} while (--right_ramps_entered > 0);
	sol_off (SOL_RIGHT_RAMP_DIV);
	task_exit ();
}


CALLSET_ENTRY (right_ramp, sw_right_ramp)
{
	if (!in_live_game)
		return;

	score (SC_10K);

	right_ramps_entered++;
	if (!task_find_gid (GID_RIGHT_RAMP_ENTERED))
		task_create_gid (GID_RIGHT_RAMP_ENTERED, sw_right_ramp_enter_task);

	if (mpf_enable_count > 0)
	{
		sound_send (SND_RAMP_ENTERS_POWERFIELD);
	}
	else
	{
		sound_send (SND_RIGHT_RAMP_DEFAULT_ENTER);
	}
}


CALLSET_ENTRY (right_ramp, start_ball)
{
	right_ramps_entered = 0;
}

