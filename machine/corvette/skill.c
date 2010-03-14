/*
 * Copyright 2010 by Dominic Clifton <me@dominicclifton.name>
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
 * @TODO Currently on a real machine the skill shot is turned off after the player launches a ball
 *       Ignore more switch presses?
 *
 * @TODO The flashing lamp for the active rollover isn't changed quickly enough.
 *       There can be up to about 1 second before the state is reflected by the lamp.
 *       Use something like: lamplist_rotate_previous (LAMPLIST_INLANES, lamp_matrix) instead?
 */
#include <freewpc.h>

U8 current_rollover; // there are 3
U8 counted_switches;

void skillshot_enable( void ) {
	counted_switches = 0;
	current_rollover = SW_MIDDLE_ROLLOVER;
	global_flag_on(GLOBAL_FLAG_SKILLSHOT_ENABLED);
	flag_off (FLAG_LOOP_GATE_OPENED);
}

void skillshot_disable( void ) {
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ENABLED)) {
		return;
	}

	global_flag_off ( GLOBAL_FLAG_SKILLSHOT_ENABLED );

	lamp_tristate_off (LM_LEFT_ROLLOVER);
	lamp_tristate_off (LM_MIDDLE_ROLLOVER);
	lamp_tristate_off (LM_RIGHT_ROLLOVER);

	// open the loop gate when the first pf switch is hit
	//task_sleep_sec(1);
	flag_on (FLAG_LOOP_GATE_OPENED);
}

void award_skillshot(U8 rollover_switch) {
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ENABLED)) {
		return;
	}

	if (rollover_switch == current_rollover) {
		score (SC_5M);
		// TODO display skillshot award.
	}

	skillshot_disable();
}

CALLSET_ENTRY (skill, lamp_update)
{
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ENABLED)) {
		return;
	}

	lamp_flash_if (LM_LEFT_ROLLOVER, current_rollover == SW_LEFT_ROLLOVER);
	lamp_flash_if (LM_MIDDLE_ROLLOVER, current_rollover == SW_MIDDLE_ROLLOVER);
	lamp_flash_if (LM_RIGHT_ROLLOVER, current_rollover == SW_RIGHT_ROLLOVER);
}

CALLSET_ENTRY (skill, sw_left_rollover)
{
	award_skillshot(SW_LEFT_ROLLOVER);
}

CALLSET_ENTRY (skill, sw_middle_rollover)
{
	award_skillshot(SW_MIDDLE_ROLLOVER);
}

CALLSET_ENTRY (skill, sw_right_rollover)
{
	award_skillshot(SW_RIGHT_ROLLOVER);
}

CALLSET_ENTRY (skill, sw_left_button) {
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ENABLED)) {
		return;
	}

	switch (current_rollover) {
		case SW_LEFT_ROLLOVER:
			current_rollover = SW_RIGHT_ROLLOVER;
		break;

		case SW_MIDDLE_ROLLOVER:
			current_rollover = SW_LEFT_ROLLOVER;
		break;

		case SW_RIGHT_ROLLOVER:
			current_rollover = SW_MIDDLE_ROLLOVER;
		break;

	}
	callset_invoke( lamp_update );
}

CALLSET_ENTRY (skill, sw_right_button) {
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ENABLED)) {
		return;
	}

	switch (current_rollover) {
		case SW_LEFT_ROLLOVER:
			current_rollover = SW_MIDDLE_ROLLOVER;
		break;

		case SW_MIDDLE_ROLLOVER:
			current_rollover = SW_RIGHT_ROLLOVER;
		break;

		case SW_RIGHT_ROLLOVER:
			current_rollover = SW_LEFT_ROLLOVER;
		break;
	}
	callset_invoke( lamp_update );
}

CALLSET_ENTRY (skill, any_pf_switch) {
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ENABLED)) {
		return;
	}

	counted_switches ++;

	// FIXME should be < 2 == bail, where's the other switch press coming from?  the other edge of the sw_right_loop switch?
	if (counted_switches < 3) {
		return;
	}

	// disable skillshot after the second pf switch is activated when skillshot is enabled.
	// can't disable it after the first one as the first one is normally always the sw_right_loop
	// switch that the ball travels over on it's way to the rollovers above the jets.
	//
	// the skillshot can never be made without passing over either the left or right loop
	// switches or through the spinner (without spinning it much, and also missing the jets)
	//
	// so if it ball launches, misses the sw_right_loop the player can quickly shoot the left
	// or right loop to flashing roll-over shot to get the skill shot - any other 2 switches
	// will disable the skillshot.

	skillshot_disable();
}

CALLSET_ENTRY (skill, serve_ball) {
	// TODO let the user select if they want to enable the rollover skillshot or the skid pad ramp super skillshot.
	skillshot_enable();
}

