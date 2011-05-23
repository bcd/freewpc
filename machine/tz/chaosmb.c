/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>

U8 chaosmb_level;
U8 chaosmb_level_stored;
U8 chaosmb_hits_to_relight;
extern U8 autofire_request_count;
extern U8 unlit_shot_count;
extern bool mball_jackpot_uncollected;

extern inline void score_deff_begin (const font_t *font, U8 x, U8 y, const char *text)
{
	score_update_start ();
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (font, x, y, text);
	sprintf_current_score ();
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_copy_low_to_high ();
}


extern inline void score_deff_end (task_ticks_t flash_rate)
{
	dmd_show_low ();
	while (!score_update_required ())
	{
		task_sleep (flash_rate);
		dmd_show_other ();
	}
}

struct {
	const char *shot_name;
	U8 jackpot_value;
	U8 lamp_num;
} chaosmb_shots[] = {
	{ "LEFT RAMP", 15, },
	{ "RIGHT RAMP", 20, },
	{ "PIANO", 25,  },
	{ "CAMERA", 30 },
	{ "POWER PAYOFF", 40 },
	{ "DEAD END", 50 },
};

/* Allow divert if chaosmb is running and
 * if the left ramp is the currently lit jackpot */

bool chaosmb_can_divert_to_autoplunger (void)
{
	if (global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING)
		&& chaosmb_level == 0
		&& chaosmb_hits_to_relight == 0)
		return TRUE;
	else
		return FALSE;
}

void chaos_jackpot_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 9, "CHAOS");
	font_render_string_center (&font_fixed10, 64, 23, "JACKPOT");
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (15, TIME_100MS);
	
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	sprintf ("%d MILLION", chaosmb_shots[chaosmb_level_stored].jackpot_value);
	font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
	sound_send (SND_EXPLOSION_1);
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (15, TIME_100MS);
	deff_exit ();
}

void chaosmb_running_deff (void)
{
	U16 fno;
	dmd_alloc_pair_clean ();
	for (;;)
	{
		for (fno = IMG_CLOCK_START; fno <= IMG_CLOCK_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_center (&font_fixed6, 64, 4, "CHAOS MULTIBALL");
			sprintf_current_score ();
			font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
			if (chaosmb_hits_to_relight == 0)
			{
				sprintf ("SHOOT %s", chaosmb_shots[chaosmb_level].shot_name);
				font_render_string_center (&font_var5, 64, 27, sprintf_buffer);
			}
			else if (chaosmb_hits_to_relight == 1)
			{
				font_render_string_center (&font_var5, 64, 27,
					"HIT CLOCK TO LIGHT JACKPOT");
				lamp_tristate_flash (LM_CLOCK_MILLIONS);
			}
			else
			{
				sprintf ("HIT CLOCK %d MORE TIMES", chaosmb_hits_to_relight);
				font_render_string_center (&font_var5, 64, 27, sprintf_buffer);
				lamp_tristate_flash (LM_CLOCK_MILLIONS);
			}
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}

}

static void chaosmb_check_jackpot_lamps (void)
{
	if (chaosmb_hits_to_relight == 0)
	{	
		lamp_tristate_off (LM_CLOCK_MILLIONS);
		
		switch (chaosmb_level)
		{
		/* TODO This is very hacky, do it properly */
			case 0:
				lamp_tristate_off (LM_DEAD_END);
				lamp_tristate_flash (LM_MULTIBALL);
				break;
			case 1:
				lamp_tristate_off (LM_MULTIBALL);
				lamp_tristate_flash (LM_RAMP_BATTLE);
				break;
			case 2:
				lamp_tristate_off (LM_RAMP_BATTLE);
				lamp_tristate_flash (LM_PIANO_JACKPOT);
				break;
			case 3:
				lamp_tristate_off (LM_PIANO_JACKPOT);
				lamp_tristate_flash (LM_CAMERA);
				break;
			case 4:
				lamp_tristate_off (LM_CAMERA);
				lamp_tristate_flash (LM_POWER_PAYOFF);
				break;
			case 5:
				lamp_tristate_off (LM_POWER_PAYOFF);
				lamp_tristate_flash (LM_DEAD_END);
				break;
		}
	}
	else
	{
		/* Turn off jackpot lamps */
		lamplist_apply (LAMPLIST_CHAOSMB_JACKPOTS, lamp_flash_off);
		lamplist_apply (LAMPLIST_CHAOSMB_JACKPOTS, lamp_off);
		lamp_tristate_flash (LM_CLOCK_MILLIONS);
	}
}

static void chaosmb_score_jackpot (void)
{
	
	magnet_disable_catch (MAG_LEFT);
	mball_jackpot_uncollected = FALSE;
	/* Score it */
	score_multiple	(SC_1M, chaosmb_shots[chaosmb_level].jackpot_value);
	/* Store level for deff */
	chaosmb_level_stored = chaosmb_level;
	/* Increment and wrap around at 5 */
	if (chaosmb_level < 5)
	{
		chaosmb_level++;
		chaosmb_hits_to_relight = chaosmb_level * 2;
	}
	else
	{
		chaosmb_level = 0;
		chaosmb_hits_to_relight = 4;
	}

	chaosmb_check_jackpot_lamps ();
	deff_start (DEFF_JACKPOT);
	leff_start (LEFF_PIANO_JACKPOT_COLLECTED);
	leff_start (LEFF_FLASH_GI2);
	deff_start (DEFF_CHAOS_JACKPOT);
	tz_clock_start_forward ();
}

static void start_clock_task (void)
{
	task_sleep_sec (2);
	tz_clock_start_forward ();
	task_exit ();
}

CALLSET_ENTRY (chaosmb, chaosmb_start)
{
	if (!global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING))
	{
		magnet_reset ();
		callset_invoke (mball_restart_stop);
		unlit_shot_count = 0;
		global_flag_on (GLOBAL_FLAG_CHAOSMB_RUNNING);
		chaosmb_level = 0;
		chaosmb_hits_to_relight = 1;
		callset_invoke (mball_start_3_ball);
		mball_jackpot_uncollected = TRUE;
		//ballsave_add_time (10);
		/* Check and light jackpot lamp */
		chaosmb_check_jackpot_lamps ();
		/* TODO vary speed based on jackpot? */
		task_create_anon (start_clock_task);
	}
}

CALLSET_ENTRY (chaosmb, chaosmb_stop)
{
	if (mball_jackpot_uncollected == TRUE)
	{
		sound_send (SND_NOOOOOOOO);
		callset_invoke (start_hurryup);
	}
	
	global_flag_off (GLOBAL_FLAG_CHAOSMB_RUNNING);
	/* Turn off jackpot lamps */
	lamp_tristate_off (LM_CLOCK_MILLIONS);
	lamplist_apply (LAMPLIST_CHAOSMB_JACKPOTS, lamp_flash_off);
	lamplist_apply (LAMPLIST_CHAOSMB_JACKPOTS, lamp_off);
	deff_stop (DEFF_CHAOSMB_RUNNING);
	music_refresh ();
	tz_clock_reset ();
}

static inline void chaosmb_check_level (U8 level)
{
	if (global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING)
		&& (chaosmb_level == level)
		&& (chaosmb_hits_to_relight == 0))
	{
		chaosmb_score_jackpot ();
	}
}

CALLSET_ENTRY (chaosmb, display_update)
{
	if (global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING))
		deff_start_bg (DEFF_CHAOSMB_RUNNING, PRI_GAME_MODE6);
}

CALLSET_ENTRY (chaosmb, lamp_update)
{
	/* Jackpot lamp lighting is done by
	 * chaosmb_check_jackpot_lamps () */
	if (global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING))
		chaosmb_check_jackpot_lamps ();
	
}

CALLSET_ENTRY (chaosmb, music_refresh)
{
	if (global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING))
		music_request (MUS_SPIRAL_MODE, PRI_GAME_MODE6);
}


CALLSET_ENTRY (chaosmb, door_start_clock_chaos)
{
	callset_invoke (chaosmb_start);
}


/* Called from leftramp.c */
void chaosmb_left_ramp_exit (void)
{
	chaosmb_check_level (0);
}

CALLSET_ENTRY (chaosmb, sw_right_ramp)
{
	chaosmb_check_level (1);
}

CALLSET_ENTRY (chaosmb, sw_piano)
{
	chaosmb_check_level (2);
}

CALLSET_ENTRY (chaosmb, sw_camera)
{
	chaosmb_check_level (3);
}

CALLSET_ENTRY (chaosmb, sw_power_payoff)
{
	chaosmb_check_level (4);
}

CALLSET_ENTRY (chaosmb, sw_dead_end)
{
	chaosmb_check_level (5);
}

CALLSET_ENTRY (chaosmb, sw_clock_target)
{
	if (global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING))
	{
		leff_start (LEFF_CLOCK_TARGET);
		if (chaosmb_hits_to_relight == 0 && !timer_find_gid (GID_STOP_IT_DEBOUNCE))
		{	
			tz_clock_stop ();
			sound_send (SND_STOP_IT);
			timer_restart_free (GID_STOP_IT_DEBOUNCE, TIME_5S);
		}
		else if (chaosmb_hits_to_relight != 0)
		{
			sound_send (SND_CLOCK_BELL);
			score (SC_250K);
			tz_clock_reverse_direction ();
		}
		bounded_decrement (chaosmb_hits_to_relight, 0);
	}
}

CALLSET_ENTRY (chaosmb, single_ball_play)
{
	callset_invoke (chaosmb_stop);
}

CALLSET_ENTRY (chaosmb, end_ball)
{
	callset_invoke (chaosmb_stop);
}

CALLSET_ENTRY (chaosmb, start_player)
{
	chaosmb_level = 0;
	chaosmb_hits_to_relight = 1;
}
