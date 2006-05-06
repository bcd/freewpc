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

int jet_sound_index;

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
	score_add_current_const (0x1230ULL);
	task_create_gid1 (GID_JET_SOUND, sw_jet_sound);

	extern void door_advance_flashing (void);
	door_advance_flashing ();
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

