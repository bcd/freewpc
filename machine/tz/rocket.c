/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

void rocket_deff (void)
{
	/* Stop deff from restarting whilst we
	 * are showing the level up deff */
	dmd_alloc_low_clean ();
	frame_draw (IMG_ROCKET1);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET2);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET3);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET4);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET5);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET6);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET7);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET8);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET9);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET10);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET11);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET12);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET13);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	frame_draw (IMG_ROCKET14);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	//task_sleep_sec (2);
	deff_exit ();
}


CALLSET_ENTRY (rocket, dev_rocket_enter)
{
	disable_skill_shot ();
	deff_start (DEFF_ROCKET);
	score (SC_10K);
}

static void rocket_kick_sound (void)
{
	sound_send (SND_ROCKET_KICK_DONE);
	flasher_pulse (FLASH_UR_FLIPPER);
	task_exit ();
}

CALLSET_ENTRY (rocket, dev_rocket_kick_attempt)
{
	event_should_follow (rocket, hitchhiker, TIME_2S);
	if (in_live_game)
	{
		leff_start (LEFF_NO_GI);
		sound_send (SND_ROCKET_KICK_REVVING);
		task_sleep (TIME_500MS);
		task_create_gid (0, rocket_kick_sound);
		leff_stop (LEFF_NO_GI);
	}
}

