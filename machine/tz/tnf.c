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

/* Total amount scored from doink mode */
__local__ score_t tnf_score;

const U8 progress_bar_slice_bitmap[] = {
	1,5,1,1,1,1,1,
};

const U8 progress_bar_chunk_bitmap[] = {
	5,5,15,15,15,15,15,
};

static void tnf_draw_progress_bar (void)
{
	U8 i;
	/* Where the next slice needs to be put */
	/* 128 - Size of bar (80 pixels) / 2 */
	U8 x = 24;

	/* Draw the finish line */
	bitmap_blit (progress_bar_slice_bitmap, 104, 20);
	for (i = masher_buttons_pressed; i > 0; i--)
	{
		if (x > 128)
			break;
		else if (i < 5)
			bitmap_blit (progress_bar_slice_bitmap, x, 20);
		else
		{
			/* Draw the 5x5 chunks */
			bitmap_blit (progress_bar_chunk_bitmap, x, 20);
			x += 4;
			i -= 4;
		}
		x++;
	}
}

void tnf_deff (void)
{
	bool blink_on = TRUE;
	tnf_x = 0;
	tnf_y = 0;
	timer_restart_free (GID_TNF_TIMER, TIME_4S);
	while (masher_buttons_pressed < 100 && task_find_gid (GID_TNF_TIMER))
	{
			dmd_alloc_low_clean ();
			if (blink_on)
			{
				font_render_string_center (&font_mono5, 64, 4, "HIT FLIPPER BUTTONS");
				blink_on = FALSE;
			}
			else
				blink_on = TRUE;
			tnf_draw_progress_bar ();
			psprintf ("%d DOINK", "%d DOINKS", masher_buttons_pressed);
			font_render_string_center (&font_term6, 60 + tnf_x, 12 + tnf_y, sprintf_buffer);
			dmd_show_low ();
			task_sleep (TIME_33MS);
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
	dmd_alloc_pair_clean ();
	sprintf_score (tnf_score);
	font_render_string_center (&font_fixed6, 64, 8, sprintf_buffer);
	font_render_string_center (&font_var5, 64, 20, "POINTS EARNED FROM DOINKS");
	dmd_copy_low_to_high ();
	dmd_show2 ();
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
		tnf_y = random_scaled(3);
		task_recreate_gid (GID_TNF_SOUND, tnf_sound_task);
	}
}

CALLSET_ENTRY (tnf, tnf_start)
{
	flipper_disable ();
	masher_buttons_pressed = 1;
	score_zero (tnf_score);
	leff_start (LEFF_BONUS);
	music_request (MUS_POWERFIELD, PRI_GAME_VMODE);
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
