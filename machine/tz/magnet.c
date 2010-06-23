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
<<<<<<< HEAD:machine/tz/magnet.c
#include <left_magnet_grab.h>
#include <upper_right_magnet_grab.h>
#include <lower_right_magnet_grab.h>

/* Code to catch ball with the loop magnets 
 * Currently using duty2 driver with these settings:
 * timeout=TIME_2S, ontime=TIME_50MS, duty_mask=DUTY_MASK_25
*/
=======
>>>>>>> ccc5c5ecf226f9c653e24731402b16220b5176e7:machine/tz/magnet.c

#define LEFT_MAGNET 0
#define LOWER_RIGHT_MAGNET 1
#define UPPER_RIGHT_MAGNET 2

/* Function to activate the magnet grab flags for a specific length of time */
void magnet_flag_task (U8 magnet, U8 seconds)
{
	switch (magnet)
	{
		case LEFT_MAGNET:
			flag_on (FLAG_LEFT_MAGNET_GRAB);
			task_sleep_sec (seconds);
			flag_off (FLAG_LEFT_MAGNET_GRAB);
			break;
		case LOWER_RIGHT_MAGNET:
			flag_on (FLAG_LOWER_RIGHT_MAGNET_GRAB);
			task_sleep_sec (seconds);
			flag_off (FLAG_LOWER_RIGHT_MAGNET_GRAB);
			break;
		case UPPER_RIGHT_MAGNET:
			flag_on (FLAG_UPPER_RIGHT_MAGNET_GRAB);
			task_sleep_sec (seconds);
			flag_off (FLAG_UPPER_RIGHT_MAGNET_GRAB);
			break;
		default:
			flag_off (FLAG_LEFT_MAGNET_GRAB);
			flag_off (FLAG_LOWER_RIGHT_MAGNET_GRAB);
			flag_off (FLAG_UPPER_RIGHT_MAGNET_GRAB);
			break;
	}
	task_exit ();
}
/* Function to hold the ball for a specific length of time */
void magnet_hold (U8 magnet, U8 hold_time)
{	
	switch (magnet)
	{
		case LEFT_MAGNET:
			//left_magnet_grab_start ();
			while (!switch_poll_logical (SW_LEFT_MAGNET) && hold_time > 0)
			{	
				task_sleep_sec (1);
				hold_time--;
			}
			flag_off (FLAG_LEFT_MAGNET_GRAB);
			left_magnet_grab_stop ();
			break;
		
		case LOWER_RIGHT_MAGNET:
			//lower_right_magnet_grab_start ();
			while (!switch_poll_logical (SW_LOWER_RIGHT_MAGNET) && hold_time > 0)
			{	
				task_sleep_sec (1);
				hold_time--;
			}
			flag_off (FLAG_LOWER_RIGHT_MAGNET_GRAB);
			lower_right_magnet_grab_stop ();
			break;
		
		case UPPER_RIGHT_MAGNET:
			//lower_right_magnet_grab_start ();
			while (!switch_poll_logical (SW_UPPER_RIGHT_MAGNET) && hold_time > 0)
			{	
				task_sleep_sec (1);
				hold_time--;
			}
			flag_off (FLAG_UPPER_RIGHT_MAGNET_GRAB);
			lower_right_magnet_grab_stop ();
			break;
		
		default:
			break;
	}
}

CALLSET_ENTRY (magnet, sw_lower_right_magnet)
{
	if (flag_test (FLAG_LOWER_RIGHT_MAGNET_GRAB))
		magnet_hold (LOWER_RIGHT_MAGNET, 2);
}

/* Not normally installed */
CALLSET_ENTRY (magnet, sw_upper_right_magnet)
{
	if (flag_test (FLAG_UPPER_RIGHT_MAGNET_GRAB))
		magnet_hold (UPPER_RIGHT_MAGNET, 2);
}

CALLSET_ENTRY (magnet, sw_left_magnet)
{
	if (flag_test (FLAG_LEFT_MAGNET_GRAB))
		magnet_hold (LEFT_MAGNET, 2);
}

CALLSET_ENTRY (magnet, init)
{
	flag_off (FLAG_LEFT_MAGNET_GRAB);
	flag_off (FLAG_UPPER_RIGHT_MAGNET_GRAB);
	flag_off (FLAG_LOWER_RIGHT_MAGNET_GRAB);
}
