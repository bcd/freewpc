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

#include <freewpc.h>


U8 spiral_round_timer;
//__local__ score_t spiral_round_score;

extern U8 spiral_loops;

void spiral_loop_deff (void)
{

	dmd_alloc_low_clean ();
	psprintf ("1 SPIRAL", "%d SPIRALS", spiral_loops);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	sprintf_score (score_deff_get());	
	font_render_string_center (&font_fixed6, 64, 18, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


void award_spiral_loop (void)
{
	if (spiral_loops < 3)
	{
		sound_send (SND_SPIRAL_AWARDED);
		score (SC_10M);
		//score_add (SC_10M, spiral_round_score);
		deff_start (DEFF_SPIRAL_LOOP);
	}
	else
	{
		sound_send (SND_SPIRAL_BREAKTHRU);
		score (SC_20M);
		//score_add (SC_20M, spiral_round_score);
		deff_start (DEFF_SPIRAL_LOOP);
		spiral_loops = 0;
	}
}

void spiral_round_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed6, 64, 5, "SPIRAL");
		sprintf_current_score ();
		//sprintf_score (spiral_round_score);
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "SHOOT LOOPS");
		sprintf ("%d", spiral_round_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
}


void spiral_round_begin (void)
{
	deff_start (DEFF_SPIRAL_ROUND);
	//score_zero (spiral_round_score);
}

void spiral_round_expire (void)
{
	deff_stop (DEFF_SPIRAL_ROUND);
	lamp_off (LM_RIGHT_SPIRAL);
	lamp_off (LM_LEFT_SPIRAL);
}

void spiral_round_end (void)
{
	deff_stop (DEFF_SPIRAL_ROUND);
	lamp_tristate_off (LM_RIGHT_SPIRAL);
	lamp_tristate_off (LM_LEFT_SPIRAL);
}

void spiral_round_task (void)
{
	timed_mode_task (spiral_round_begin, spiral_round_expire, spiral_round_end,
		&spiral_round_timer, 20, 3);
}

bool spiralround_running (void)
{
	if (timed_mode_timer_running_p (GID_SPIRAL_ROUND_RUNNING,
		&spiral_round_timer))
		return TRUE;
	else
		return FALSE;
}

CALLSET_ENTRY (spiral, display_update)
{
	if (spiralround_running ())
		deff_start_bg (DEFF_SPIRAL_ROUND, 0);
}

CALLSET_ENTRY (spiral, lamp_update)
{
	if (spiralround_running ())
	{
		lamp_tristate_flash (LM_RIGHT_SPIRAL);
		lamp_tristate_flash (LM_LEFT_SPIRAL);
	}
}
CALLSET_ENTRY (spiral, music_refresh)
{
	if (spiralround_running ())
		music_request (MUS_SPIRAL_ROUND, PRI_GAME_MODE1);
}

CALLSET_ENTRY (spiral, door_start_spiral)
{
	timed_mode_start (GID_SPIRAL_ROUND_RUNNING, spiral_round_task);
}

CALLSET_ENTRY (spiral, end_ball)
{
	timed_mode_stop (&spiral_round_timer);
}

CALLSET_ENTRY (spiral, start_player)
{
}
