/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (rules, __machine2__) */

#include <freewpc.h>

typedef enum {
	RULES_SPIRALAWARD=0,
	RULES_INLANES,
	RULES_CHAOSMB,
	RULES_SSSMB,
	RULES_GUMBALL,
	RULES_EXIT,
} rules_page_t;

rules_page_t rule_page;

void rule_begin (void)
{
	lamp_all_off ();
}

static void rule_msg (const char *line1, const char *line2, const char *line3, const char *line4)
{
	dmd_alloc_pair_clean ();
	font_render_string_left (&font_fixed6, 2, 2, line1)
	font_render_string_center (&font_var5, 64, 14, line2)
	font_render_string_center (&font_var5, 64, 20, line3)
	font_render_string_center (&font_var5, 64, 27, line4)
	dmd_sched_transition (&trans_sequential_boxfade);
	dmd_show_low ();
}

void rule_complete (void)
{
	task_sleep_sec (5);
	task_kill_gid (GID_RULES_LEFF);
}

void rules_leff (void)
{
}

void rules_rollover_leff (void)
{
	triac_disable (TRIAC_GI_MASK);
	for (;;)
	{
		lamp_tristate_off (LM_RIGHT_INLANE);
		lamp_tristate_on (LM_LEFT_INLANE1);
		task_sleep_sec (1);
		lamp_tristate_off (LM_LEFT_INLANE1);
		lamp_tristate_on (LM_LEFT_INLANE2);
		task_sleep_sec (1);
		lamp_tristate_off (LM_LEFT_INLANE2);
		lamp_tristate_on (LM_RIGHT_INLANE);
		task_sleep_sec (1);
	}
}

void rules_sssmb_leff (void)
{
	triac_disable (TRIAC_GI_MASK);
	for (;;)
	{
		lamp_tristate_off (LM_BONUS_X);
		lamp_tristate_flash (LM_SUPER_SKILL);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_SUPER_SKILL);
		lamp_tristate_flash (LM_MULTIBALL);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_MULTIBALL);
		lamp_tristate_flash (LM_BONUS_X);
		task_sleep (TIME_500MS);
	}
}

void rules_chaosmb_leff (void)
{
	triac_disable (TRIAC_GI_MASK);
	lamp_tristate_flash (LM_CLOCK_MILLIONS);
	task_sleep_sec (3);
	for (;;)
	{
		lamp_tristate_flash (LM_SUPER_SKILL);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_SUPER_SKILL);
		lamp_tristate_flash (LM_MULTIBALL);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_MULTIBALL);
		lamp_tristate_flash (LM_BONUS_X);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_BONUS_X);

		lamp_tristate_flash (LM_RAMP_BATTLE);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_RAMP_BATTLE);
		lamp_tristate_flash (LM_PIANO_JACKPOT);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_PIANO_JACKPOT);
		lamp_tristate_flash (LM_CAMERA);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_CAMERA);
		lamp_tristate_flash (LM_POWER_PAYOFF);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_POWER_PAYOFF);
	}
}

void rules_spiralaward_leff (void)
{
	triac_disable (TRIAC_GI_MASK);
	for (;;)
	{
		lamp_tristate_off (LM_LEFT_INLANE2);
		lamp_tristate_flash (LM_LEFT_INLANE1);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_LEFT_INLANE1);
		lamp_tristate_flash (LM_LEFT_INLANE2);
		task_sleep (TIME_500MS);
	}
}

void rules_fastlock_leff2 (void)
{
	for (;;)
	{
		lamp_tristate_off (LM_LOCK_EB);
		lamp_tristate_flash (LM_LOCK_ARROW);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_LOCK_ARROW);
		lamp_tristate_flash (LM_LOCK_EB);
		task_sleep (TIME_500MS);
	}
}

void rules_fastlock_leff (void)
{
	triac_disable (TRIAC_GI_MASK);
	task_create_peer (rules_fastlock_leff2);
	for (;;)
	{
		lamp_tristate_flash (LM_RIGHT_SPIRAL);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_RIGHT_SPIRAL);
		lamp_tristate_flash (LM_RIGHT_POWERBALL);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_RIGHT_POWERBALL);
		lamp_tristate_flash (LM_LEFT_POWERBALL);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_LEFT_POWERBALL);
		lamp_tristate_flash (LM_LEFT_SPIRAL);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_LEFT_SPIRAL);
	}
}
		
void rules_deff (void)
{
	music_disable ();
	leff_stop_all ();

	rule_begin ();
	rule_msg ("BACK TO THE ZONE", "", "HOW TO PLAY", "");
	rule_complete ();

	rule_page = RULES_SPIRALAWARD;
	rule_begin ();
	rule_msg ("SPIRALAWARD", "EITHER LEFT INLANE STARTS", "A 3 SECOND TIMER", "");
	task_create_gid1 (GID_RULES_LEFF, rules_spiralaward_leff);
	rule_complete ();
	
	rule_begin ();
	rule_msg ("SPIRALAWARD", "SHOOT A RIGHT LOOP TO COLLECT", "RANDOM AWARD" , "20M FOR COLLECTING ALL");
	lamplist_apply (LAMPLIST_SPIRAL_AWARDS, lamp_flash_on);
	lamp_tristate_flash (LM_RIGHT_SPIRAL);
	lamp_tristate_flash (LM_GUMBALL_LANE);
	task_sleep_sec (2);
	lamplist_apply (LAMPLIST_SPIRAL_AWARDS, lamp_flash_off);
	rule_complete ();

	rule_begin ();
	rule_msg ("ROLLOVERS", "USE FLIPPER BUTTONS TO STEP", "INLANE LIGHTS", "1M FOR COLLECTING ALL");
	task_create_gid1 (GID_RULES_LEFF, rules_rollover_leff);
	rule_complete ();
	
	rule_begin ();
	rule_msg ("SUPER SKILL MB", "SHOOT LEFT RAMP", "AND HIT SKILL SHOT", "DURING MULTIBALL");
	task_create_gid1 (GID_RULES_LEFF, rules_sssmb_leff);
	rule_complete ();

	rule_begin ();
	rule_msg ("CHAOS MULTIBALL", "HIT CLOCK TO LIGHT JACKPOTS", "JACKPOTS MOVE", "AROUND THE TABLE");
	task_create_gid1 (GID_RULES_LEFF, rules_chaosmb_leff);
	task_sleep_sec (3);
	rule_complete ();

	rule_begin ();
	rule_msg ("FAST LOCK", "SHOOT LOOPS TO BUILD", "JACKPOT, HIT LOCK", "TO COLLECT");
	task_create_gid1 (GID_RULES_LEFF, rules_fastlock_leff);
	rule_complete ();

	rule_begin ();
	rule_msg ("EVERYTHING ELSE?", "", "YOU CAN FIGURE", "IT OUT");
	rule_complete ();

	music_enable ();
	leff_start (LEFF_AMODE);
	deff_exit ();
}

CALLSET_ENTRY (rules, sw_buyin_button)
{
	if (!in_game && !in_test)
		deff_start (DEFF_RULES);
}
