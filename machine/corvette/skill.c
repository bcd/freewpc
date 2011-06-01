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

void skillshot_rollover_disable( void ) {
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ROLLOVER_ENABLED)) {
		return;
	}

	global_flag_off ( GLOBAL_FLAG_SKILLSHOT_ROLLOVER_ENABLED );

	lamp_tristate_off (LM_LEFT_ROLLOVER);
	lamp_tristate_off (LM_MIDDLE_ROLLOVER);
	lamp_tristate_off (LM_RIGHT_ROLLOVER);

	// open the loop gate when the first pf switch is hit
	//task_sleep_sec(1);
	flag_on (FLAG_LOOP_GATE_OPENED);

	task_kill_gid (GID_SKILLSHOT_ROLLOVER_TIMER);
}

void skillshot_rollover_timer( void )
{
	task_sleep_sec (10);
	skillshot_rollover_disable();
	task_exit ();
}

void skillshot_rollover_enable( void ) {
	current_rollover = SW_MIDDLE_ROLLOVER;
	global_flag_on(GLOBAL_FLAG_SKILLSHOT_ROLLOVER_ENABLED);
	flag_off (FLAG_LOOP_GATE_OPENED);

	task_create_gid1 (GID_SKILLSHOT_ROLLOVER_TIMER, skillshot_rollover_timer);
}

void award_rollover_skillshot(U8 rollover_switch) {
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ROLLOVER_ENABLED)) {
		return;
	}

	if (rollover_switch == current_rollover) {
		score (SC_5M);
		// TODO display skillshot award.
	}

	skillshot_rollover_disable();
}

CALLSET_ENTRY (skill, lamp_update)
{
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ROLLOVER_ENABLED)) {
		return;
	}

	lamp_flash_if (LM_LEFT_ROLLOVER, current_rollover == SW_LEFT_ROLLOVER);
	lamp_flash_if (LM_MIDDLE_ROLLOVER, current_rollover == SW_MIDDLE_ROLLOVER);
	lamp_flash_if (LM_RIGHT_ROLLOVER, current_rollover == SW_RIGHT_ROLLOVER);
}

CALLSET_ENTRY (skill, sw_left_rollover)
{
	award_rollover_skillshot(SW_LEFT_ROLLOVER);
}

CALLSET_ENTRY (skill, sw_middle_rollover)
{
	award_rollover_skillshot(SW_MIDDLE_ROLLOVER);
}

CALLSET_ENTRY (skill, sw_right_rollover)
{
	award_rollover_skillshot(SW_RIGHT_ROLLOVER);
}

CALLSET_ENTRY (skill, sw_left_button) {
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ROLLOVER_ENABLED)) {
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
	if (!global_flag_test (GLOBAL_FLAG_SKILLSHOT_ROLLOVER_ENABLED)) {
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


CALLSET_ENTRY (skill, serve_ball) {
	// TODO skillshot menu - let the user select if they want to enable the rollover skillshot or the skid pad ramp super skillshot.
	skillshot_rollover_enable();
}

CALLSET_ENTRY (skill, shoot_again) {
	// the player clearly sucks, that, or the machine was being evil. :D
	skillshot_rollover_disable();
}

