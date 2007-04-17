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

/**
 * \file
 * \brief Tournament mode functions
 *
 */

#include <freewpc.h>


U8 tournament_mode_enabled;


void tournament_player_enable_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();
	deff_exit ();
}


void tournament_player_detect (void)
{
	U8 hold = TIME_5S / TIME_100MS;

	while (hold > 0)
	{
		if (!switch_poll_logical (SW_L_L_FLIPPER_BUTTON))
		{
			task_exit ();
		}
		task_sleep (TIME_100MS);
		hold--;
	}

	task_exit ();
}


void tournament_check_player_enable (void)
{
	if (!in_game && !in_test)
	{
		task_recreate_gid (GID_PLAYER_TOURNAMENT_DETECT, tournament_player_detect);
	}
}


CALLSET_ENTRY (tournament, sw_escape_button)
{
	if (in_game && tournament_mode_enabled)
	{
	}
}


CALLSET_ENTRY (tournament, sw_l_l_flipper_button)
{
	tournament_check_player_enable ();
}


CALLSET_ENTRY (tournament, start_game)
{
	tournament_mode_enabled |= system_config.tournament_mode;
}


CALLSET_ENTRY (tournament, end_game)
{
	tournament_mode_enabled = OFF;
}


CALLSET_ENTRY (tournament, init)
{
	tournament_mode_enabled = OFF;
}

