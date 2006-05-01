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
	0x00, 0x00, 0x08, 0x3F, 0x1F, 0x07, 0x00, 0x00, 0x00, 0xFF,
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
	0x00, 0x00, 0x00, 0x1F, 0x12, 0x38, 0x00, 0xC0, 0x00, 0x00,
};

void wcs_init (void)
{
	#include <init.callset>
}

CALLSET_ENTRY (unused, init) {}
CALLSET_ENTRY (unused, start_game) {}
CALLSET_ENTRY (unused, end_game) {}
CALLSET_ENTRY (unused, start_ball) {}


void wcs_start_game (void)
{
	#include <start_game.callset>
	task_sleep_sec (2);
}


void wcs_end_game (void)
{
	#include <end_game.callset>
	if (!in_test)
		music_set (MUS_MAIN_DRUM_ENTRY);
}

void wcs_start_ball (void)
{
	music_set (MUS_MULTIBALL_LIT_PLUNGER);
	#include <start_ball.callset>
}

void wcs_ball_in_play (void)
{
	music_set (MUS_MULTIBALL_LIT);
	/* start ballsaver if enabled */
}

bool wcs_end_ball (void)
{
	sound_reset ();
	return TRUE;
}


void wcs_add_player (void)
{
}


void wcs_any_pf_switch (void)
{
}


void wcs_bonus (void)
{
	deff_start (DEFF_BONUS);
	leff_start (LEFF_BONUS);
	task_sleep_sec (1);
	while (deff_get_active () == DEFF_BONUS)
		task_sleep (TIME_33MS);
	leff_stop (LEFF_BONUS);
}


void wcs_tilt (void)
{
}


void wcs_tilt_warning (void)
{
}


void wcs_start_without_credits (void)
{
}

void wcs_coin_added (void)
{
}

machine_hooks_t wcs_hooks = {
	INHERIT_FROM_DEFAULT_HOOKS,
	.start_game = wcs_start_game,
	.end_game = wcs_end_game,
	.start_ball = wcs_start_ball,
	.ball_in_play = wcs_ball_in_play,
	.end_ball = wcs_end_ball,
	.add_player = wcs_add_player,
	.init = wcs_init,
	.bonus = wcs_bonus,
	.tilt = wcs_tilt,
	.tilt_warning = wcs_tilt_warning,
	.any_pf_switch = wcs_any_pf_switch,
	.start_without_credits = wcs_start_without_credits,
	.coin_added = wcs_coin_added,
};

