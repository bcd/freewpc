/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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


/* CALLSET_SECTION ( loopmag, __machine3__ ) */
#include <freewpc.h>
#include <left_magnet_hold.h>

/* Checks for the powerball should be done when the FLAG_*_MAGNET_GRAB
 * flags are set, so we can grab during a multiball with the powerball out
 */

U8 left_magnet_hold_time;

void left_magnet_hold_task (void)
{
	left_magnet_hold_start ();
	task_sleep_sec (left_magnet_hold_time);
	left_magnet_hold_stop ();
	flag_off (FLAG_LEFT_MAGNET_HOLD);
	task_exit ();
}

CALLSET_ENTRY (loopmag, sw_left_magnet)
{
	if (flag_test (FLAG_LEFT_MAGNET_GRAB))
	{
		sol_request (SOL_LEFT_MAGNET);
		flag_off (FLAG_LEFT_MAGNET_GRAB);
	}
	/* Give it time to grab the ball before holding */
	task_sleep (TIME_66MS);
	if (flag_test (FLAG_LEFT_MAGNET_HOLD))
	{
		task_recreate_gid (GID_LEFT_MAGNET_HOLD, left_magnet_hold_task);
	}
}

CALLSET_ENTRY (loopmag, sw_right_magnet)
{
	if (flag_test (FLAG_RIGHT_MAGNET_GRAB))
	{
		sol_request (SOL_RIGHT_MAGNET);
		flag_off (FLAG_RIGHT_MAGNET_GRAB);
	}
}

CALLSET_ENTRY (loopmag, start_game)
{
	/* Default to 3 second hold */
	left_magnet_hold_time = 3;
	flag_off (FLAG_LEFT_MAGNET_GRAB);
	flag_off (FLAG_LEFT_MAGNET_HOLD);
}
