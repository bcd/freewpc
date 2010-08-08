/*
 * Copyright 2006-2010 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* Uses the code from mpf.c to monitor the flipper buttons */
extern U8 mpf_buttons_pressed;

/* used to randomise the position of the doink text */
U8 tnf_x;
U8 tnf_y;

/* Total amount scored from doink mode */
score_t tnf_score;

void tnf_deff (void)
{
	U8 timer = 40; /* 4 seconds */
	tnf_x = 0;
	tnf_y = 0;
	while (--timer != 0)
	{
		dmd_alloc_low_clean ();
		if (timer % 2 != 0)
			font_render_string_center (&font_mono5, 64, 4, "HIT FLIPPER BUTTONS");
		psprintf ("%d DOINK", "%d DOINKS", mpf_buttons_pressed);
		font_render_string_center (&font_term6, 64 + tnf_x, 16 + tnf_y, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_100MS);
	}
	deff_exit ();
}

void tnf_exit_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf_score (tnf_score);
	font_render_string_center (&font_fixed6, 64, 8, sprintf_buffer);
	font_render_string_center (&font_var5, 64, 20, "POINTS EARNED FROM DOINKS");
	dmd_show_low ();
	task_sleep_sec (1);
	sound_send (SND_OOH_GIMME_SHELTER);
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (tnf, tnf_button_pushed)
{
	bounded_increment (mpf_buttons_pressed, 255);
	sound_send (SND_POWER_GRUNT_2);
	score_add (tnf_score, score_table[SC_250K]);
	tnf_x = random_scaled(10);
	tnf_y = random_scaled(8);
}

CALLSET_ENTRY (tnf, tnf_start)
{
	flipper_disable ();
	music_disable ();
	mpf_buttons_pressed = 1;
	score_zero (tnf_score);
	leff_start (LEFF_NO_GI);
}

CALLSET_ENTRY (tnf, tnf_end)
{
	score_add (current_score, tnf_score);
	flipper_enable ();
	music_enable ();
	music_refresh ();
	magnet_enable_catch_and_throw (MAG_LEFT);
	deff_start_sync (DEFF_TNF_EXIT);
}
