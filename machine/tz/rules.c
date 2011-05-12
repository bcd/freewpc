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

/* CALLSET_SECTION (rules, __machine3__) */

#include <freewpc.h>

U8 rules_timer;

/* Use a seperate function to wait so we can abort early */
static void rules_sleep_sec (U8 secs)
{
	/* convert to 200ms chunks so we don't have to wait 
	 * a full second or so to abort */
	rules_timer = secs * 5;
	while (rules_timer-- != 0)
	{	
		task_sleep (TIME_200MS);
	}
}

CALLSET_ENTRY (rules, sw_left_button, sw_right_button)
{
	if (rules_timer)
	{
		rules_timer = 0;
	}
}

void rule_begin (void)
{
	lamp_all_off ();
}

static void rule_msg (const char *line1, const char *line2, const char *line3, const char *line4)
{
	dmd_alloc_pair_clean ();
	font_render_string_left (&font_fixed6, 2, 2, line1)
	font_render_string_center (&font_var5, 64, 15, line2)
	font_render_string_center (&font_var5, 64, 22, line3)
	font_render_string_center (&font_var5, 64, 29, line4)
	dmd_sched_transition (&trans_sequential_boxfade);
	dmd_show_low ();
}

void rule_complete (void)
{
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

void rules_fastlock_leff (void)
{
	for (;;)
	{
		U8 i;
		for (i = 0; i < 3; i++)
		{
			lamp_tristate_flash (LM_LEFT_POWERBALL);
			lamp_tristate_flash (LM_RIGHT_POWERBALL);
			task_sleep (TIME_500MS);
			lamp_tristate_off (LM_LEFT_POWERBALL);
			lamp_tristate_off (LM_RIGHT_POWERBALL);
			lamp_tristate_flash (LM_RIGHT_SPIRAL);
			lamp_tristate_flash (LM_LEFT_SPIRAL);
			task_sleep (TIME_500MS);
			lamp_tristate_off (LM_RIGHT_SPIRAL);
			lamp_tristate_off (LM_LEFT_SPIRAL);
		}
		lamp_tristate_flash (LM_LOCK_ARROW);
		task_sleep_sec (2);
		lamp_tristate_off (LM_LOCK_ARROW);
	}
}

void rules_hitchhiker_leff (void)
{
	for (;;)
	{
		lamp_tristate_flash (LM_RIGHT_JET);
		lamp_tristate_flash (LM_CAMERA);
		task_sleep_sec (2);
		lamp_tristate_off (LM_RIGHT_JET);
		lamp_tristate_off (LM_CAMERA);
		sol_request (SOL_UR_FLIP_POWER);
		task_sleep (TIME_500MS);
	}
}

void rules_ramp_battle_leff (void)
{
	for (;;)
	{
		triac_disable (GI_POWERFIELD);
		lamp_tristate_flash (LM_RAMP_BATTLE);
		task_sleep_sec (4);
		lamp_tristate_off (LM_RAMP_BATTLE);
		triac_enable (GI_POWERFIELD);
		task_sleep_sec (3);
	}
}

void rules_powerfield_leff (void)
{
	for (;;)
	{
		triac_enable (GI_POWERFIELD);
		task_sleep_sec (2);
		triac_disable (GI_POWERFIELD);
		leff_start (LEFF_MPF_HIT);
		task_sleep_sec (2);
	}
}

void rules_spiralaward2_leff (void)
{
	for (;;)
	{
		lamplist_apply (LAMPLIST_SPIRAL_AWARDS, lamp_flash_on);
		lamp_tristate_flash (LM_RIGHT_SPIRAL);
		lamp_tristate_flash (LM_RIGHT_POWERBALL);
		task_sleep_sec (1);
		sol_request (SOL_LL_FLIP_POWER);
		lamp_tristate_off (LM_RIGHT_SPIRAL);
		lamp_tristate_off (LM_RIGHT_POWERBALL);
		task_sleep (TIME_500MS);
	}
}

void rules_doinks_leff (void)
{
	for (;;)
	{
		lamp_tristate_flash (LM_RIGHT_INLANE);
		task_sleep_sec (1);
		sol_request (SOL_LR_FLIP_POWER);
		lamp_tristate_off (LM_RIGHT_INLANE);
		lamp_tristate_flash (LM_SUPER_SKILL);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_SUPER_SKILL);
		lamp_tristate_flash (LM_MULTIBALL);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_MULTIBALL);
		lamp_tristate_flash (LM_BONUS_X);
		task_sleep (TIME_500MS);
		lamp_tristate_off (LM_BONUS_X);
	}
}

void rules_oddchange1_leff (void)
{
	for (;;)
	{
		lamp_tristate_flash (LM_LL_5M);
		task_sleep (TIME_200MS);
		lamp_tristate_off (LM_LL_5M);

		lamp_tristate_flash (LM_ML_5M);
		task_sleep (TIME_200MS);
		lamp_tristate_off (LM_ML_5M);

		lamp_tristate_flash (LM_UL_5M);
		task_sleep (TIME_200MS);
		lamp_tristate_off (LM_UL_5M);

		lamp_tristate_flash (LM_UR_5M);
		task_sleep (TIME_200MS);
		lamp_tristate_off (LM_UR_5M);
		
		lamp_tristate_flash (LM_MR1_5M);
		task_sleep (TIME_200MS);
		lamp_tristate_off (LM_MR1_5M);

		lamp_tristate_flash (LM_MR2_5M);
		task_sleep (TIME_200MS);
		lamp_tristate_off (LM_MR2_5M);

		lamp_tristate_flash (LM_LR_5M);
		task_sleep (TIME_200MS);
		lamp_tristate_off (LM_LR_5M);
	}
}

void rules_oddchange2_leff (void)
{
	for (;;)
	{
		lamp_tristate_flash (LM_PIANO_JACKPOT);
		task_sleep_sec (1);
		lamp_tristate_off (LM_PIANO_JACKPOT);

		lamp_tristate_flash (LM_SLOT_MACHINE);
		task_sleep_sec (1);
		lamp_tristate_off (LM_SLOT_MACHINE);
	}
}

void rules_deff (void)
{
	music_disable ();
	leff_stop_all ();

	rule_begin ();
	triac_disable (TRIAC_GI_MASK);
	rule_msg ("BACK TO THE ZONE", "", "HOW TO PLAY", "");
	rules_sleep_sec (5);
	rule_complete ();

	rule_begin ();
	rule_msg ("SPIRALAWARD", "EITHER LEFT INLANE STARTS", "A 3 SECOND TIMER", "");
	task_create_gid1 (GID_RULES_LEFF, rules_spiralaward_leff);
	rules_sleep_sec (6);
	rule_complete ();
	
	rule_begin ();
	rule_msg ("SPIRALAWARD", "SHOOT A RIGHT LOOP TO COLLECT", "A RANDOM AWARD" , "20M FOR COLLECTING ALL");
	task_create_gid1 (GID_RULES_LEFF, rules_spiralaward2_leff);
	rules_sleep_sec (5);
	rule_complete ();
	lamplist_apply (LAMPLIST_SPIRAL_AWARDS, lamp_flash_off);
	lamp_tristate_off (LM_RIGHT_SPIRAL);
	lamp_tristate_off (LM_RIGHT_POWERBALL);

	rule_begin ();
	rule_msg ("ROLLOVERS", "USE FLIPPER BUTTONS TO STEP", "INLANE LIGHTS", "1M FOR COLLECTING ALL");
	task_create_gid1 (GID_RULES_LEFF, rules_rollover_leff);
	rules_sleep_sec (9);
	rule_complete ();
	
	rule_begin ();
	rule_msg ("SUPER SKILL MB", "SHOOT LEFT RAMP", "AND HIT SKILL SHOT", "DURING MULTIBALL");
	task_create_gid1 (GID_RULES_LEFF, rules_sssmb_leff);
	rules_sleep_sec (9);
	rule_complete ();

	rule_begin ();
	rule_msg ("CHAOS MULTIBALL", "HIT CLOCK TO LIGHT JACKPOTS", "JACKPOTS MOVE", "AROUND THE TABLE");
	task_create_gid1 (GID_RULES_LEFF, rules_chaosmb_leff);
	rules_sleep_sec (9);
	rule_complete ();
	
	rule_begin ();
	rule_msg ("FASTLOCK", "SHOOT FAST LOOPS TO", "BUILD UP JACKPOTS", "HIT LOCK TO COLLECT");
	task_create_gid1 (GID_RULES_LEFF, rules_fastlock_leff);
	rules_sleep_sec (9);
	rule_complete ();

	rule_begin ();
	rule_msg ("HITCHHIKER", "SHOOT THE HOLE BETWEEN", "THE FLASHING LAMPS", "TO SCORE A HITCHHIKER");
	task_create_gid1 (GID_RULES_LEFF, rules_hitchhiker_leff);
	rules_sleep_sec (9);
	rule_complete ();

	
	rule_begin ();
	rule_msg ("POWERFIELD", "HITCHHIKERS UNLOCK", "THE RIGHT RAMP", "TO THE POWERFIELD");
	task_create_gid1 (GID_RULES_LEFF, rules_ramp_battle_leff);
	rules_sleep_sec (9);
	rule_complete ();

	rule_begin ();
	rule_msg ("POWERFIELD", "PRESS THE FLIPPER BUTTONS", "TO SHOOT THE BALL INTO", "THE TOP OF THE POWERFIELD");
	task_create_gid1 (GID_RULES_LEFF, rules_powerfield_leff);
	rules_sleep_sec (9);
	triac_disable (GI_POWERFIELD);
	rule_complete ();

	rule_begin ();
	rule_msg ("DOINK MODE", "SHOOT THE LEFT RAMP", "FROM THE RIGHT INLANE", "TO START DOINK MODE");
	task_create_gid1 (GID_RULES_LEFF, rules_doinks_leff);
	rules_sleep_sec (9);
	rule_msg ("DOINK MODE", "HIT THE FLIPPER BUTTONS", "WHILST THE BALL IS BEING HELD", "TO SCORE POINTS");
	rules_sleep_sec (9);
	rule_complete ();
	
	rule_begin ();
	rule_msg ("ODDCHANGE", "HITTING THE YELLOW", "TARGETS RANDOMLY BUILDS", "THE ODDCHANGE POT");
	task_create_gid1 (GID_RULES_LEFF, rules_oddchange1_leff);
	rules_sleep_sec (9);
	rule_complete ();

	rule_begin ();
	rule_msg ("ODDCHANGE", "HIT THE PIANO OR", "THE SLOT MACHINE WHEN", "UNLIT TO COLLECT");
	task_create_gid1 (GID_RULES_LEFF, rules_oddchange2_leff);
	rules_sleep_sec (9);
	rule_complete ();

	rule_begin ();
	rule_msg ("THANKS", "WE HOPE YOU", "ENJOY PLAYING","WWW.ODDCHANGE.COM/FREEWPC");
	rules_sleep_sec (4);
	rule_complete ();
	
	music_enable ();
	leff_start (LEFF_AMODE);
	deff_exit ();
}

CALLSET_ENTRY (rules, init)
{
	rules_timer = 0;
}

CALLSET_ENTRY (rules, start_game)
{
	task_kill_gid (GID_RULES_LEFF);
	deff_stop (DEFF_RULES);
}

CALLSET_ENTRY (rules, sw_buyin_button)
{
	if (deff_get_active () == DEFF_AMODE)
		deff_start (DEFF_RULES);
}
