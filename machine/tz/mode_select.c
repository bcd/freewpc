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

/* CALLSET_SECTION (select_mode, __machine2__) */
#include <freewpc.h>
#include <search.h>


void select_mode_deff (void)
{
	U16 fno;
	dmd_alloc_pair_clean ();
	timer_restart_free (GID_SELECT_MODE, TIME_10S);
	while (task_find_gid (GID_SELECT_MODE))
	{
		ball_search_timer_reset ();
		for (fno = IMG_ARROW_START; fno <= IMG_ARROW_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_center (&font_fixed10, 64, 6, "CHOOSE PANEL");
			font_render_string_center (&font_mono5, 64, 17, "USE FLIPPER BUTTONS");
			font_render_string_left (&font_var5, 10, 24, "LEFT: COLLECT");
			font_render_string_right (&font_var5, 118, 24, "RIGHT: SELECT");
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
CALLSET_ENTRY (mode_select, select_mode)
{

	callset_invoke (stop_door_rotate);
	leff_start (LEFF_SELECT_MODE);
	timer_restart_free (GID_SELECT_MODE_DEBOUNCE, TIME_1S);
	//TODO Chaosmb twice Bug?
	door_advance_flashing ();
	sound_send (SND_PIANO_ENTRY_TUNE);
	flipper_disable ();
	deff_start_sync (DEFF_SELECT_MODE);
	leff_stop (LEFF_SELECT_MODE);
	leff_start (LEFF_FLASHER_HAPPY);
	task_create_anon (flipper_enable_task);
}

CALLSET_ENTRY (mode_select, sw_left_button)
{
	if (deff_get_active () == DEFF_SELECT_MODE && !timer_find_gid (GID_SELECT_MODE_DEBOUNCE))
		deff_stop (DEFF_SELECT_MODE);
}

CALLSET_ENTRY (mode_select, sw_right_button)
{
	if (deff_get_active () == DEFF_SELECT_MODE)
	{
		sound_send (SND_BUYIN_CANCELLED);
		door_advance_flashing ();
	}
}
