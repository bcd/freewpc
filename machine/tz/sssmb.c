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
//TODO Relight rules?

#include <freewpc.h>

/* Super Skill Shot multiball rules */

extern void mball_start_3_ball (void);
extern void maybe_ramp_divert (void);
extern U8 autofire_request_count;
extern bool mball_jackpot_uncollected;

U8 sssmb_initial_ramps_to_divert;
U8 sssmb_ramps_to_divert;
U8 sssmb_jackpot_value;
bool sssmb_ball_in_plunger;

void sssmb_running_deff (void)
{
	for (;;)
	{
		score_update_start ();
		//dmd_alloc_low_high ();
		dmd_alloc_pair ();
		dmd_clean_page_low ();

		font_render_string_center (&font_term6, 64, 4, "SKILL MULTIBALL");

		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);

		dmd_copy_low_to_high ();

		if (timer_find_gid (GID_SSSMB_DIVERT_DEBOUNCE))
		{
			sprintf ("SKILL SHOT SCORES JACKPOT");
		}
		else if (sssmb_ramps_to_divert == 0)
		{
			sprintf ("SHOOT LEFT RAMP NOW");
		}
		else if (sssmb_ramps_to_divert == 1)
		{
			sprintf ("1 RAMP FOR SKILL SHOT");
		}
		else
		{
			sprintf ("%d RAMPS FOR SKILL SHOT", sssmb_ramps_to_divert);
		}
		font_render_string_center (&font_var5, 64, 26, sprintf_buffer);

		dmd_show_low ();
		while (!score_update_required ())
		{
			task_sleep (TIME_66MS);
			dmd_show_other ();
		}
	}
}

void sssmb_jackpot_lit_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 9, "GET THE");
	font_render_string_center (&font_fixed10, 64, 22, "JACKPOT");
	dmd_show_low ();
	sound_send (SND_SPIRAL_EB_LIT);
	task_sleep_sec (2);
	for (;;)
	{
		dmd_alloc_low_clean ();
		sprintf ("JACKPOT IS %d,000,000", sssmb_jackpot_value);
		font_render_string_center (&font_var5, 64, 24, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_100MS);
	}
}

void sssmb_jackpot_collected_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_term6, 64, 10, "SKILL JACKPOT");
	printf_millions (sssmb_jackpot_value);
	font_render_string_center (&font_fixed6, 64, 21, sprintf_buffer);
	sound_send (SND_SKILL_SHOT_CRASH_3);
	dmd_show_low ();
	task_sleep_sec (1);
	sound_send (SND_SKILL_SHOT_CRASH_3);
	task_sleep_sec (1);
	deff_exit ();
}


void sssmb_relight_all_jackpots (void)
{
	flag_on (FLAG_SSSMB_RED_JACKPOT);
	flag_on (FLAG_SSSMB_ORANGE_JACKPOT);
	flag_on (FLAG_SSSMB_YELLOW_JACKPOT);
}


void sssmb_award_jackpot (void)
{
	/* Hack to make sure restart mball doesn't start after
	 * the Mball/Sssmb combo mode */
	mball_jackpot_uncollected = FALSE;
	sssmb_initial_ramps_to_divert++;
	score_1M (sssmb_jackpot_value);
	deff_start (DEFF_JACKPOT);
	deff_start (DEFF_SSSMB_JACKPOT_COLLECTED);
	sound_send (SND_EXPLOSION_1);

	/* score it */

	if (sssmb_jackpot_value < 100)
		sssmb_jackpot_value += 10;
	sssmb_ramps_to_divert = sssmb_initial_ramps_to_divert;

	if (!flag_test (FLAG_SSSMB_RED_JACKPOT)
		&& !flag_test (FLAG_SSSMB_ORANGE_JACKPOT)
		&& !flag_test (FLAG_SSSMB_YELLOW_JACKPOT))
	{
		sssmb_relight_all_jackpots ();
	}
}


void sssmb_jackpot_ready_task (void)
{
	deff_start (DEFF_SSSMB_JACKPOT_LIT);
	sound_send (SND_HEEHEE);
	task_sleep_sec (4);
	sound_send (SND_FASTER);
	sssmb_jackpot_value--;
	task_sleep_sec (2);
	sssmb_jackpot_value--;
	task_sleep_sec (2);
	sssmb_jackpot_value--;
	sound_send (SND_PUT_IT_BACK_2);
	task_sleep_sec (2);
	sssmb_jackpot_value--;
	task_sleep_sec (2);
	sssmb_jackpot_value--;
	sound_send (SND_OH_NO);
	task_sleep_sec (2);
	task_exit ();
}


CALLSET_ENTRY (sssmb, sssmb_start)
{
	if (!flag_test (FLAG_SSSMB_RUNNING))
	{
		deff_update ();
		music_refresh ();
		flag_on (FLAG_SSSMB_RUNNING);
		flag_on (FLAG_SSSMB_RED_JACKPOT);
		flag_on (FLAG_SSSMB_ORANGE_JACKPOT);
		flag_on (FLAG_SSSMB_YELLOW_JACKPOT);
		sssmb_initial_ramps_to_divert = 1;
		sssmb_ramps_to_divert = 0;
		sssmb_jackpot_value = 20;
		sssmb_ball_in_plunger = FALSE;
		ballsave_add_time (10);
		mball_start_3_ball ();
	}
}


void sssmb_stop (void)
{
	if (flag_test (FLAG_SSSMB_RUNNING))
	{
		flag_off (FLAG_SSSMB_RUNNING);
		flag_off (FLAG_SSSMB_RED_JACKPOT);
		flag_off (FLAG_SSSMB_ORANGE_JACKPOT);
		flag_off (FLAG_SSSMB_YELLOW_JACKPOT);
		timer_kill_gid (GID_SSSMB_DIVERT_DEBOUNCE);
		task_kill_gid (GID_SSSMB_JACKPOT_READY);
		deff_stop (DEFF_SSSMB_RUNNING);
		lamp_tristate_off (LM_SUPER_SKILL);
		music_refresh ();
		//autofire_request_count = 0;
	}
}

CALLSET_ENTRY (sssmb, lamp_update)
{
	if (flag_test (FLAG_SSSMB_RUNNING) && sssmb_ramps_to_divert > 0)
		lamp_tristate_on (LM_SUPER_SKILL);
	else if (flag_test (FLAG_SSSMB_RUNNING) && sssmb_ramps_to_divert == 0 && !sssmb_ball_in_plunger)
		lamp_tristate_flash (LM_SUPER_SKILL);
}

CALLSET_ENTRY (sssmb, display_update)
{
	if (flag_test (FLAG_SSSMB_RUNNING))
		deff_start_bg (DEFF_SSSMB_RUNNING, 0);
}

CALLSET_ENTRY (sssmb, music_refresh)
{
	if (flag_test (FLAG_SSSMB_RUNNING))
		music_request (MUS_SPIRAL_ROUND, PRI_GAME_MODE1 + 9);
}


CALLSET_ENTRY (sssmb, door_start_super_skill)
{
	callset_invoke (sssmb_start);
}


CALLSET_ENTRY (sssmb, single_ball_play)
{
	sssmb_stop ();
}

CALLSET_ENTRY (sssmb, skill_red)
{
	if (flag_test (FLAG_SSSMB_RUNNING)
		&& flag_test (FLAG_SSSMB_RED_JACKPOT))
	{
		flag_off (FLAG_SSSMB_RED_JACKPOT);
		sssmb_award_jackpot ();
	}
}

CALLSET_ENTRY (sssmb, skill_orange)
{
	if (flag_test (FLAG_SSSMB_RUNNING)
		&& flag_test (FLAG_SSSMB_ORANGE_JACKPOT))
	{
		flag_off (FLAG_SSSMB_ORANGE_JACKPOT);
		sssmb_award_jackpot ();
	}
}

CALLSET_ENTRY (sssmb, skill_yellow)
{
	if (flag_test (FLAG_SSSMB_RUNNING)
		&& flag_test (FLAG_SSSMB_YELLOW_JACKPOT))
	{
		flag_off (FLAG_SSSMB_YELLOW_JACKPOT);
		sssmb_award_jackpot ();
	}
}

/* Called from leftramp.c */
void sssmb_left_ramp_exit (void)
{
	if (flag_test (FLAG_SSSMB_RUNNING))
	{
		if (sssmb_ramps_to_divert == 0)
		{
			if (!timer_find_gid (GID_SSSMB_DIVERT_DEBOUNCE))
			{
				timer_start_free (GID_SSSMB_DIVERT_DEBOUNCE, TIME_6S);
			//	maybe_ramp_divert ();
			}
		}
		else
		{
			bounded_decrement (sssmb_ramps_to_divert, 0);
		}
		score_update_required ();
	}
}

CALLSET_ENTRY (sssmb, sw_shooter)
{
	if (flag_test (FLAG_SSSMB_RUNNING)
		&& timer_find_gid (GID_SSSMB_DIVERT_DEBOUNCE))
	{
		extern U8 skill_switch_reached;
		skill_switch_reached = 0;
		/* TODO: handle case where red jackpot switch is tripped but
		ball falls back to plunger lane */
		task_create_gid1 (GID_SSSMB_JACKPOT_READY, sssmb_jackpot_ready_task);
	}
}

CALLSET_ENTRY (sssmb, any_skill_switch)
{
	dbprintf ("Jackpot ready cancelled\n");
	sssmb_ball_in_plunger = FALSE;
	task_kill_gid (GID_SSSMB_JACKPOT_READY);
	deff_stop (DEFF_SSSMB_JACKPOT_LIT);
}

CALLSET_ENTRY (sssmb, start_game)
{
	sssmb_stop ();
}

