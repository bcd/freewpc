/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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


void stop_goalie (void)
{
	goalie_stop ();
}

void stop_goalie_left (void)
{
}

void stop_goalie_center (void)
{
}

void stop_goalie_right (void)
{
}

void start_goalie (void)
{
	if (feature_config.disable_goalie == NO)
	{
		goalie_start ();
	}
}

CALLSET_ENTRY (goalie_driver, start_ball)
{
	stop_goalie ();
}

CALLSET_ENTRY (goalie_driver, end_ball)
{
	stop_goalie ();
}

CALLSET_ENTRY (goalie_driver, end_game)
{
	stop_goalie ();
}

