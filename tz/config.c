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

/** Filename: mach/config.c
 * Machine-specific functions.  These are all callbacks
 * from the main kernel code to handle various events
 * in game-specific ways.
 */


/** An array in which each 1 bit represents an opto switch
 * and 0 represents a normal switch.  This is used for
 * determining the logical level of a switch (open/closed)
 * rather than its actual level.
 *
 * The first byte is for the dedicated switches, next is
 * column 1, etc. to column 8.
 */
const uint8_t mach_opto_mask[] = {
	0x00, 0x00, 0x28, 0x0, 0x0, 0x10, 0x0, 0x3F, 0x5D, 0xFF,
};


/** An array in which each 0 bit indicates a 'normal'
 * playfield switch where the switch is serviced only
 * when it transitions from inactive->active.  When set
 * to a 1, the switch is also serviced on active->inactive
 * transitions.  These are called edge switches since
 * they 'trigger' on any edge transition.
 *
 * At a minimum, container switches need to be declared
 * as edge switches, since they must be handled whenever
 * they change state (the device count goes up or down).
 */
const uint8_t mach_edge_switches[] = {
	0x00, 0x70, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x98, 0x00,
};


U8 faster_quote_given;

CALLSET_ENTRY (tz, start_ball)
{
	faster_quote_given = 0;
}


CALLSET_ENTRY (tz, add_player)
{
#ifdef CONFIG_TZONE_IP
	if (num_players > 1)
		sound_send (SND_PLAYER_ONE + num_players - 1);
#endif
}


CALLSET_ENTRY (tz, bonus)
{
	deff_start (DEFF_BONUS);
	leff_start (LEFF_BONUS);
	task_sleep_sec (1);
	while (deff_get_active () == DEFF_BONUS)
		task_sleep (TIME_33MS);
	leff_stop (LEFF_BONUS);
}


CALLSET_ENTRY (tz, tilt)
{
	sound_send (SND_TILT);
	task_sleep_sec (3);
	sound_send (SND_OH_NO);
}


CALLSET_ENTRY (tz, tilt_warning)
{
	sound_send (SND_TILT_WARNING);
}


CALLSET_ENTRY (tz, start_without_credits)
{
	sound_send (SND_GREEEED);
}


CALLSET_ENTRY (tz, timed_game_tick)
{
	if (!in_live_game || in_bonus)
		return;
	switch (timed_game_timer)
	{
		case 10: 
			if (faster_quote_given == 0)
				sound_send (SND_FASTER); 
			faster_quote_given = 1;
			break;
		case 5: sound_send (SND_FIVE); break;
		case 4: sound_send (SND_FOUR); break;
		case 3: sound_send (SND_THREE); break;
		case 2: sound_send (SND_TWO); break;
		case 1: sound_send (SND_ONE); break;
		case 0: callset_invoke (music_update); break;
		default: break;
	}
}


CALLSET_ENTRY (tz, music_update)
{
#ifdef CONFIG_TIMED_GAME
	if (timed_game_timer == 0)
		music_change (MUS_MULTIBALL_LIT_PLUNGER);
	else
#endif
	if (flag_test (FLAG_MULTIBALL_RUNNING))
		music_change (MUS_MULTIBALL);
	else if (!ball_in_play)
		music_change (MUS_MULTIBALL_LIT_PLUNGER);
	else
		music_change (MUS_MULTIBALL_LIT);
}

