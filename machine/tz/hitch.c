/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (hitch, __machine2__) */

#include <freewpc.h>

__local__ U8 hitch_count;
U8 hitch_round_timer;
extern void award_unlit_shot (U8 unlit_called_from);
extern __local__ U8 mpf_enable_count;

void hitchhiker_deff (void)
{
	/* Start a timer so jets won't stop animation */
	timer_restart_free (GID_HITCHHIKER, TIME_3S);
	U16 fno;
	for (fno = IMG_HITCHHIKER_START; fno <= IMG_HITCHHIKER_END; fno += 2)
	{
		dmd_alloc_pair_clean ();
		frame_draw (fno);
		/* text can only be printed to the low page so we flip them */
		dmd_flip_low_high ();
		
		if (timed_mode_timer_running_p (GID_HITCH_ROUND_RUNNING,
		&hitch_round_timer))
		{
			sprintf("10 MILLION");
			font_render_string_center (&font_mono5, 98, 5, sprintf_buffer);
		}
		else
		{
			sprintf ("HITCHERS");
			font_render_string_center (&font_mono5, 98, 5, sprintf_buffer);
			sprintf ("%d", hitch_count);
			font_render_string_center (&font_fixed6, 99, 24, sprintf_buffer);
		}	
		/* Flip back again */
		dmd_flip_low_high ();
		
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	
	
		//task_sleep_sec (1);
		task_sleep  (TIME_700MS);
	/* Stop the timer so jets.c can show deffs again */
	timer_kill_gid (GID_HITCHHIKER);
	deff_exit ();
	
}

void hitch_round_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 5, "SHOOT HITCHHIKER");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "FOR 10M");
		sprintf ("%d", hitch_round_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
}


void hitch_round_begin (void)
{
	deff_start (DEFF_HITCH_ROUND);
}

void hitch_round_expire (void)
{
	deff_stop (DEFF_HITCH_ROUND);
}

void hitch_round_end (void)
{
	deff_stop (DEFF_HITCH_ROUND);
}

void hitch_round_task (void)
{
	timed_mode_task (hitch_round_begin, hitch_round_expire, hitch_round_end,
		&hitch_round_timer, 20, 3);
}

CALLSET_ENTRY (hitch, display_update)
{
	if (timed_mode_timer_running_p (GID_HITCH_ROUND_RUNNING,
		&hitch_round_timer))
		deff_start_bg (DEFF_HITCH_ROUND, 0);
}

CALLSET_ENTRY (hitch, music_refresh)
{
	if (timed_mode_timer_running_p (GID_HITCH_ROUND_RUNNING,
		&hitch_round_timer))
		music_request (MUS_FASTLOCK_ADDAMS_FAMILY, PRI_GAME_MODE1);
}

CALLSET_ENTRY (hitch, sw_hitchhiker)
{
	if (timed_mode_timer_running_p (GID_HITCH_ROUND_RUNNING,
		&hitch_round_timer))
	{
		score (SC_10M);
		sound_send (SND_HITCHHIKER_COUNT);
	}
	else
	{
		score (SC_250K);
		sound_send (SND_HITCHHIKER_DRIVE_BY);
		award_unlit_shot (SW_HITCHHIKER);
	}
	bounded_increment (hitch_count, 99);
	
	/* Yes, I know it's ugly, I'll fix it at some point */
	if (hitch_count == 5 ||
		hitch_count == 10 ||
		hitch_count == 15 ||
		hitch_count == 20 ||
		hitch_count == 30 ||
		hitch_count == 40 ||
		hitch_count == 50 ||
		hitch_count == 60 ||
		hitch_count == 70 ||
		hitch_count == 80 ||
		hitch_count == 90 ||
		hitch_count == 99 )
	{
		mpf_enable_count++;
		sound_send (SND_ARE_YOU_READY_TO_BATTLE);
	}
	deff_start (DEFF_HITCHHIKER);
}

CALLSET_ENTRY (hitch, door_start_hitchhiker)
{
	timed_mode_start (GID_HITCH_ROUND_RUNNING, hitch_round_task);
}

CALLSET_ENTRY (hitch, start_ball)
{
	hitch_count = 1;
}

CALLSET_ENTRY (hitch, end_ball)
{
	timed_mode_stop (&hitch_round_timer);
}
