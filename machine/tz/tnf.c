/*
 * Copyright 2006-2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (tnf, __machine3__) */

#include <freewpc.h>

extern U8 masher_buttons_pressed;
/* used to randomise the position of the doink text */
U8 tnf_x;
U8 tnf_y;
U8 tnf_timer;

/* Total amount scored from doink mode */
__local__ score_t tnf_score;

void tnf_deff (void)
{
	music_disable ();
	tnf_timer = 60;
	tnf_x = 0;
	tnf_y = 0;
	sound_send (SND_PIANO_ENTRY_TUNE);
	while (tnf_timer > 1)
	{
		U16 fno;
		dmd_alloc_pair_clean ();
		for (fno = IMG_PINWHEEL_END; fno >= IMG_PINWHEEL_START; fno -= 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			if (tnf_timer % 2 != 0)
				font_render_string_center (&font_mono5, 64, 4, "HIT FLIPPER BUTTONS");
			psprintf ("%d DOINK", "%d DOINKS", masher_buttons_pressed);
			font_render_string_center (&font_term6, 64 + tnf_x, 16 + tnf_y, sprintf_buffer);
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_16MS);
			bounded_decrement (tnf_timer, 0);
		}
	}
	deff_exit ();
}

void tnf_exit_deff (void)
{	
	dmd_alloc_pair_clean ();
	U16 fno;
	sound_send (SND_CLOCK_CHAOS_END_BOOM);
	for (fno = IMG_EXPLODE_START; fno <= IMG_EXPLODE_END; fno += 2)
	{
		dmd_map_overlay ();
		dmd_clean_page_low ();
		
		sprintf_score (tnf_score);
		font_render_string_center (&font_fixed6, 64, 8, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 20, "POINTS EARNED FROM DOINKS");
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
	if (score_compare (tnf_score, score_table[SC_20M]) == 1)
		sound_send (SND_OOH_GIMME_SHELTER);
	else
		sound_send (SND_RETURN_TO_YOUR_HOMES);
	task_sleep_sec (2);
	deff_exit ();
}

static void tnf_sound_task (void)
{
	if (masher_buttons_pressed < 80)
		sound_send (SND_BUYIN_CANCELLED);
	else
		sound_send (SND_CLOCK_CHAOS_END_BOOM);
	task_exit ();
}

CALLSET_ENTRY (tnf, sw_left_button, sw_right_button)
{
	if (deff_get_active () == DEFF_TNF)
	{
		bounded_increment (masher_buttons_pressed, 255);
		score_add (tnf_score, score_table[SC_250K]);
		tnf_x = random_scaled(10);
		tnf_y = random_scaled(8);
		task_recreate_gid (GID_TNF_SOUND, tnf_sound_task);
	}
}

CALLSET_ENTRY (tnf, init)
{
	tnf_timer = 0;
}

CALLSET_ENTRY (tnf, tnf_start)
{
	flipper_disable ();
	masher_buttons_pressed = 1;
	score_zero (tnf_score);
	leff_start (LEFF_BONUS);
	deff_start_sync (DEFF_TNF);
	task_sleep_sec (1);
	while (deff_get_active () == DEFF_TNF)
		task_sleep (TIME_500MS);
	leff_stop (LEFF_BONUS);
	callset_invoke (tnf_end);
}

CALLSET_ENTRY (tnf, tnf_end)
{
	music_enable ();
	deff_start_sync (DEFF_TNF_EXIT);
	score_add (current_score, tnf_score);
	flipper_enable ();
	music_refresh ();
	magnet_enable_catch_and_throw (MAG_LEFT);
}
