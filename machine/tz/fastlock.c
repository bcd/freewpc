/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
//TODO What happens with multimode and multiball???? pause timer. 
#include <freewpc.h>
#include <queue.h>


U8 fastlock_round_timer;
U8 fastlock_award;
__local__ U8 fastlocks_collected;

extern U8 loop_time;

void fastlock_round_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 5, "SHOOT FAST LOOPS");
		sprintf("%d MILLION", fastlock_award);
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "HIT LOCK TO COLLECT");
		sprintf ("%d", fastlock_round_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
}

void fastlock_award_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 5, "JACKPOT");
	sprintf("%d MILLION", fastlock_award);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void fastlock_round_begin (void)
{
	deff_start (DEFF_FASTLOCK_ROUND);
	fastlock_award = 5;
	fastlocks_collected = 0;
}

void fastlock_round_expire (void)
{
	deff_stop (DEFF_FASTLOCK_ROUND);
}

void fastlock_round_end (void)
{
	deff_stop (DEFF_FASTLOCK_ROUND);
	lamp_off (LM_LOCK_ARROW);
}

void fastlock_round_task (void)
{
	timed_mode_task (fastlock_round_begin, fastlock_round_expire, fastlock_round_end,
		&fastlock_round_timer, 40, 3);
}

bool fastlock_running (void)
{
	if (timed_mode_timer_running_p (GID_FASTLOCK_ROUND_RUNNING,
		&fastlock_round_timer))
		return TRUE;
	else
		return FALSE;
}

CALLSET_ENTRY (fastlock, dev_lock_enter)
{
	if (fastlock_running ())
	{
		deff_start (DEFF_FASTLOCK_AWARD);
		score_multiple (SC_1M, fastlock_award);
		fastlocks_collected++;
		fastlock_award = (fastlocks_collected * 5);
		fastlock_round_timer =+ 10;
	}
}

void fastlock_right_loop_completed (void)
{
	if (!fastlock_running ())
		return;

	if (loop_time < 30)
	{
		fastlock_award += 10;
		sound_send (SND_CRASH);
	}
	else if (loop_time < 100)
	{
		fastlock_award += 5;
		sound_send (SND_FIVE);
	}
	else
		fastlock_award += 1;
		sound_send (SND_ONE);
}

CALLSET_ENTRY (fastlock, lamp_update)
{
	if (fastlock_running ())
		lamp_tristate_flash (LM_LOCK_ARROW);
}
CALLSET_ENTRY (fastlock, display_update)
{
	if (fastlock_running ())
		deff_start_bg (DEFF_FASTLOCK_ROUND, 0);
}

CALLSET_ENTRY (fastlock, music_refresh)
{
	if (fastlock_running ())
		music_request (MUS_FASTLOCK_ADDAMS_FAMILY, PRI_GAME_MODE1);
}

CALLSET_ENTRY (fastlock, door_start_fast_lock)
{
	timed_mode_start (GID_FASTLOCK_ROUND_RUNNING, fastlock_round_task);
}

CALLSET_ENTRY (fastlock, end_ball)
{
	timed_mode_stop (&fastlock_round_timer);
}

CALLSET_ENTRY (fastlock, start_player)
{
}
