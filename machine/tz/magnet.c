/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
/* Code to catch ball with the loop magnets 
 * Currently using duty2 driver with these settings:
 * timeout=TIME_2S, ontime=TIME_200MS, duty_mask=DUTY_MASK_25
*/

#define LEFT_MAGNET 0
#define LOWER_RIGHT_MAGNET 0
#define UPPER_RIGHT_MAGNET 0

/* Function to activate the magnet grab flags for a specific length of time */
void magnet_grab_task (U8 magnet, U8 time)
{
	switch (magnet)
	{
		case LEFT_MAGNET:
			flag_on (FLAG_LEFT_MAGNET);
			task_sleep_sec (time);
			flag_off (FLAG_LEFT_MAGNET);
			break;
		case LOWER_RIGHT_MAGNET:
			flag_on (FLAG_LOWER_RIGHT_MAGNET);
			task_sleep_sec (time);
			flag_off (FLAG_LOWER_RIGHT_MAGNET);
			break;
		case UPPER_RIGHT_MAGNET:
			flag_on (FLAG_UPPER_RIGHT_MAGNET);
			task_sleep_sec (time);
			flag_off (FLAG_UPPER_RIGHT_MAGNET);
			break;
		default:
			flag_off (FLAG_LEFT_MAGNET);
			flag_off (FLAG_LOWER_RIGHT_MAGNET);
			flag_off (FLAG_UPPER_RIGHT_MAGNET);
			break;
	}
	task_exit ();
}

CALLSET_ENTRY (magnet, sw_lower_right_magnet)
{
	if (flag_test (FLAG_LOWER_RIGHT_MAGNET_GRAB))
	{
		sol_request (SOL_LOWER_RIGHT_MAGNET_GRAB);
		flag_off (FLAG_LOWER_RIGHT_MAGNET_GRAB);
	}
}

/* Not normally installed */
CALLSET_ENTRY (magnet, sw_upper_right_magnet)
{
	if (flag_test (FLAG_UPPER_RIGHT_MAGNET_GRAB))
	{
		sol_request (SOL_UPPER_RIGHT_MAGNET_GRAB);
		flag_off (FLAG_UPPER_RIGHT_MAGNET_GRAB);
	}
}

CALLSET_ENTRY (magnet, sw_left_magnet)
{
	if (flag_test (FLAG_LEFT_MAGNET_GRAB))
	{
		sol_request (SOL_LEFT_MAGNET_GRAB);
		flag_off (FLAG_LEFT_MAGNET_GRAB);
	}
}

CALLSET_ENTRY (magnet, init)
{
	flag_off (FLAG_LEFT_MAGNET_GRAB);
	flag_off (FLAG_UPPER_RIGHT_MAGNET_GRAB);
	flag_off (FLAG_LOWER_RIGHT_MAGNET_GRAB);
}
