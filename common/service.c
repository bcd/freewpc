/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Handle the service button switches inside the coin door.
 */

/* TODO : coin door may be closed, in which case button presses
ought to generate a warning message */

void coin_door_buttons_deff (void)
{
	dmd_alloc_low_clean ();
#if (MACHINE_DMD == 1)
	font_render_string_center (&font_var5, 64, 3, "COIN DOOR IS CLOSED");
#endif
	font_render_string_center (&font_var5, 64, 10, "OPEN COIN DOOR");
	font_render_string_center (&font_var5, 64, 17, "TO USE BUTTONS");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}


void coin_door_power_deff (void)
{
	U8 n;
	for (n=0; n < 5; n++)
	{
		dmd_alloc_low_clean ();
		dmd_show_low ();
		task_sleep (TIME_200MS);

		dmd_alloc_low_clean ();
#if (MACHINE_DMD == 1)
		font_render_string_center (&font_fixed6, 64, 6, "COIN DOOR IS OPEN");
		font_render_string_center (&font_fixed6, 64, 16, "HIGH POWER");
		font_render_string_center (&font_fixed6, 64, 26, "IS DISABLED");
#else
		font_render_string_center (&font_fixed6, 64, 10, "HIGH POWER");
		font_render_string_center (&font_fixed6, 64, 21, "IS DISABLED");
#endif
		dmd_show_low ();
		sound_send (SND_TEST_ALERT);
		task_sleep (TIME_300MS);
	}
	task_sleep_sec (3);
	deff_exit ();
}


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
	if (coin_door_warning_needed ())
		return;
	else if (!in_test)
	{
		add_credit ();
		audit_increment (&system_audits.service_credits);
	}
}

CALLSET_ENTRY (service, sw_down)
{
	if (coin_door_warning_needed ())
		return;
	else if (!in_test)
		button_invoke (SW_VOLUME_DOWN, volume_down, TIME_500MS, TIME_100MS);
	else	
		test_down_button ();
}

CALLSET_ENTRY (service, sw_up)
{
	if (coin_door_warning_needed ())
		return;
	else if (!in_test)
		button_invoke (SW_VOLUME_UP, volume_up, TIME_500MS, TIME_100MS);
	else
		test_up_button ();
}


void coin_door_opened (void)
{
	global_flag_on (GLOBAL_FLAG_COIN_DOOR_OPENED);

	/* TODO : Enable Stern's "coindoor ballsave" feature */

	/* Print a message that high power coils are disabled */
	if (!in_test)
	{
		deff_start (DEFF_COIN_DOOR_POWER);
	}
}


void coin_door_closed (void)
{
	global_flag_off (GLOBAL_FLAG_COIN_DOOR_OPENED);
}


CALLSET_ENTRY (service, sw_coin_door_closed)
{
	/* Be kind and ignore slam tilt switch briefly after the
	coin door is opened/closed */
	event_can_follow (sw_coin_door_closed, sw_slam_tilt, TIME_5S);

	if (switch_poll_logical (SW_COIN_DOOR_CLOSED))
		coin_door_closed ();
	else
		coin_door_opened ();
}

