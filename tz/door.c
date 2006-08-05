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

/** Index of the panel which is currently slow flashing (next to
 * be awarded) or fast flashing (running) */
__local__ U8 door_index;

/** Number of door panels that have been started */
__local__ U8 door_panels_started;

/** Number of door panels that have been completed */
__local__ U8 door_panels_completed;

U8 door_active_lamp;


/** Names of all the door panels, in order */
const char *door_panel_names[] = {
	"TOWN SQUARE MADNESS",
	"LIGHT EXTRA BALL",
	"SUPER SLOT",
	"CLOCK MILLIONS",
	"THE SPIRAL",
	"BATTLE THE POWER",
	"10 MILLION",
	"GREED",
	"THE CAMERA",
	"THE HITCHHIKER",
	"CLOCK CHAOS",
	"SUPER SKILL SHOT",
	"FAST LOCK",
	"LIGHT GUMBALL",
	"RETURN TO THE ZONE",
};

const char *door_award_goals[] = {
	"JET BUMPERS",
	"LOCK LANE",
	"SLOT MACHINE",
	"CLOCK TARGET",
	"THE LOOPS",
	"RIGHT RAMP",
	"ANYTHING",
	"STANDUPS",
	"THE LEFT HOLE",
	"THE SIDE LANE",
	"CLOCK TARGET",
	"LEFT RAMP",
	"LOCK LANE",
	"RIGHT LOOP",
	"PIANO!",
};

extern inline const U8 *door_get_lamps (void)
{
	return lampset_lookup (LAMPSET_DOOR_PANELS);
}


extern inline U8 door_get_flashing_lamp (void)
{
	return ((door_get_lamps ())[door_index]);
}


void door_set_flashing (U8 id)
{
	const U8 *door_lamps = door_get_lamps ();
	lamp_flash_off (door_lamps[door_index]);
	door_index = id;
	lamp_flash_on (door_lamps[door_index]);
}


void door_advance_flashing (void)
{
	const U8 *door_lamps = door_get_lamps ();
	U8 new_door_index;

	if (door_panels_started < 14)
	{
		new_door_index = door_index;
		do {
			new_door_index++;
			if (new_door_index >= 14)
				new_door_index = 0;
		} while (lamp_test (door_lamps[new_door_index]));
	}
	else
		new_door_index = 14;

	door_set_flashing (new_door_index);
}

void door_award_rotate (void) __taskentry__
{
	task_sleep_sec (2);
	while (in_live_game)
	{
		door_advance_flashing ();
		task_sleep_sec (1);
	}
	task_exit ();
}


void door_award_deff (void)
{
	dmd_alloc_low_clean ();
	printf ("%s", door_panel_names[door_index]);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	printf (
		(door_panels_started != 1) ? "%d DOOR PANELS" : "%d DOOR PANEL", 
		door_panels_started);
	font_render_string_center (&font_term6, 64, 21, sprintf_buffer);
	kickout_lock (KLOCK_DEFF);
	dmd_show_low ();
	sound_send (SND_NEXT_CAMERA_AWARD_SHOWN);
	task_sleep_sec (2);
	kickout_unlock (KLOCK_DEFF);
	task_sleep_sec (1);
	deff_exit ();
}


void door_award_enable (void)
{
	lamp_tristate_flash (LM_SLOT_MACHINE);
	lamp_tristate_flash (LM_PIANO_PANEL);
	task_recreate_gid (GID_DOOR_AWARD_ROTATE, door_award_rotate);
}


void door_award_flashing (void)
{
	task_kill_gid (GID_DOOR_AWARD_ROTATE);
	door_active_lamp = door_get_flashing_lamp ();
	lamp_on (door_active_lamp);
	timed_game_extend (10);
	door_panels_started++;
	deff_start (DEFF_DOOR_AWARD);
	task_sleep (TIME_100MS);
	door_advance_flashing ();
	score (SC_50K);
	door_award_enable ();
	callset_invoke (door_panel_awarded);
}


CALLSET_ENTRY(door, start_player)
{
	door_index = 0;
	door_panels_started = 0;
	door_panels_completed = 0;
}

CALLSET_ENTRY(door, start_ball)
{
	door_set_flashing (door_index);
	door_award_enable ();
}

