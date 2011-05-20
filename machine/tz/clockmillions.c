/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (clockmillions, __machine3__) */

#include <freewpc.h>

U8 clock_millions_mode_timer;
U8 clock_mode_hits;
score_t clock_mode_score;

extern U8 chaosmb_hits_to_relight;

void clock_millions_mode_init (void);
void clock_millions_mode_exit (void);

struct timed_mode_ops clock_millions_mode = {
	DEFAULT_MODE,
	.init = clock_millions_mode_init,
	.exit = clock_millions_mode_exit,
	.gid = GID_CLOCK_MILLIONS_MODE_RUNNING,
	.music = MUS_CLOCK_CHAOS1,
	.deff_running = DEFF_CLOCK_MILLIONS_MODE,
	.deff_ending = DEFF_CLOCK_MILLIONS_MODE_TOTAL,
	.prio = PRI_GAME_MODE1,
	.init_timer = 20,
	.timer = &clock_millions_mode_timer,
	.grace_timer = 3,
	.pause = system_timer_pause,
};

void clock_millions_mode_total_deff (void)
{
	sound_send (SND_CLOCK_GONG);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 5, "CLOCK MILLIONS");
	sprintf_score (clock_mode_score);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	font_render_string_center (&font_var5, 64, 27, "POINTS EARNED FROM MODE");
	dmd_show_low ();
	task_sleep_sec (4);
	deff_exit ();
}


void clock_millions_explode_deff (void)
{	
	dmd_alloc_pair_clean ();
	U16 fno;
	sound_send (SND_GREED_MODE_BOOM);
	for (fno = IMG_EXPLODE_START; fno <= IMG_EXPLODE_END; fno += 2)
	{
		dmd_map_overlay ();
		dmd_clean_page_low ();
		font_render_string_center (&font_fixed6, 64, 10, "CLOCK DESTROYED");
		font_render_string_center (&font_mono5, 64, 21, "20 MILLION");
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
	dmd_alloc_pair_clean ();
	font_render_string_center (&font_fixed6, 64, 10, "CLOCK DESTROYED");
	font_render_string_center (&font_mono5, 64, 21, "20 MILLION");
	dmd_copy_low_to_high ();
	dmd_show2 ();
	task_sleep_sec (2);
	deff_exit ();
}

void clock_millions_hit_deff (void)
{	
	U16 fno;
	U16 img_start = 0;
	U16 img_end = 0;
	dmd_alloc_pair_clean ();
	switch (random_scaled (3))
	{
		case 0:
			img_start = IMG_FLASH_START;
			img_end = IMG_FLASH_END;
			break;
		case 1:
			img_start = IMG_FLASHCENTRE_START;
			img_end = IMG_FLASHCENTRE_END;
			break;
		case 2:
			img_start = IMG_FLASHLEFT_START;
			img_end = IMG_FLASHLEFT_END;
			break;
	}

	for (fno = img_start; fno < img_end; fno += 2)
	{
		U8 x = random_scaled (4);
		U8 y = random_scaled (4);
		dmd_map_overlay ();
		dmd_clean_page_low ();

		psprintf ("CLOCK HIT %d TIME", "CLOCK HIT %d TIMES", clock_mode_hits);
		font_render_string_center (&font_fixed6, 64 + x, 10 + y, sprintf_buffer);
		sprintf_score (clock_mode_score);
		font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
	/* Redraw it so the 'HITS' text is centred */
	dmd_clean_page_low ();
	dmd_clean_page_high ();
	dmd_alloc_low ();
	psprintf ("CLOCK HIT %d TIME", "CLOCK HIT %d TIMES", clock_mode_hits);
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	sprintf_score (clock_mode_score);
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	dmd_show_low ();	
	task_sleep_sec (2);
	deff_exit ();
}

void clock_millions_mode_deff (void)
{
	U16 fno;
	dmd_alloc_pair_clean ();
	for (;;)
	{
		for (fno = IMG_CLOCK_START; fno <= IMG_CLOCK_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
	
			font_render_string_center (&font_var5, 64, 5, "CLOCK MILLIONS");
			sprintf_current_score ();
			font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
			psprintf ("SHOOT CLOCK 1 MORE TIME", "SHOOT CLOCK %d MORE TIMES", 6 - clock_mode_hits);
			font_render_string_center (&font_var5, 64, 27, sprintf_buffer);
			sprintf ("%d", clock_millions_mode_timer);
			font_render_string (&font_var5, 2, 2, sprintf_buffer);
			font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
}

CALLSET_ENTRY (clock_millions, sw_clock_target)
{
	
	if (timed_mode_running_p (&clock_millions_mode))
	{
		leff_start (LEFF_CLOCK_TARGET);
		/* Award bonus if hit 6 times */
		if (++clock_mode_hits > 5)
		{
			sound_send (SND_EXPLOSION_3);
			score (SC_20M);
			score_add (clock_mode_score, score_table[SC_20M]);
			deff_start (DEFF_CLOCK_MILLIONS_EXPLODE);
			timed_mode_end (&clock_millions_mode);
		}
		else
		{
			sound_send (SND_CLOCK_BELL);
			score (SC_5M);
			score_add (clock_mode_score, score_table[SC_5M]);	
			deff_start (DEFF_CLOCK_MILLIONS_HIT);
		}
		if (!global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING))
			tz_clock_reverse_direction ();
		tz_clock_set_speed (clock_mode_hits);
	}
	else if (!global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING))
	{
		callset_invoke (sw_jet_noflash);
		score (SC_50K);
		sound_send (SND_NO_CREDITS);
	}
}

void clock_millions_mode_init (void)
{
	clock_mode_hits = 0;
	score_zero (clock_mode_score);
	lamp_tristate_flash (LM_CLOCK_MILLIONS);
	tz_clock_start_forward ();
}

void clock_millions_mode_expire (void)
{
	if (clock_mode_hits <= 2)
		callset_invoke (start_hurryup);
}

void clock_millions_mode_exit (void)
{
	lamp_tristate_off (LM_CLOCK_MILLIONS);
	tz_clock_reset ();
}

CALLSET_ENTRY (clock_millions, end_ball)
{
	timed_mode_end (&clock_millions_mode);
}

CALLSET_ENTRY (clock_millions, display_update)
{
	timed_mode_display_update (&clock_millions_mode);
}

CALLSET_ENTRY (clock_millions, music_refresh)
{
	if (!timed_mode_running_p (&clock_millions_mode))
		return;
	switch (clock_mode_hits)
	{
		case 0:
			music_request (MUS_CLOCK_CHAOS1, PRI_GAME_MODE1);
			break;
		case 1:
			music_request (MUS_CLOCK_CHAOS2, PRI_GAME_MODE1);
			break;
		case 2:
			music_request (MUS_CLOCK_CHAOS3, PRI_GAME_MODE1);
			break;
		case 3:
			music_request (MUS_CLOCK_CHAOS4, PRI_GAME_MODE1);
			break;
		case 4:
			music_request (MUS_CLOCK_CHAOS5, PRI_GAME_MODE1);
			break;
		default:
			music_request (MUS_CLOCK_CHAOS5, PRI_GAME_MODE1);
			break;
	}
}

CALLSET_ENTRY (clock_millions, door_start_clock_millions)
{
	timed_mode_begin (&clock_millions_mode);
}

