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

#include <freewpc.h>

__local__ U8 hitch_count;
__local__ U8 hitch_level;
U8 hitch_round_timer;
extern void award_unlit_shot (U8 unlit_called_from);

/*void hitchhiker_deff (void)
{
	dmd_alloc_low ();
	frame_draw (IMG_HITCHER);
	if (timed_mode_timer_running_p (GID_HITCH_ROUND_RUNNING,
		&hitch_round_timer))
		sprintf("10 MILLION");
	else
	psprintf ("%d HITCHHIKER", "%d HITCHHIKERS", hitch_count);
	font_render_string_center (&font_fixed6, 78, 10, sprintf_buffer);
	dmd_sched_transition (&trans_scroll_left);
	dmd_show_low ();
	task_sleep (TIME_500MS);
	deff_exit ();
}*/

void hitchhiker_deff (void)
{
	//dmd_alloc_pair_clean ();
	U8 fno;	
	for (fno = IMG_HITCHHIKER_START; fno <= IMG_HITCHHIKER_END; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		/*if (timed_mode_timer_running_p (GID_HITCH_ROUND_RUNNING,
			&hitch_round_timer))
			sprintf("10 MILLION");
		else if (fno > 4)
			psprintf ("%d HITCHHIKER", "%d HITCHHIKERS", hitch_count);
		font_render_string (&font_var5, 50, 6+fno, sprintf_buffer);*/
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
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
	//if (lamp_test (LM_PANEL_HH))
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
	/*if (hitch_count == hitch_level)
	{
		SECTION_VOIDCALL (__machine__, mpf_ball_count);
		if (hitch_level == 2)
			hitch_level = 5;
		else
			hitch_level += 5;
	}*/
	/* Wait a bit before showing deff if hit from rocket */
	//if (event_did_follow (rocket, hitchhiker))
	//	task_sleep (TIME_200MS);
	deff_start (DEFF_HITCHHIKER);
}

CALLSET_ENTRY (hitch, door_start_hitchhiker)
{
	timed_mode_start (GID_HITCH_ROUND_RUNNING, hitch_round_task);
}

CALLSET_ENTRY (hitch, start_player)
{
	hitch_count = 1;
	/* hitch_level = 2;*/
}

CALLSET_ENTRY (hitch, end_ball)
{
	timed_mode_stop (&hitch_round_timer);
}
