/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

void quickmb_running_deff (void)
{
	for (;;)
	{
		seg_alloc_clean ();
		seg_write_row_center (0, "HIT RUDY");
		sprintf_current_score ();
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep (TIME_166MS);
	}
	deff_exit ();
}

void quickmb_started_deff (void)
{
	task_sleep_sec (2);
	seg_alloc_clean ();
	seg_write_row_center (0, "UNLIMITED");
	seg_write_row_center (1, "MILLION IS LIT");
	seg_copy_low_to_high ();
	seg_erase ();
	deff_swap_low_high (8, TIME_200MS);
	deff_exit ();
}

void quickmb_score_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "MILLION");
	seg_write_row_center (1, "1,000,000");
	seg_show ();
	music_effect_start (SND_MILLION_AWARD, SL_3S);
	task_sleep_sec (3);
	deff_exit ();
}

bool quickmb_running_p (void)
{
	return flag_test (FLAG_QUICK_MB_RUNNING);
}

void quickmb_start (void)
{
	if (!flag_test (FLAG_QUICK_MB_RUNNING))
	{
		flag_on (FLAG_QUICK_MB_RUNNING);
		deff_start (DEFF_QUICKMB_STARTED);
		device_multiball_set (2);
	}
}

void quickmb_stop (void)
{
	if (flag_test (FLAG_QUICK_MB_RUNNING))
	{
		flag_off (FLAG_QUICK_MB_RUNNING);
		effect_update_request ();
	}
}


CALLSET_BOOL_ENTRY (quickmb, dev_tunnel_kick_request)
{
	/* Do not allow the tunnel to kick out while there is
	a ball at the plunger */
	if (in_live_game && flag_test (FLAG_QUICK_MB_RUNNING) &&
		 global_flag_test (GLOBAL_FLAG_BALL_AT_PLUNGER))
		return FALSE;
	return TRUE;
}

CALLSET_ENTRY (quickmb, display_update)
{
	if (quickmb_running_p ())
		deff_start_bg (DEFF_QUICKMB_RUNNING, 0);
}

CALLSET_ENTRY (quickmb, music_refresh)
{
	if (quickmb_running_p ())
		music_request (MUS_HIGH_SCORE, PRI_GAME_MODE4);
}

CALLSET_ENTRY (quickmb, rudy_shot)
{
	if (quickmb_running_p ())
	{
		score (SC_1M);
		deff_start (DEFF_QUICKMB_SCORE);
	}
}

CALLSET_ENTRY (quickmb, start_ball, single_ball_play)
{
	quickmb_stop ();
}

