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
extern struct timed_mode_ops greed_mode;
score_t oddchange_score;

void oddchange_collected_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_sched_transition (&trans_scroll_up);
	font_render_string_center (&font_var5, 64, 8, "ODDCHANGE COLLECTED");
	sprintf_score (oddchange_score);
	font_render_string_center (&font_fixed6, 64, 18, sprintf_buffer);
	dmd_show_low ();
	sound_send (SND_KACHING);
	task_sleep_sec (2);
	deff_exit ();
}

void oddchange_pot_deff (void)
{
	U16 fno;
	U16 img_start;
	U16 img_end;
	switch (random_scaled (3))
	{
		case 0:
			img_start = IMG_FLASH_START;
			img_end = IMG_FLASH_END;
			break;
		default:
		case 1:
			img_start = IMG_FLASHCENTRE_START;
			img_end = IMG_FLASHCENTRE_END;
			break;
		case 2:
			img_start = IMG_FLASHLEFT_START;
			img_end = IMG_FLASHLEFT_END;
			break;
	}

	dmd_alloc_pair_clean ();
	for (fno = img_start; fno < img_end; fno += 2)
	{
		U8 x = random_scaled (4);
		U8 y = random_scaled (4);
		dmd_map_overlay ();
		dmd_clean_page_low ();
		sprintf_score (oddchange_score);
		font_render_string_center (&font_fixed6, 62 + x, 7 + y, sprintf_buffer);
		font_render_string_center (&font_mono5, 64, 20, "ODDCHANGE POT");
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	sprintf_score (oddchange_score);
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_500MS);
	deff_exit ();

}

CALLSET_ENTRY (oddchange, oddchange_collected)
{
	score_add (current_score, oddchange_score);
	deff_start_sync (DEFF_ODDCHANGE_COLLECTED);
	callset_invoke (reset_oddchange_pot);
}

CALLSET_ENTRY (oddchange, start_ball, reset_oddchange_pot)
{
	score_zero (oddchange_score);
	score_add (oddchange_score, score_table[SC_50K]);
}

CALLSET_ENTRY (oddchange, sw_standup_1, sw_standup_2, sw_standup_3, sw_standup_4, sw_standup_5, sw_standup_6, sw_standup_7)
{
	if (!in_live_game)
		return;
	if (score_compare (score_table[SC_10M], oddchange_score)  == 1
			&& !timed_mode_running_p (&greed_mode)
			&& single_ball_play ())
	{
		score_add (oddchange_score, score_table[SC_150K]);
		deff_start (DEFF_ODDCHANGE_POT);
	}

}
