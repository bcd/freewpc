/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (dixon, __machine2__) */
#include <freewpc.h>

/*
 * Anti-cradling mechanism, as suggested by Philip Dixon
 * During multiball, if the player holds the ball on a flipper for longer than 5
 * seconds, the flipper is momentarily disabled, until the player releases the
 * button
 */
bool left_flipper_dixoned;
bool right_flipper_dixoned;

static void kill_flipper (U8 flipper_button)
{
	sound_send (SND_WITH_THE_DEVIL);
	deff_start (DEFF_ANTI_CRADLE);
	switch (flipper_button)
	{
		case SW_LEFT_BUTTON:
			callset_invoke (left_flipper_disable);
			left_flipper_dixoned = TRUE;
			break;
		case SW_RIGHT_BUTTON:
			callset_invoke (right_flipper_disable);
			right_flipper_dixoned = TRUE;
			break;
	}
}

/* Monitor the flipper button for 5 seconds */
static void anti_cradle_monitor (U8 flipper_button)
{
	U8 timer;
	timer = 0;
	while (switch_poll_logical (flipper_button)
		&& timer <= 50
		&& in_live_game)
	{
		task_sleep (TIME_100MS);
		bounded_increment (timer, 50);
		if (timer == 50)
			kill_flipper (flipper_button);
	}
}

static bool anti_cradle_enabled (void)
{
	if (in_live_game && global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING)
			&& feature_config.dixon_anti_cradle == YES)
		return TRUE;
	else
		return FALSE;
}

CALLSET_ENTRY (dixon, sw_left_button)
{
	if (left_flipper_dixoned == TRUE)
		callset_invoke (all_flippers_enable);
	else if (anti_cradle_enabled ())
		anti_cradle_monitor (SW_LEFT_BUTTON);
}

CALLSET_ENTRY (dixon, sw_right_button)
{
	if (right_flipper_dixoned == TRUE)
		callset_invoke (all_flippers_enable);
	else if (anti_cradle_enabled ())
		anti_cradle_monitor (SW_RIGHT_BUTTON);
}

/* Pretty sure that something else will take care of reenabling the flippers at the
 * start of a new ball */
CALLSET_ENTRY (dixon, start_ball, all_flippers_enable)
{
	left_flipper_dixoned = FALSE;
	right_flipper_dixoned = FALSE;
}

