/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (rocket, __machine2__) */
#include <freewpc.h>

extern bool skill_shot_enabled;

CALLSET_ENTRY (rocket, ball_search)
{
	if (switch_poll_logical (SW_ROCKET_KICKER))
	{
		sol_request (SOL_ROCKET_KICKER);
	}
}

/* Rocket animation contributed by highrise */
void rocket_deff (void)
{
	/* Show loading frames and wait for kick */
	U16 fno;
	for (fno = IMG_ROCKET_LOAD_START; fno <= IMG_ROCKET_LOAD_END; fno += 2)
	{
		dmd_map_overlay ();
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		callset_invoke (score_overlay);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
	task_sleep (TIME_100MS);
	/* Rocket takes 500ms before kick 
	 * load animation takes 400ms */
	/* Launch rocket */
	for (fno = IMG_NEWROCKET_START; fno <= IMG_NEWROCKET_END; fno += 2)
	{
		dmd_alloc_pair_clean ();
		frame_draw (fno);
		callset_invoke (score_overlay);
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
	deff_exit ();
}

static void rocket_kick_sound (void)
{
	sound_send (SND_ROCKET_KICK_DONE);
	flasher_pulse (FLASH_UR_FLIPPER);
	task_exit ();
}


/* Give the player 1M if he hits the right flipper on
 * the rocket launch, within 33ms before or 66ms after */
CALLSET_ENTRY (rocket, sw_right_button)
{
	if (task_kill_gid (GID_ROCKET_FLIPPER) && !task_find_gid (GID_ROCKET_FLIPPER_DEBOUNCE))
	{
		sound_send (SND_CUCKOO);
		score (SC_1M);
		timer_restart_free (GID_ROCKET_FLIPPER_DEBOUNCE, TIME_1S);
	}
}


CALLSET_ENTRY (rocket, dev_rocket_enter)
{
		if (skill_shot_enabled)
		{
			callset_invoke (skill_missed);
		}
}

CALLSET_ENTRY (rocket, dev_rocket_kick_attempt)
{
	if (in_live_game)
	{
		/* Small sleep to make sure skill code gets a chance to
		 * execute */
		task_sleep (TIME_33MS);
		if (!multi_ball_play ())
			leff_start (LEFF_ROCKET);
		sound_send (SND_ROCKET_KICK_REVVING);
		deff_start (DEFF_ROCKET);
		task_sleep (TIME_400MS + TIME_33MS);
		timer_restart_free (GID_ROCKET_FLIPPER, TIME_100MS);
		task_sleep (TIME_33MS);
		task_create_gid (0, rocket_kick_sound);
	}
}
