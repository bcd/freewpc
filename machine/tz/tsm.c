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


U8 tsm_round_timer;

void tsm_round_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 5, "TOWN SQUARE MADNESS");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "SHOOT JETS");
		sprintf ("%d", tsm_round_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
}


void tsm_round_begin (void)
{
	deff_start (DEFF_TSM_ROUND);
	leff_start (LEFF_JETS_ACTIVE);
}

void tsm_round_expire (void)
{
	deff_stop (DEFF_TSM_ROUND);
	leff_stop (LEFF_JETS_ACTIVE);
}

void tsm_round_end (void)
{
}

void tsm_round_task (void)
{
	timed_mode_task (tsm_round_begin, tsm_round_expire, tsm_round_end,
		&tsm_round_timer, 20, 3);
}

CALLSET_ENTRY (tsm, display_update)
{
	if (timed_mode_timer_running_p (GID_TSM_ROUND_RUNNING,
		&tsm_round_timer))
		deff_start_bg (DEFF_TSM_ROUND, 0);
}

CALLSET_ENTRY (tsm, music_refresh)
{
	if (timed_mode_timer_running_p (GID_TSM_ROUND_RUNNING,
		&tsm_round_timer))
		music_request (MUS_TOWN_SQUARE_MADNESS, PRI_GAME_MODE1);
}

CALLSET_ENTRY (tsm, door_start_tsm)
{
	timed_mode_start (GID_TSM_ROUND_RUNNING, tsm_round_task);
}

CALLSET_ENTRY (tsm, end_ball)
{
	timed_mode_stop (&tsm_round_timer);
}

CALLSET_ENTRY (tsm, start_player)
{
}
