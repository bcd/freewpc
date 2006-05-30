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
#include <rtsol.h>

int jet_sound_index;

__local__ BCD jet_value;
__local__ int jets_scored;
__local__ int jets_for_bonus;

U8 rtsol_left_jet;
U8 rtsol_right_jet;
U8 rtsol_bottom_jet;


void jets_rtt (void)
{
	rt_solenoid_update (&rtsol_left_jet,
		SOL_LEFT_JET, RTSOL_ACTIVE_HIGH,
		SW_LEFT_JET, RTSW_ACTIVE_HIGH,
		8, 8);

	rt_solenoid_update (&rtsol_right_jet,
		SOL_RIGHT_JET, RTSOL_ACTIVE_HIGH,
		SW_RIGHT_JET, RTSW_ACTIVE_HIGH,
		8, 8);

	rt_solenoid_update (&rtsol_bottom_jet,
		SOL_LOWER_JET, RTSOL_ACTIVE_HIGH,
		SW_BOTTOM_JET, RTSW_ACTIVE_HIGH,
		8, 8);
}


CALLSET_ENTRY(jet, start_player)
{
	jets_scored = 0;
	jets_for_bonus = 25;
}


CALLSET_ENTRY(jet, start_ball)
{
	jet_value = MAKE_BCD(25);
}


sound_code_t jet_sounds[] = { SND_HORN1, SND_HORN2, SND_HORN3 };

void sw_jet_sound (void)
{
	jet_sound_index++;
	if (jet_sound_index >= 3)
		jet_sound_index = 0;

	sound_send (jet_sounds[jet_sound_index]);
	flasher_pulse (FLASH_JETS);
	task_sleep (TIME_100MS * 2);
	task_exit ();
}

void sw_jet_handler (void)
{
	score_add_current_const (SCORE_25K);
	task_create_gid1 (GID_JET_SOUND, sw_jet_sound);
}

DECLARE_SWITCH_DRIVER (sw_left_jet)
{
	.fn = sw_jet_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

DECLARE_SWITCH_DRIVER (sw_right_jet)
{
	.fn = sw_jet_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

DECLARE_SWITCH_DRIVER (sw_bottom_jet)
{
	.fn = sw_jet_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

