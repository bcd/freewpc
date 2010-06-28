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
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>

U8 jet_sound_index;
U8 jetscore;
__local__ U8 jets_scored;
__local__ U8 jets_for_bonus;
__local__ U8 jets_bonus_level;

U8 tsm_mode_timer;
extern U8 mpf_timer;

bool noflash;

void tsm_mode_init (void);
void tsm_mode_exit (void);

struct timed_mode_ops tsm_mode = {
	DEFAULT_MODE,
	.init = tsm_mode_init,
	.exit = tsm_mode_exit,
	.gid = GID_TSM_MODE_RUNNING,
	.music = MUS_TOWN_SQUARE_MADNESS,
	.deff_running = DEFF_TSM_MODE,
	.init_timer = 30,
	.prio = PRI_GAME_MODE3,
	.timer = &tsm_mode_timer,
	.grace_timer = 3,
	.pause = system_timer_pause,
};

void tsm_mode_init (void)
{
	leff_start (LEFF_JETS_ACTIVE);
//	deff_start (DEFF_TSM_MODE);
}

void tsm_mode_exit (void)
{
	leff_stop (LEFF_JETS_ACTIVE);
}

CALLSET_ENTRY(jet, start_player)
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
	jet_sound_index++;
	if (jet_sound_index >= 3)
		jet_sound_index = 0;
	
	if (timed_mode_running_p (&tsm_mode))
		sound_send (super_jet_sounds[jet_sound_index]);
	else
		sound_send (jet_sounds[jet_sound_index]);
	
	if (!noflash)
		flasher_pulse (FLASH_JETS);
	noflash = FALSE;
	//task_sleep (TIME_200MS);
	task_exit ();
}

void tsm_mode_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 5, "TOWN SQUARE MADNESS");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "JETS AT 500K");
		sprintf ("%d", tsm_mode_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
}

void jets_hit_deff (void)
{
	U8 i = 0;
	do {
	U8 x = random_scaled (4);
	U8 y = random_scaled (4);
	dmd_alloc_low_clean ();
	psprintf ("1 HIT", "%d HITS", jets_scored);
	font_render_string_center (&font_fixed6, 62 + x, 7 + y, sprintf_buffer);
	sprintf ("%d FOR NEXT LEVEL", (jets_for_bonus - jets_scored));
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	} while (i++ < 8);
	/* Redraw it so the 'HITS' text is centred */
	dmd_alloc_low_clean ();
	psprintf ("1 HIT", "%d HITS", jets_scored);
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	sprintf ("%d FOR NEXT LEVEL", (jets_for_bonus - jets_scored));
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_200MS);
	deff_exit ();
}

void jets_level_up_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("JETS LEVEL UP");
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	/* We don't use scoreget as it's likely another score
	 * has been awarded */
	sprintf("%d MILLION", jetscore);
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
	jets_scored = 1;
	dmd_show_low ();
	task_sleep_sec (2);
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

CALLSET_ENTRY (jet, sw_jet)
{
	/* Hack to work amode bug when mpf_exit switch breaks */
	if (!multi_ball_play () && mpf_timer > 0)
		callset_invoke (sw_mpf_exit);
	
	if (flag_test(FLAG_POWERBALL_IN_PLAY))
		jets_scored += 2;
	else
		jets_scored++;
	
	if (jets_scored >= jets_for_bonus)
	{	
		//TODO Set sensible bonus levels
		//bounded_increment (jets_bonus_level, 50);
		jets_bonus_level++;
		jets_for_bonus += 5;
		//TODO Set sound priority higher than jets
		sound_send (SND_GLASS_BREAKS);
		task_sleep (TIME_500MS);
		// We use jetscore as it's likely another score
		// will be awarded soon
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
		deff_start (DEFF_JETS_LEVEL_UP);
	}

	if (timed_mode_running_p (&tsm_mode))
		score (SC_500K);
	else
	{	
		score (SC_150K);
		/* Stop deff from restarting whilst we
		 * are showing the level up deff
		 * or when the hitch anim is running */
		if ((jets_scored <= jets_for_bonus) 
			&& (!timer_find_gid (GID_HITCHHIKER)))
			deff_restart (DEFF_JETS_HIT);
	}
	
	task_create_gid1 (GID_JET_SOUND, sw_jet_sound);
}

CALLSET_ENTRY (jet, lamp_update)
{
//	if (timed_mode_running_p (&tsm_mode))
//		leff_start (LEFF_JETS_ACTIVE);
//	else if (leff_running_p (LEFF_JETS_ACTIVE))
//		leff_stop (LEFF_JETS_ACTIVE);
}


CALLSET_ENTRY (jet, start_ball)
{
	leff_stop (LEFF_JETS_ACTIVE);
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

