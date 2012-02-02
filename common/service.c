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

#include <freewpc.h>
#include <test.h>
#include <coin.h>
#include <search.h>

/**
 * \file
 * \brief Handle the service button switches inside the coin door.
 */

/* TODO : coin door may be closed, in which case button presses
ought to generate a warning message */

U8 live_balls_before_door_open;


static bool coin_door_warning_needed (void)
{
#ifdef CONFIG_COIN_DOOR_WARNING
	static bool already_warned = 0;

	if (already_warned && switch_poll_logical (SW_COIN_DOOR_CLOSED))
		return 0;

	if (!already_warned)
	{
		deff_start (DEFF_COIN_DOOR_BUTTONS);
		already_warned = 1;
	}
	return 1;
#else
	return 0;
#endif
}


CALLSET_ENTRY (service, sw_escape)
{
#ifndef CONFIG_BPT
	if (coin_door_warning_needed ())
		return;
	else if (!in_test)
	{
		add_credit ();
		audit_increment (&system_audits.service_credits);
		timestamp_update (&system_timestamps.last_service_credit);
	}
#endif
}

CALLSET_ENTRY (service, sw_down)
{
	if (coin_door_warning_needed ())
		return;
	else if (!in_test)
		button_invoke (SW_VOLUME_DOWN, volume_down, TIME_500MS, TIME_100MS);
		/* callset_invoke (volume_down) */
	else	
		test_down_button ();
}

CALLSET_ENTRY (service, sw_up)
{
	if (coin_door_warning_needed ())
		return;
	else if (!in_test)
		button_invoke (SW_VOLUME_UP, volume_up, TIME_500MS, TIME_100MS);
		/* callset_invoke (volume_up) */
	else
		test_up_button ();
}

/**************************************************************/

void coin_door_opened (void)
{
	global_flag_on (GLOBAL_FLAG_COIN_DOOR_OPENED);

	if (in_live_game && system_config.coin_door_ball_save)
	{
		dbprintf ("%d balls before coin door drain\n", live_balls);
		live_balls_before_door_open = live_balls;
		ball_search_monitor_stop ();
	}

	/* Print a message that high power coils are disabled */
	if (!in_test)
	{
		deff_start (DEFF_COIN_DOOR_POWER);
	}
}


void coin_door_closed (void)
{
	global_flag_off (GLOBAL_FLAG_COIN_DOOR_OPENED);

	if (in_live_game && system_config.coin_door_ball_save)
	{
#ifdef DEVNO_TROUGH
		dbprintf ("resetting to %d balls\n", live_balls_before_door_open);
		if (live_balls_before_door_open != live_balls)
		{
			set_ball_count (live_balls_before_door_open);
#ifdef DEFF_BALL_SAVE
		deff_start (DEFF_BALL_SAVE);
#endif
		}
#endif
		ball_search_monitor_start ();
		live_balls_before_door_open = 0;
	}
}


CALLSET_ENTRY (coin_door, amode_start)
{
	live_balls_before_door_open = 0;
}


CALLSET_ENTRY (coin_door, sw_coin_door_closed)
{
	/* Be kind and ignore slam tilt switch briefly after the
	coin door is opened/closed */
	event_can_follow (sw_coin_door_closed, sw_slam_tilt, TIME_5S);

	if (switch_poll_logical (SW_COIN_DOOR_CLOSED))
		coin_door_closed ();
	else
		coin_door_opened ();
}

CALLSET_BOOL_ENTRY (coin_door, ball_drain)
{
	if (live_balls_before_door_open)
	{
		dbprintf ("drain will not end ball\n");
		return FALSE;
	}
	else
		return TRUE;
}

