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

/* CALLSET_SECTION (pb_lock, __machine4__) */
#include <freewpc.h>
#include <search.h>

bool lock_powerball;

void lock_powerball_deff (void)
{
	sound_send (SND_TOO_HOT_TO_HANDLE);
	U16 fno;
	dmd_alloc_pair_clean ();
	timer_restart_free (GID_LOCK_POWERBALL, TIME_5S);
	while (task_find_gid (GID_LOCK_POWERBALL))
	{
		ball_search_timer_reset ();
		for (fno = IMG_POWERBALL_START; fno <= IMG_POWERBALL_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_center (&font_fireball, 64, 9, "LOCK BALL");
			font_render_string_left (&font_var5, 10, 24, "LEFT: NO");
			font_render_string_right (&font_var5, 118, 24, "RIGHT: YES");
			dmd_text_outline ();

			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_33MS);
		}
	}
	deff_exit ();
}

static void flipper_enable_task (void)
{
	task_sleep (TIME_200MS);
	flipper_enable ();
	task_exit ();
}

/* The ball should be held at this point */
CALLSET_ENTRY (pb_lock, pb_lock_choose)
{

	timer_restart_free (GID_PB_LOCK_DEBOUNCE, TIME_1S);
	sound_send (SND_PIANO_ENTRY_TUNE);
	flipper_disable ();
	deff_start_sync (DEFF_LOCK_POWERBALL);
	task_create_anon (flipper_enable_task);
}

CALLSET_ENTRY (pb_lock, sw_left_button)
{
	if (deff_get_active () == DEFF_LOCK_POWERBALL && !timer_find_gid (GID_PB_LOCK_DEBOUNCE))
	{
		sound_send (SND_BUYIN_CANCELLED);
		lock_powerball = FALSE;
		deff_stop (DEFF_LOCK_POWERBALL);
	}
}

CALLSET_ENTRY (pb_lock, sw_right_button)
{
	if (deff_get_active () == DEFF_LOCK_POWERBALL && !timer_find_gid (GID_PB_LOCK_DEBOUNCE))
	{
		lock_powerball = TRUE;
		deff_stop (DEFF_LOCK_POWERBALL);
	}
}

CALLSET_ENTRY (pb_lock, serve_ball)
{
	lock_powerball = TRUE;
}
