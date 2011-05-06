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

/* CALLSET_SECTION (oddchange , __machine2__) */
#include <freewpc.h>
#include <status.h>

#define MAX_ODDCHANGE_VALUE SC_500K
#define MAX_ODDCHANGE_SCORE SC_50M

extern struct timed_mode_ops greed_mode;
score_t oddchange_score;

void oddchange_collected_deff (void)
{
	sprintf_score (score_deff_get ());
	U16 fno;
	dmd_alloc_pair_clean ();
	sound_send (SND_ODD_CHANGE_BEGIN);
	U8 y = 30;
	for (fno = IMG_ODDCHANGE_START; fno < IMG_ODDCHANGE_END; fno += 2)
	{
		y--;
		dmd_map_overlay ();
		dmd_clean_page_low ();
		font_render_string_center (&font_var5, 64, y, "ODDCHANGE COLLECTED");
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_16MS);
	}
	dmd_clean_page_low ();
	dmd_clean_page_high ();
	dmd_alloc_pair ();

	sound_send (SND_KACHING);
	for (fno = IMG_FLASHCENTRE_START; fno < IMG_FLASHCENTRE_END; fno += 2)
	{
		dmd_map_overlay ();
		dmd_clean_page_low ();
		font_render_string_center (&font_var5, 64, y, "ODDCHANGE COLLECTED");
		sprintf_score (oddchange_score);
		font_render_string_center (&font_fixed6, 64, 12, sprintf_buffer);
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_16MS);
	}
	task_sleep_sec (2);
	deff_exit ();
}

void oddchange_grows_deff (void)
{
	U16 fno;
	dmd_alloc_pair_clean ();
	U8 y = 10;
	for (fno = IMG_ODDCHANGE_END; fno > IMG_ODDCHANGE_START; fno -= 4)
	{
		y++;
		y++;
		dmd_map_overlay ();
		dmd_clean_page_low ();
		font_render_string_center (&font_mono5, 64, y, "ODDCHANGE GROWS");
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_16MS);
	}
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	sprintf_score (oddchange_score);
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	font_render_string_center (&font_mono5, 64, y, "ODDCHANGE GROWS");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();

}

void reset_oddchange_score (void)
{
	score_zero (oddchange_score);
	score_add (oddchange_score, score_table[SC_50K]);
}

void oddchange_collected (void)
{
	if (in_live_game && single_ball_play ())
	{
		score_add (current_score, oddchange_score);
		deff_start (DEFF_ODDCHANGE_COLLECTED);
		reset_oddchange_score ();
	}
}

CALLSET_ENTRY (oddchange, start_ball)
{
	reset_oddchange_score ();
}

CALLSET_ENTRY (oddchange, sw_standup_1, sw_standup_2, sw_standup_3, sw_standup_4, sw_standup_5, sw_standup_6, sw_standup_7)
{
	if (in_live_game && !timed_mode_running_p (&greed_mode)
		&& (score_compare (score_table[MAX_ODDCHANGE_SCORE], oddchange_score)  == 1))
	{
		score_add (oddchange_score, score_table[random_scaled(MAX_ODDCHANGE_VALUE + 1)]);
		deff_restart (DEFF_ODDCHANGE_GROWS);
	}

}

CALLSET_ENTRY (oddchange, status_report)
{
	status_page_init ();
	font_render_string_center (&font_mono5, 64, 10, "ODDCHANGE POT");
	sprintf_score (oddchange_score);
	font_render_string_center (&font_mono5, 64, 18, sprintf_buffer);
	status_page_complete ();
}
