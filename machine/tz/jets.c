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
#include <rtsol.h>

U8 jet_sound_index;

__local__ U8 jets_scored;
__local__ U8 jets_for_bonus;

__fastram__ S8 rtsol_left_jet;
__fastram__ S8 rtsol_right_jet;
__fastram__ S8 rtsol_bottom_jet;


void jets_rtt (void)
{
	if (in_live_game)
	{
		rt_solenoid_update (&rtsol_left_jet,
			SOL_LEFT_JET, SW_LEFT_JET, 8, 8);
	
		rt_solenoid_update (&rtsol_right_jet,
			SOL_RIGHT_JET, SW_RIGHT_JET, 8, 8);
	
		rt_solenoid_update (&rtsol_bottom_jet,
			SOL_LOWER_JET, SW_BOTTOM_JET, 8, 8);
	}
}


CALLSET_ENTRY(jet, start_player)
{
	jets_scored = 0;
	jets_for_bonus = 25;
	lamp_tristate_on (LM_LEFT_JET);
	lamp_tristate_on (LM_LOWER_JET);
	lamp_tristate_on (LM_RIGHT_JET);
}

sound_code_t jet_sounds[] = { 
	SND_HORN1, SND_HORN2, SND_HORN3
};

sound_code_t super_jet_sounds[] = { 
	SND_TSM_HIT_1, SND_TSM_HIT_2, SND_TSM_HIT_3
};

void sw_jet_sound (void)
{
	jet_sound_index++;
	if (jet_sound_index >= 3)
		jet_sound_index = 0;

	if (flag_test (FLAG_TSM_RUNNING))
		sound_send (super_jet_sounds[jet_sound_index]);
	else
		sound_send (jet_sounds[jet_sound_index]);
	flasher_pulse (FLASH_JETS);
	task_sleep (TIME_200MS);
	task_exit ();
}


CALLSET_ENTRY (jet, sw_jet)
{
	if (flag_test (FLAG_TSM_RUNNING))
		score (SC_500K);
	else
		score (SC_250K);
	task_create_gid1 (GID_JET_SOUND, sw_jet_sound);
}


CALLSET_ENTRY (jet, lamp_update)
{
	/* if (flag_test (FLAG_TSM_RUNNING))
		leff_start (LEFF_JETS_ACTIVE);
	else
		leff_stop (LEFF_JETS_ACTIVE); */
}


CALLSET_ENTRY (jet, start_ball)
{
	flag_off (FLAG_TSM_RUNNING);
	leff_stop (LEFF_JETS_ACTIVE);
}


CALLSET_ENTRY (jet, door_start_tsm)
{
	flag_on (FLAG_TSM_RUNNING);
	leff_start (LEFF_JETS_ACTIVE);
}

