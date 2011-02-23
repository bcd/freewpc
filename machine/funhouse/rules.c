/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

#define TIME_0_MIN  0
#define TIME_15_MIN 3
#define TIME_30_MIN 6
#define TIME_45_MIN 9


static void rule_begin (void)
{
	lamp_all_off ();
	triac_disable (TRIAC_GI_MASK);
}

static void rule_msg (const char *line1, const char *line2)
{
	seg_alloc_clean ();
	seg_write_row_center (0, line1);
	seg_write_row_center (1, line2);
	seg_sched_transition (&seg_trans_fast_center_out);
	seg_show ();
}

static void rule_complete (void)
{
	task_sleep_sec (5);
	task_kill_gid (GID_RULES_LEFF);
}

void rules_clock_leff2 (void)
{
	for (;;)
	{
		flasher_pulse (FLASH_CLOCK_FLASHERS);
		task_sleep (TIME_500MS);
	}
}

void rules_clock_leff (void)
{
	task_create_peer (rules_clock_leff2);
	fh_clock_set (0, TIME_0_MIN);
	for (;;)
	{
		task_sleep (TIME_66MS);
		fh_clock_advance_step ();
	}
}

void rules_flasher_leff (void)
{
	for (;;)
	{
		flasher_pulse (FLASH_BLUE_FLASHERS);
		task_sleep (TIME_66MS);
		flasher_pulse (FLASH_DUMMY_FLASHER);
		task_sleep (TIME_66MS);
		flasher_pulse (FLASH_CLEAR_FLASHERS);
		task_sleep (TIME_66MS);
		flasher_pulse (FLASH_SUPER_DOG_FLASH);
		task_sleep (TIME_66MS);
		flasher_pulse (FLASH_RED_FLASHERS);
		task_sleep (TIME_66MS);
		flasher_pulse (FLASH_CLOCK_FLASHERS);
		task_sleep (TIME_66MS);
	}
}

void rules_deff (void)
{
	music_disable ();
	music_set (MUS_RESTART_PLUNGER);
	sol_request (SOL_EYELIDS_OPEN);

	rule_begin ();
	rule_msg ("HOW TO PLAY", "FUNHOUSE");
	task_create_gid1 (GID_RULES_LEFF, rules_flasher_leff);
	rule_complete ();

	rule_begin ();
	rule_msg ("ALL TARGETS", "ADVANCE CLOCK");
	task_create_gid1 (GID_RULES_LEFF, rules_clock_leff);
	rule_complete ();

	rule_begin ();
	rule_msg ("REACH 11:30", "TO LIGHT LOCK");
	fh_clock_set (11, TIME_30_MIN);
	lamp_tristate_flash (LM_LOCK);
	rule_complete ();

	rule_begin ();
	rule_msg ("LOCK BALL 1", "FOR 11:45");
	fh_clock_set (11, TIME_45_MIN);
	lamp_tristate_flash (LM_LOCK);
	rule_complete ();

	rule_begin ();
	rule_msg ("LOCK BALL 2", " FOR 12:00");
	fh_clock_set (12, TIME_0_MIN);
	lamp_tristate_flash (LM_MILLION);
	sol_request (SOL_EYELIDS_CLOSED);
	rule_complete ();

	rule_begin ();
	rule_msg ("WAKE RUDY", "FOR MULTIBALL");
	sol_request (SOL_EYELIDS_OPEN);
	rule_complete ();

	rule_begin ();
	rule_msg ("SHOOT TRAP DOOR", "FOR JACKPOTS");
	lamp_tristate_flash (LM_MILLION_PLUS);
	rule_complete ();

	rule_begin ();
	rule_msg ("SHOOT MIRROR TO", "COLLECT AWARDS");
	lamplist_apply (LAMPLIST_MIRROR_AWARDS, lamp_flash_on);
	lamp_tristate_flash (LM_MIRROR_VALUE);
	rule_complete ();

	rule_begin ();
	rule_msg ("HIT RUDY TO", "RELIGHT MIRROR");
	lamplist_apply (LAMPLIST_MIRROR_AWARDS, lamp_on);
	rule_complete ();

	rule_begin ();
	rule_msg ("BLUE TARGETS", "LIGHT THE STEPS");
	lamplist_apply (LAMPLIST_STEP_TARGETS, lamp_flash_on);
	lamplist_apply (LAMPLIST_STEPS_AWARDS, lamp_flash_on);
	rule_complete ();

	rule_begin ();
	rule_msg ("SHOOT STEPS FROM", "LEFT PLUNGER");
	lamplist_apply (LAMPLIST_STEPS_AWARDS, lamp_flash_on);
	lamp_tristate_flash (LM_STEPS_GATE_OPEN);
	lamp_tristate_flash (LM_RAMP_STEPS);
	rule_complete ();

	rule_begin ();
	rule_msg ("SHOOT LOOPS TO", "COLLECT GANGWAYS");
	lamp_flash_on (LM_LEFT_GANGWAY);
	lamp_flash_on (LM_RIGHT_GANGWAY);
	lamplist_apply (LAMPLIST_GANGWAYS, lamp_on);
	rule_complete ();

	rule_begin ();
	rule_msg ("EXTRA BALLS LIT", "AT TOP LOOP");
	lamp_on (LM_EXTRA_BALL);
	lamp_flash_on (LM_MIRROR_EX_BALL);
	lamp_flash_on (LM_GANGWAY_EX_BALL);
	lamp_flash_on (LM_STEPS_EB);
	rule_complete ();

	rule_begin ();
	rule_msg ("PLAY", "FUNHOUSE");
	task_create_gid1 (GID_RULES_LEFF, rules_flasher_leff);
	task_create_gid1 (GID_RULES_LEFF, rules_clock_leff);
	rule_complete ();

	sol_request (SOL_EYELIDS_CLOSED);
	music_enable ();
	deff_exit ();
}

