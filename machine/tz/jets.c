/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com
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
 * You shoud have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* CALLSET_SECTION (jets, __machine__) */

#include <freewpc.h>
#include <status.h>

#define TOTAL_BAR_WIDTH 106
#define SEG_SIZE 5

U8 jet_sound_index;
U8 jetscore;
U8 jets_scored;
U8 jets_for_bonus;
U8 jets_bonus_level;

U8 tsm_mode_timer;
extern U8 mpf_timer;
score_t tsm_mode_total;
/* Used to not flash the jets when the clock or
 * slings are triggered */
bool noflash;
/* Used to tell the TSM deff that a jet was hit during TSM */
bool tsm_hit;

extern void award_unlit_shot (U8 unlit_called_from);

void tsm_mode_init (void);
void tsm_mode_exit (void);

struct timed_mode_ops tsm_mode = {
	DEFAULT_MODE,
	.init = tsm_mode_init,
	.exit = tsm_mode_exit,
	.gid = GID_TSM_MODE_RUNNING,
	.music = MUS_TOWN_SQUARE_MADNESS,
	.deff_running = DEFF_TSM_MODE,
	.deff_ending = DEFF_TSM_MODE_TOTAL,
	.init_timer = 30,
	.prio = PRI_GAME_MODE3,
	.timer = &tsm_mode_timer,
	.grace_timer = 3,
	.pause = system_timer_pause,
};

struct progress_bar_ops jets_progress_bar = {
	.x = 8,
	.y = 26,
	.fill_level = &jets_scored,
	.max_level = &jets_for_bonus,
	.bar_width = 106,
};

void jets_active_task (void)
{
	while (timed_mode_running_p (&tsm_mode))
	{
		lamp_tristate_off (LM_RIGHT_JET);
		lamp_tristate_on (LM_LEFT_JET);
		task_sleep (TIME_100MS);	
		lamp_tristate_off (LM_LEFT_JET);
		lamp_tristate_on (LM_LOWER_JET);
		task_sleep (TIME_100MS);	
		lamp_tristate_off (LM_LOWER_JET);
		lamp_tristate_on (LM_RIGHT_JET);
		task_sleep (TIME_100MS);	
	}
	lamp_tristate_on (LM_LEFT_JET);
	lamp_tristate_on (LM_LOWER_JET);
	lamp_tristate_on (LM_RIGHT_JET);
	task_exit ();
}

void tsm_mode_init (void)
{
	task_create_gid (GID_JETS_ACTIVE_TASK, jets_active_task);
	score_zero (tsm_mode_total);
	init_all_dollars ();
}

void tsm_mode_exit (void)
{
	task_kill_gid (GID_JETS_ACTIVE_TASK);
}

CALLSET_ENTRY(jet, start_ball)
{
	jets_scored = 0;
	jets_for_bonus = 10;
	jets_bonus_level = 0;
	lamp_tristate_on (LM_LEFT_JET);
	lamp_tristate_on (LM_LOWER_JET);
	lamp_tristate_on (LM_RIGHT_JET);
}

sound_code_t jet_sounds[] = { 
	SND_HORN1, SND_HORN2, SND_HORN3
};

sound_code_t super_jet_sounds[] = { 
	SND_TSM_HIT_1, SND_TSM_HIT_2, SND_TSM_HIT_3
};

void sw_jet_sound (void)
{
	if (!in_live_game)
		return;
	jet_sound_index = random_scaled(3);
	if (timed_mode_running_p (&tsm_mode))
		sound_send (super_jet_sounds[jet_sound_index]);
	else
		sound_send (jet_sounds[jet_sound_index]);
	/* Hack to stop slings and clock target from flashing jets */	
	if (!noflash)
		flasher_pulse (FLASH_JETS);
	noflash = FALSE;
	task_exit ();
}

void tsm_mode_total_deff (void)
{
	dmd_alloc_pair_clean ();
	dmd_map_overlay ();
	dmd_clean_page_low ();

	font_render_string_center (&font_fixed6, 64, 5, "TSM OVER");
	sprintf_score (tsm_mode_total);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	font_render_string_center (&font_var5, 64, 27, "POINTS EARNED FROM MODE");
	dmd_text_outline ();
	dmd_alloc_pair ();
	frame_draw (IMG_CITY);
	dmd_overlay_outline ();
	dmd_show2 ();
	task_sleep_sec (2);
	dmd_sched_transition (&trans_bitfade_fast);
	deff_exit ();
}


void tsm_mode_deff (void)
{	
	dmd_alloc_pair_clean ();
	for (;;)
	{
		dmd_map_overlay ();
		dmd_clean_page_low ();
		font_render_string_center (&font_var5, 64, 5, "TOWN SQUARE MADNESS");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "JETS AT 500K");
		sprintf ("%d", tsm_mode_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (IMG_CITY);
		dollar_overlay ();
		dmd_overlay_outline ();

		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
}

void jets_hit_deff (void)
{
	U16 fno;
	U16 img_start;
	U16 img_end;
	U8 x;
	U8 y;

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
		x = random_scaled (4);
		y = random_scaled (4);
		dmd_map_overlay ();
		dmd_clean_page_low ();

		psprintf ("1 HIT", "%d HITS", jets_scored);
		font_render_string_center (&font_fixed6, 62 + x, 7 + y, sprintf_buffer);
		sprintf ("%d FOR LEVEL %d", (jets_for_bonus - jets_scored), jets_bonus_level + 1);
		font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		callset_invoke (score_overlay);
		dmd_overlay_outline ();
		
		draw_progress_bar (&jets_progress_bar);
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
	for (fno = 0; fno < 10; fno++)
	{
		dmd_alloc_pair_clean ();
		dmd_clean_page_low ();
		psprintf ("1 HIT", "%d HITS", jets_scored);
		font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
		sprintf ("%d FOR LEVEL %d", (jets_for_bonus - jets_scored), jets_bonus_level + 1);
		font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
		/* Copy to the high page so it doesn't look dark */
		dmd_copy_low_to_high ();
		callset_invoke (score_overlay);
		draw_progress_bar (&jets_progress_bar);
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}
	deff_exit ();
}

void jets_level_up_deff (void)
{
	dmd_alloc_pair_clean ();
	U16 fno;
	sound_send (SND_GLASS_BREAKS);
	for (fno = IMG_EXPLODE_START; fno < IMG_EXPLODE_END; fno += 2)
	{
		dmd_map_overlay ();
		dmd_clean_page_low ();
		
		if (fno > 4)
		{
			sprintf ("TOWN SQUARE LEVEL %d", jets_bonus_level);
			font_render_string_center (&font_mono5, 64, 7, sprintf_buffer);
			/* We don't use scoreget as it's likely another score
			 * has been awarded */
			sprintf("%d MILLION", jetscore);
			font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
		}
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		callset_invoke (score_overlay);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
	for (fno = 0; fno < 10; fno++)
	{
		dmd_alloc_pair_clean ();
		sprintf ("TOWN SQUARE LEVEL %d", jets_bonus_level);
		font_render_string_center (&font_mono5, 64, 7, sprintf_buffer);
		/* We don't use scoreget as it's likely another score
		 * has been awarded */
		sprintf("%d MILLION", jetscore);
		font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
		dmd_copy_low_to_high ();
		callset_invoke (score_overlay);
		dmd_show2 ();	
		task_sleep (TIME_100MS);
	}
	deff_exit ();
}

/* Hack to make slings work as jets */
CALLSET_ENTRY (jet, sw_sling)
{
	callset_invoke (sw_jet_noflash);
}

CALLSET_ENTRY (jet, sw_jet_noflash)
{
	noflash = TRUE;
	callset_invoke (sw_jet);
}

static void jets_level_up (void)
{
	timer_restart_free (GID_JETS_LEVEL_UP, TIME_2S);
	jets_scored = 1;
	bounded_increment (jets_bonus_level, 50);
	if (jets_for_bonus <= 195)
		jets_for_bonus += 5;
	award_unlit_shot (SW_BOTTOM_JET);
	/* jetscore is used rather than score_deff_get 
	 * because it's likely another score woud of
	 * happened */
	if (jets_bonus_level < 3)
	{
		score (SC_1M);
		jetscore = 1;
	}
	else if (jets_bonus_level < 5)
	{
		score (SC_5M);
		jetscore = 5;
	}
	else if (jets_bonus_level < 7)
	{
		score (SC_10M);
		jetscore = 10;
	}
	if (!timer_find_gid (GID_HITCHHIKER))
		deff_start (DEFF_JETS_LEVEL_UP);
}

CALLSET_ENTRY (jet, sw_jet)
{
	
	task_create_gid1 (GID_JET_SOUND, sw_jet_sound);
	if (global_flag_test(GLOBAL_FLAG_POWERBALL_IN_PLAY))
		jets_scored += 2;
	else
		jets_scored++;
	
	if (jets_scored >= jets_for_bonus)
	{	
		jets_level_up ();
	}

	if (timed_mode_running_p (&tsm_mode))
	{
		score (SC_500K);
		score_add (tsm_mode_total, score_table[SC_500K]);
		callset_invoke (respawn_dollar);
	}
	else
	{	
		score (SC_150K);
		/* Stop deff from restarting whilst we
		 * are showing the level up deff
		 * or when the hitch anim is running */
		if (!timer_find_gid (GID_HITCHHIKER) && !task_find_gid (GID_JETS_LEVEL_UP))
			deff_restart (DEFF_JETS_HIT);
	}
	/* Hack for when mpf_exit switch breaks */
	if (!multi_ball_play () && mpf_timer > 0)
		callset_invoke (sw_mpf_exit);
}

CALLSET_ENTRY (jets, end_ball)
{
	timed_mode_end (&tsm_mode);
}

CALLSET_ENTRY (jet, display_update)
{
	timed_mode_display_update (&tsm_mode);
}

CALLSET_ENTRY (jet, music_refresh)
{
	timed_mode_music_refresh (&tsm_mode);
}

CALLSET_ENTRY (jet, door_start_tsm)
{
	timed_mode_begin (&tsm_mode);
}

CALLSET_ENTRY (jet, status_report)
{
	status_page_init ();
	sprintf ("JET LEVEL %d", jets_bonus_level + 1);
	font_render_string_center (&font_mono5, 64, 9, sprintf_buffer);
	sprintf ("%d FOR LEVEL UP", (jets_for_bonus - jets_scored));
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
	status_page_complete ();
}
