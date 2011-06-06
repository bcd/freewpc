/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (music , __machine2__) */
#include <freewpc.h>

U8 old_mode_timer;

void slow_music_lin (U8 count, U8 delay)
{
	U8 i;
	for (i = 0; i < count; i++)
	{
		sound_send (SND_MUSIC_SLOWER);
		task_sleep (delay);
	}
}

void slow_music_log (U8 count, U8 delay)
{
	U8 i;
	for (i = 0; i < count; i++)
	{
		sound_send (SND_MUSIC_SLOWER);
		task_sleep (delay + i);
	}
}

void speed_up_music_lin (U8 count, U8 delay)
{
	U8 i;
	if (count > 10)
		count = 10;
	for (i = 0; i < count; i++)
	{
		sound_send (SND_MUSIC_FASTER);
		task_sleep (delay);
	}
}

void slow_down_music_task (void)
{
	task_sleep_sec (6);
	slow_music_lin (20, TIME_1S);
	task_exit ();
}

void speed_up_music_task (void)
{
	speed_up_music_lin (10, TIME_500MS);
	task_exit ();
}

void music_check_mode_timer (U8 mode_timer)
{
	if (old_mode_timer == mode_timer)
		return;
	old_mode_timer = mode_timer;
	if (mode_timer <= 10 && mode_timer > 5)
	{
		sound_send (SND_MUSIC_FASTER);
	}
}

CALLSET_ENTRY (music, idle_every_second)
{
	extern U8 mpf_timer;
	extern U8 hurryup_mode_timer;
	extern U8 greed_mode_timer;
	extern U8 tsm_mode_timer;
	extern U8 fastlock_mode_timer;
	extern U8 hitch_mode_timer;
	extern U8 sslot_mode_timer;
	extern U8 spiral_mode_timer;
	extern sound_code_t music_active;

	if (!in_live_game || task_find_gid (GID_MUSIC_SPEED) || live_balls != 1)
		return;
	switch (music_active)
	{
		case MUS_POWERFIELD:
			music_check_mode_timer (mpf_timer);
			break;
		case MUS_FASTLOCK_COUNTDOWN:
			music_check_mode_timer (hurryup_mode_timer);
			break;
		case MUS_GREED_MODE:
			music_check_mode_timer (greed_mode_timer);
			break;
		case MUS_TOWN_SQUARE_MADNESS:
			music_check_mode_timer (tsm_mode_timer);
			break;
		case MUS_FASTLOCK_ADDAMS_FAMILY:
			music_check_mode_timer (fastlock_mode_timer);
			music_check_mode_timer (hitch_mode_timer);
			break;
		case MUS_SUPER_SLOT:
			music_check_mode_timer (sslot_mode_timer);
			break;
		case MUS_SPIRAL_MODE:
			music_check_mode_timer (spiral_mode_timer);
			break;
	}
}

CALLSET_ENTRY (music, init)
{
	old_mode_timer = 0;
}

CALLSET_ENTRY (music, bonus, stop_game, start_game)
{
	task_kill_gid (GID_MUSIC_SPEED);
	old_mode_timer = 0;
}

CALLSET_ENTRY (music, speed_up_music)
{
	task_create_gid (GID_MUSIC_SPEED, speed_up_music_task);
}

CALLSET_ENTRY (music, end_game)
{
	callset_invoke (slow_down_music);
}

CALLSET_ENTRY (music, slow_down_music)
{
	task_create_gid (GID_MUSIC_SPEED, slow_down_music_task);
}
