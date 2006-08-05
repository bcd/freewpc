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


__local__ U8 battle_power_lit;


void right_ramp_default_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();
	deff_exit ();
}


void sw_right_ramp_powerfield_task (void)
{
	task_set_flags (TASK_PROTECTED);
	sol_on (SOL_RIGHT_RAMP_DIV);
	task_sleep_sec (5);
	sol_off (SOL_RIGHT_RAMP_DIV);
}


void sw_right_ramp_task (void)
{
	task_set_flags (TASK_PROTECTED);
	task_sleep_sec (2);
	sound_send (SND_RIGHT_RAMP_EXIT);
	sol_on (SOL_RIGHT_RAMP_DIV);
	task_sleep (TIME_100MS * 2);
	sol_off (SOL_RIGHT_RAMP_DIV);
	task_exit ();
}


void sw_right_ramp_handler (void)
{
	if (!in_live_game)
		return;

	score (SC_10K);

	if (!task_find_gid (GID_RIGHT_RAMP_ENTERED))
	{
		if (battle_power_lit)
		{
			sound_send (SND_RAMP_ENTERS_POWERFIELD);
			task_create_gid (GID_RIGHT_RAMP_ENTERED, 
				sw_right_ramp_powerfield_task);
		}
		else
		{
			sound_send (SND_RIGHT_RAMP_DEFAULT_ENTER);
			task_create_gid (GID_RIGHT_RAMP_ENTERED, sw_right_ramp_task);
		}
	}

	callset_invoke (right_ramp);
}


CALLSET (right_ramp, init)
{
	battle_power_lit = 0;
}


DECLARE_SWITCH_DRIVER (sw_right_ramp)
{
	.fn = sw_right_ramp_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};


