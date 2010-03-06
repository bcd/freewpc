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

void rule_begin (void)
{
	lamp_all_off ();
	triac_disable (TRIAC_GI_MASK);
}

static void rule_msg (const char *line1, const char *line2, const char *line3, const char *line4)
{
	dmd_alloc_pair_clean ();
	font_render_string_left (&font_fixed6, 2, 2, line1)
	font_render_string_center (&font_var5, 64, 14, line2)
	font_render_string_center (&font_var5, 64, 20, line3)
	font_render_string_center (&font_var5, 64, 27, line4)
	dmd_sched_transition (&trans_sequential_boxfade);
	dmd_show2 ();
}

void rule_complete (void)
{
	task_sleep_sec (5);
	task_kill_gid (GID_RULES_LEFF);
}

void rules_spiralaward_inlanes_leff (void)
{
	lamp_tristate_flash (LM_LEFT_INLANE1);
	lamp_tristate_flash (LM_LEFT_INLANE2);
	task_sleep_sec (3);
	lamp_tristate_off (LM_LEFT_INLANE1);
	lamp_tristate_off (LM_LEFT_INLANE2);
	lamplist_apply (LAMPLIST_SPIRAL_AWARDS, lamp_flash);
}

void rules_spiralaward_loop_leff (void)
{
	lamp_tristate_flash (LM_RIGHT_SPIRAL);
}

void rules_leff (void)
{
}

void rules_deff (void)
{
	music_disable ();

	rule_begin ();
	rule_msg ("BACK TO THE ZONE", "", "HOW TO PLAY", "");
	rule_complete ();

	rule_begin ();
	rule_msg ("SPIRALAWARD", "EITHER LEFT INLANE STARTS", "A 3 SECOND TIMER", "");
	task_create_gid1 (GID_RULES_LEFF, rules_spiralaward_inlanes_leff);
	rule_complete ();
	
	rule_begin ();
	rule_msg ("SPIRALAWARD", "SHOOT A RIGHT LOOP", "TO COLLECT A RANDOM AWARD", "20 MILLION BONUS FOR COLLECTING ALL");
	task_create_gid1 (GID_RULES_LEFF, rules_spiralaward_loop_leff);
	rule_complete ();

	music_enable ();
	deff_exit ();
}

CALLSET_ENTRY (rules, sw_buyin_button)
{
	if (!in_game && !in_test)
	{
		//leff_start (LEFF_RULES);
		leff_stop_all ();
		deff_start (DEFF_RULES);
	}
}
