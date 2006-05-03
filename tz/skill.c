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
}

void disable_skill_shot (void)
{
	skill_shot_enabled = FALSE;
}

void award_skill_shot (void)
{
	mark_ball_in_play ();
	disable_skill_shot ();
	leff_restart (LEFF_FLASHER_HAPPY);
	sound_send (SND_SKILL_SHOT_CRASH_1);
	switch (skill_switch_reached)
	{
		case 1: score_add_current_const (SCORE_25K); break;
		case 2: score_add_current_const (SCORE_50K); break;
		case 3: score_add_current_const (SCORE_100K); break;
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
	timer_restart (GID_SLOT_DISABLED_BY_SKILL_SWITCH, TIME_3S, freerunning_timer_function);

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
	if (skill_shot_enabled)
	{
		award_skill_switch (1);
	}
}


void sw_center_skill_handler (void) __taskentry__
{
	if (skill_shot_enabled)
	{
		award_skill_switch (2);
	}
}


void sw_upper_skill_handler (void) __taskentry__
{
	if (skill_shot_enabled)
	{
		award_skill_switch (3);
	}
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


CALLSET_ENTRY (skill, end_game)
{
	disable_skill_shot ();
}

