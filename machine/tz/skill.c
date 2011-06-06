/*
 * Copyright 2006, 2007, 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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

__local__ U8 skill_min_value;
/* Store skill_min for deff */
U8 skill_min_stored;

extern inline void flash_deff_begin_static (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
}

extern inline void flash_deff_begin_flashing (void)
{
	dmd_copy_low_to_high ();
}

extern inline __noreturn__ void flash_deff_run (void)
{
	dmd_show_low ();
	for (;;)
	{
		task_sleep (TIME_100MS * 3); /* all text on */
		dmd_show_other ();
		task_sleep (TIME_100MS * 1); /* flashing text on */
		dmd_show_other ();
	}
}

void skill_shot_ready_deff (void)
{
	flash_deff_begin_static ();

	sprintf ("PLAYER %d", player_up);
	font_render_string_left (&font_var5, 1, 2, sprintf_buffer);

	sprintf_current_score ();
	font_render_string_right (&font_mono5, 127, 2, sprintf_buffer);

	font_render_string (&font_mono5, 16, 10, "YELLOW");
	font_render_string (&font_mono5, 16, 16, "ORANGE");
	font_render_string (&font_mono5, 16, 22, "RED");

	flash_deff_begin_flashing ();

	sprintf ("%d,000,000", skill_min_value+3);
	font_render_string_right (&font_mono5, 110, 10, sprintf_buffer);
	sprintf ("%d,000,000", skill_min_value+1);
	font_render_string_right (&font_mono5, 110, 16, sprintf_buffer);
	sprintf ("%d,000,000", skill_min_value);
	font_render_string_right (&font_mono5, 110, 22, sprintf_buffer);

	flash_deff_run ();
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
	task_kill_gid (GID_SKILL_SWITCH_TRIGGER);
}

void skill_shot_made_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_sched_transition (&trans_scroll_up);
	font_render_string_center (&font_fixed10, 64, 8, "SKILL SHOT");
	switch (skill_switch_reached)
	{
		case 1:
			sprintf ("%d,000,000", skill_min_stored);
			break;
		case 2:
			sprintf ("%d,000,000", skill_min_stored+1);
			break;
		case 3:
			sprintf ("%d,000,000", skill_min_stored+3);
			break;
	}
	font_render_string_center (&font_times8, 64, 23, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (1);
	dmd_sched_transition (&trans_scroll_down_fast);
	deff_exit ();
}

/* Called from slot.c */
CALLSET_ENTRY (skill, skill_missed)
{
	set_valid_playfield ();
	disable_skill_shot ();
}

void award_skill_shot (void)
{
	set_valid_playfield ();
	disable_skill_shot ();
	leff_restart (LEFF_FLASHER_HAPPY);
	sound_send (SND_SKILL_SHOT_CRASH_1);
	switch (skill_switch_reached)
	{
		case 1:
		/* Inform sssmb.c that we hit a skill switch */
			callset_invoke (skill_red);
			score_1M (skill_min_value);
			skill_min_stored = skill_min_value;
			break;
		case 2: 
			callset_invoke (skill_orange);
			score_1M (skill_min_value+1);
			skill_min_stored = skill_min_value;
			skill_min_value++;
			timed_game_extend (5);
			break;
		case 3: 
			callset_invoke (skill_yellow);
			score_1M (skill_min_value+3);
			skill_min_stored = skill_min_value;
			skill_min_value += 2;
			timed_game_extend (10);
			break;
	}
	if (skill_min_value > 7)
		skill_min_value = 7;
	deff_start (DEFF_SKILL_SHOT_MADE);
}

/* Task that monitors the ball as it travels up and down the 
 * skill switch lane */
static void skill_switch_monitor (void)
{
	if (skill_switch_reached < 3)
		task_sleep_sec (1);
	else
	/* Wait longer so the ball can reach the slot switch */
		task_sleep_sec (2);
	award_skill_shot ();
	task_exit ();
}


static void award_skill_switch (U8 sw)
{
	/* Only trigger if skillshot or sssmb is enabled */
	if (!skill_shot_enabled && !task_find_gid (GID_SSSMB_JACKPOT_READY))
		return;
	callset_invoke (any_skill_switch);
	if (skill_switch_reached < sw)
	{
		skill_switch_reached = sw;
		task_recreate_gid (GID_SKILL_SWITCH_TRIGGER, skill_switch_monitor);
		sound_send (skill_switch_reached + SND_SKILL_SHOT_RED);
	}
	else if (task_kill_gid (GID_SKILL_SWITCH_TRIGGER))
	{
		/* Ball is now rolling back down */
		award_skill_shot ();
	}
}


CALLSET_ENTRY (skill, display_update)
{
	if (skill_shot_enabled)
		deff_start_bg (DEFF_SKILL_SHOT_READY, PRI_NULL);
}

CALLSET_ENTRY (skill, sw_skill_bottom)
{
	award_skill_switch (1);
}

CALLSET_ENTRY (skill, sw_rocket_kicker)
{
	award_skill_switch (1);
}

CALLSET_ENTRY (skill, sw_skill_center)
{
	award_skill_switch (2);
}


CALLSET_ENTRY (skill, sw_skill_top)
{
	device_switch_can_follow (skill_shot, slot, TIME_4S);
	award_skill_switch (3);
}


CALLSET_ENTRY (skill, start_player)
{
	/* This will also make sure that if the ball slips
	 * past the first skill switch, it will always count it properly
	 */
	skill_min_value = 1;
}


CALLSET_ENTRY (skill, start_ball)
{
	enable_skill_shot ();
}

CALLSET_ENTRY (skill, end_game)
{
	disable_skill_shot ();
}

CALLSET_ENTRY (skill, sw_shooter)
{
	/* Because the shooter switch is declared as an 'edge' switch,
	an event is generated on both transitions.  Check the current
	state of the switch to see which transition occurred. */
	if (!in_live_game)
		return;

	if (!switch_poll_logical (SW_SHOOTER))
	{
		if (skill_shot_enabled
			&& !timer_find_gid (GID_SHOOTER_SOUND_DEBOUNCE))
		{
			sound_send (SND_SHOOTER_PULL);
			leff_restart (LEFF_STROBE_UP);
			timer_restart_free (GID_SHOOTER_SOUND_DEBOUNCE, TIME_3S);
		}
	}
	else
	{
		leff_stop (LEFF_STROBE_UP);
	}
}

