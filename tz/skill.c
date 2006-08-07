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

void enable_skill_shot (void)
{
	skill_shot_enabled = TRUE;
	skill_switch_reached = 0;
	/* TODO : start display effect here to show skill shot ready */
}

void disable_skill_shot (void)
{
	skill_shot_enabled = FALSE;
}

void skill_shot_made_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_sched_transition (&trans_scroll_up);
	font_render_string_center (&font_fixed10, 64, 8, "SKILL SHOT");
	switch (skill_switch_reached)
	{
		case 1:
			sprintf ("RED  25,000");
			break;
		case 2:
			sprintf ("ORANGE  50,000");
			break;
		case 3:
			sprintf ("YELLOW  100,000");
			break;
	}
	font_render_string_center (&font_fixed10, 64, 23, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (1);
	deff_exit ();
}


void award_skill_shot (void)
{
	mark_ball_in_play ();
	disable_skill_shot ();
	deff_start (DEFF_SKILL_SHOT_MADE);
	leff_restart (LEFF_FLASHER_HAPPY);
	sound_send (SND_SKILL_SHOT_CRASH_1);
	door_award_flashing ();
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

void skill_switch_monitor (void) __taskentry__
{
	if (skill_switch_reached < 3)
		task_sleep_sec (1);
	else
		task_sleep_sec (3);
	award_skill_shot ();
	task_exit ();
}


void award_skill_switch (U8 sw)
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


void sw_lower_skill_handler (void) __taskentry__
{
	award_skill_switch (1);
}


void sw_center_skill_handler (void) __taskentry__
{
	award_skill_switch (2);
}


void sw_upper_skill_handler (void) __taskentry__
{
	award_skill_switch (3);
}


DECLARE_SWITCH_DRIVER (sw_lower_skill)
{
	.fn = sw_lower_skill_handler,
	.flags = SW_IN_GAME,
};


DECLARE_SWITCH_DRIVER (sw_center_skill)
{
	.fn = sw_center_skill_handler,
	.flags = SW_IN_GAME,
};


DECLARE_SWITCH_DRIVER (sw_upper_skill)
{
	.fn = sw_upper_skill_handler,
	.flags = SW_IN_GAME,
};


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

