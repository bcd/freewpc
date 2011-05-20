/*
 * Copyright 2010 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (deffs2, __machine3__) */
#include <freewpc.h>

extern struct timed_mode_ops hitch_mode;
extern U8 mute_and_pause_timeout;
extern bool amode_show_scores_long;

#if 0
void match_loop_deff (void)
{
	U16 fno;
	dmd_alloc_pair_clean ();
	for (;;)
	{
		for (fno = IMG_MATCH_START; fno <= IMG_MATCH_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_right (&font_fixed6, 126, 1, "MATCH");
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
#endif
void shoot_again_deff (void)
{	
	sound_send (SND_EXTRA_BALL_BALLOON);
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 10, "SHOOT AGAIN");
	sprintf ("PLAYER %d", player_up);
	font_render_string_center (&font_fixed6, 64, 22, sprintf_buffer);
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (30, TIME_100MS);
	deff_exit ();
}

static inline void flash_and_exit_deff2 (U8 flash_count, task_ticks_t flash_delay)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (flash_count, flash_delay);
	deff_exit ();
}

void shoot_camera_deff (void)
{
	if (timed_mode_running_p (&hitch_mode))
			sprintf ("SHOOT HITCH");
		else
			sprintf ("SHOOT CAMERA");

	if (event_did_follow (loop, ball_grab))
	{
		dmd_alloc_pair_clean ();
		U16 fno;
		for (fno = IMG_LOOP_START; fno < IMG_LOOP_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
		/* Get rid of the last dirty frame */
		dmd_alloc_pair_clean ();
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		dmd_copy_low_to_high ();
		dmd_show2 ();
		flash_and_exit_deff2 (10, TIME_66MS);
	}
	else
	{
		flash_and_exit_deff2 (15, TIME_66MS);
	}
}

void shoot_power_payoff_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 9, "SHOOT");
	font_render_string_center (&font_fixed10, 64, 24, "POWER PAYOFF");
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (20, TIME_100MS);
	deff_exit ();
}

CALLSET_ENTRY (deffs2, amode_page)
{
	if (amode_show_scores_long)
		return;
	extern U8 hour;
	dmd_map_overlay ();
	dmd_clean_page_high ();
	dmd_clean_page_low ();
	if (hour < 12)
		sprintf ("GOOD MORNING");
	else if (hour < 18)
		sprintf ("GOOD AFTERNOON");
	else if (hour < 21)
		sprintf ("GOOD EVENING");
	else
		sprintf ("THE TWILIGHT HOUR");
	font_render_string_center (&font_var5, 64, 16, sprintf_buffer);
	show_text_on_stars ();
}
