/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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


/** Nonzero if the next game started will be played with tournament rules. */
U8 tournament_mode_enabled;


void tournament_player_detect (void)
{
	U8 hold = TIME_5S / TIME_100MS;

	while (hold > 0)
	{
		if (!switch_poll_logical (SW_LEFT_BUTTON))
		{
			task_exit ();
		}
		task_sleep (TIME_100MS);
		hold--;
	}

	tournament_mode_enabled = ON;
	deff_start (DEFF_PLAYER_TOURNAMENT_READY);
	task_exit ();
}


void tournament_check_player_enable (void)
{
	if (!in_game && !in_test)
	{
		task_recreate_gid (GID_PLAYER_TOURNAMENT_DETECT,
			tournament_player_detect);
	}
}


CALLSET_ENTRY (tournament, sw_left_button)
{
	tournament_check_player_enable ();
}


CALLSET_ENTRY (tournament, start_game)
{
	tournament_mode_enabled |= system_config.tournament_mode;
}


CALLSET_ENTRY (tournament, init, amode_start)
{
	tournament_mode_enabled = OFF;
}

