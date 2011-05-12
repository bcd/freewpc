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

//extern U8 greed_mode_timer;
score_t oddchange_score;
extern score_t greed_mode_total;
extern struct timed_mode_ops greed_mode;
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

const char *greed_text [] = { 
	"GREEEEEEEED",
	"GREEEEEEED",
	"GEEEEEEED",
	"GEEEEEED",
	"GEEEEED",
	"GREEED",
};

void oddchange_grows_deff (void)
{
	U16 fno;
	dmd_alloc_pair_clean ();
	U8 y = 10;
	U8 i = 0;
	for (fno = IMG_ODDCHANGE_END; fno > IMG_ODDCHANGE_START; fno -= 3)
	{
		y += 2;
		i++;
		dmd_map_overlay ();
		dmd_clean_page_low ();
		if (timed_mode_running_p (&greed_mode))
		{
			font_render_string_center (&font_mono5, 64, y, greed_text[i]);
		}
		else
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
	if (timed_mode_running_p (&greed_mode))
	{
		sound_send (SND_KACHING);
		font_render_string_center (&font_mono5, 64, y, "GREED");
		sprintf_score (greed_mode_total);
	}
	else
	{
		font_render_string_center (&font_mono5, 64, y, "ODDCHANGE GROWS");
		sprintf_score (oddchange_score);
	}
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	dmd_show_low ();
	if (timed_mode_running_p (&greed_mode))
	{
		dmd_copy_low_to_high ();
		dmd_invert_page (dmd_low_buffer);
		deff_swap_low_high (50, TIME_33MS);
		task_sleep_sec (1);
	}
	else
		task_sleep_sec (2);
	deff_exit ();

}

void reset_oddchange_score (void)
{
	score_zero (oddchange_score);
	score_add (oddchange_score, score_table[SC_50K]);
}

CALLSET_ENTRY (oddchange, oddchange_collected)
{
	if (in_live_game && single_ball_play ())
	{
		score_add (current_score, oddchange_score);
		deff_start_sync (DEFF_ODDCHANGE_COLLECTED);
		reset_oddchange_score ();
	}
}

CALLSET_ENTRY (oddchange, minute_elapsed)
{
	//TODO Change SC_5M after playtesting
	if (in_live_game && score_compare (oddchange_score, score_table[SC_5M]) == 1)
		sound_send (SND_THE_STAKES_ARE_HIGHER);
}

CALLSET_ENTRY (oddchange, start_ball)
{
	reset_oddchange_score ();
}

CALLSET_ENTRY (oddchange, grow_oddchange)
{
	if (score_compare (score_table[MAX_ODDCHANGE_SCORE], oddchange_score)  == 1)
	{
		U8 random_number;
		/* Award up to 5M during greed */
		if (timed_mode_running_p (&greed_mode))
			random_number = SC_4M;
		if (feature_config.oddchange_level > MAX_ODDCHANGE_VALUE)
			random_number = random_scaled(MAX_ODDCHANGE_VALUE);
		else
			random_number = random_scaled(feature_config.oddchange_level);
		/* Always remembering that random_scaled returns from 0 to N-1 */
		score_add (oddchange_score, score_table[random_number + 1]);
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
