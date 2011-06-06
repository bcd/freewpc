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

/* CALLSET_SECTION (tnf, __machine4__) */

#include <freewpc.h>

U8 tnf_buttons_pressed;
/* How many times the buttons must be pushed to win */
U8 tnf_target;

__local__ U8 tnf_level;

/* used to randomise the position of the doink text */
U8 tnf_x;
U8 tnf_y;

struct progress_bar_ops tnf_progress_bar = {
	.x = 8,
	.y = 26,
	.fill_level = &tnf_buttons_pressed,
	.max_level = &tnf_target,
	.bar_width = 106,
};


/* Total amount scored from doink mode */
__local__ score_t tnf_score;

void tnf_deff (void)
{
	bool blink_on = TRUE;
	tnf_x = 0;
	tnf_y = 0;
	timer_restart_free (GID_TNF_TIMER, TIME_4S);
	while (tnf_buttons_pressed < 100 && task_find_gid (GID_TNF_TIMER))
	{
		dmd_alloc_pair_clean ();
		if (blink_on)
		{
			font_render_string_center (&font_mono5, 64, 4, "HIT FLIPPER BUTTONS");
			blink_on = FALSE;
		}
		else
			blink_on = TRUE;
		psprintf ("%d DOINK", "%d DOINKS", tnf_buttons_pressed);
		font_render_string_center (&font_term6, 60 + tnf_x, 12 + tnf_y, sprintf_buffer);
		dmd_copy_low_to_high ();
		callset_invoke (score_overlay);
		draw_progress_bar (&tnf_progress_bar);
		dmd_show2 ();
		task_sleep (TIME_66MS);
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
	if (tnf_buttons_pressed >= tnf_target)
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
	if (tnf_buttons_pressed < tnf_target - 10)
		sound_send (SND_BUYIN_CANCELLED);
	else
		sound_send (SND_CLOCK_CHAOS_END_BOOM);
	task_exit ();
}

CALLSET_ENTRY (tnf, sw_left_button, sw_right_button)
{
	if (deff_get_active () == DEFF_TNF)
	{
		bounded_increment (tnf_buttons_pressed, 255);
		score_add (tnf_score, score_table[SC_100K + tnf_level]);
		tnf_x = random_scaled(10);
		tnf_y = random_scaled(3);
		task_recreate_gid (GID_TNF_SOUND, tnf_sound_task);
		if (tnf_buttons_pressed > tnf_target)
		{
			score_add (tnf_score, score_table[SC_5M]);
			task_kill_gid (GID_TNF_TIMER);
		}
	}
}

static void set_tnf_target (void)
{
	tnf_target = 20 + (tnf_level * 20);
}


static void tnf_increase_level (void)
{
	bounded_increment (tnf_level, 4);
}

CALLSET_ENTRY (tnf, tnf_start)
{
	flipper_disable ();
	tnf_buttons_pressed = 1;
	score_zero (tnf_score);
	set_tnf_target ();
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
	if (tnf_buttons_pressed >= tnf_target)
		tnf_increase_level ();

	flipper_enable ();
	music_refresh ();
	magnet_enable_catch_and_throw (MAG_LEFT);
}

CALLSET_ENTRY (tnf, start_player)
{
	tnf_level = 1;
}

