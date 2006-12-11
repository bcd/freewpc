/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

bool skill_shot_enabled;
U8 skill_switch_reached;

void skill_shot_ready_deff (void)
{
	dmd_alloc_low_high ();
	dmd_clean_page_low ();
	font_render_string (&font_mono5, 2, 3, "YELLOW");
	font_render_string (&font_mono5, 2, 12, "ORANGE");
	font_render_string (&font_mono5, 2, 21, "RED");

	dmd_copy_low_to_high ();
	font_render_string_right (&font_mono5, 120, 3, "100,000");
	font_render_string_right (&font_mono5, 120, 12, "50,000");
	font_render_string_right (&font_mono5, 120, 21, "25,000");
	dmd_show_low ();
	for (;;)
	{
		task_sleep (TIME_100MS);
		dmd_show_other ();
	}
}

void enable_skill_shot (void)
{
	skill_shot_enabled = TRUE;
	skill_switch_reached = 0;
	deff_start (DEFF_SKILL_SHOT_READY);
}

void disable_skill_shot (void)
{
	skill_shot_enabled = FALSE;
	deff_stop (DEFF_SKILL_SHOT_READY);
}

void skill_shot_made_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_sched_transition (&trans_scroll_up);
	font_render_string_center (&font_fixed10, 64, 8, "SKILL SHOT");
	switch (skill_switch_reached)
	{
		case 1:
			sprintf ("25,000");
			break;
		case 2:
			sprintf ("50,000");
			break;
		case 3:
			sprintf ("100,000");
			break;
	}
	font_render_string_center (&font_times8, 64, 23, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (1);
	deff_exit ();
}


static void award_skill_shot (void)
{
	mark_ball_in_play ();
	deff_start (DEFF_SKILL_SHOT_MADE);
	leff_restart (LEFF_FLASHER_HAPPY);
	sound_send (SND_SKILL_SHOT_CRASH_1);
	disable_skill_shot ();
	switch (skill_switch_reached)
	{
		case 1: 
			score (SC_25K);
			break;
		case 2: 
			score (SC_50K); 
			timed_game_extend (5);
			break;
		case 3: 
			score (SC_100K); 
			timed_game_extend (10);
			break;
	}
}

static void skill_switch_monitor (void) __taskentry__
{
	if (skill_switch_reached < 3)
		task_sleep_sec (1);
	else
		task_sleep_sec (3);
	award_skill_shot ();
	task_exit ();
}


static void award_skill_switch (U8 sw)
{
	switch_can_follow (any_skill_switch, slot, TIME_3S);
	if (!skill_shot_enabled)
		return;

	if (skill_switch_reached < sw)
	{
		skill_switch_reached = sw;
		task_recreate_gid (GID_SKILL_SWITCH_TRIGGER, skill_switch_monitor);
		sound_send (skill_switch_reached + SND_SKILL_SHOT_RED);
	}
	else
	{
		task_kill_gid (GID_SKILL_SWITCH_TRIGGER);
		award_skill_shot ();
	}
}


CALLSET_ENTRY (skill, sw_skill_bottom)
{
	award_skill_switch (1);
}


CALLSET_ENTRY (skill, sw_skill_center)
{
	award_skill_switch (2);
}


CALLSET_ENTRY (skill, sw_skill_top)
{
	award_skill_switch (3);
}


CALLSET_ENTRY (skill, start_ball)
{
	enable_skill_shot ();
}


CALLSET_ENTRY (skill, ball_in_play)
{
	disable_skill_shot ();
}


CALLSET_ENTRY (skill, end_game)
{
	disable_skill_shot ();
}

CALLSET_ENTRY (skill, sw_shooter)
{
	if (!switch_poll_logical (SW_SHOOTER))
	{
		if (skill_shot_enabled)
		{
			sound_send (SND_SHOOTER_PULL);
			leff_restart (LEFF_STROBE_UP);
		}
	}
	else
	{
		leff_stop (LEFF_STROBE_UP);
	}
}

