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

score_t frenzy_total;

U8 frenzy_timer;

void frenzy_running_deff (void)
{
	for (;;)
	{
		seg_alloc_clean ();
		if (flag_test (FLAG_SUPER_FRENZY_RUNNING))
			seg_write_string (1, 0, "SUPER FRENZY");
		else
			seg_write_string (1, 0, "FRENZY");
		sprintf ("%d SEC.", frenzy_timer);
		seg_write_row_right (1, sprintf_buffer);
		sprintf_score (frenzy_total);
		seg_write_string (0, 0, sprintf_buffer);
		seg_show ();
		task_sleep (TIME_166MS);
	}
	deff_exit ();
}


void frenzy_running (void)
{
	while (frenzy_timer > 0)
	{
		timer_pause_second ();
		frenzy_timer--;
	}
	task_sleep_sec (1);
	flag_off (FLAG_SUPER_FRENZY_RUNNING);
	effect_update_request ();
	task_exit ();
}

bool frenzy_qualified_p (void)
{
	return flag_test (FLAG_FRENZY_LIT);
}

bool frenzy_masked_p (void)
{
	return multiball_mode_running_p ();
}

bool frenzy_lit_p (void)
{
	return frenzy_qualified_p () && !frenzy_masked_p ();
}

bool frenzy_running_p (void)
{
	return (frenzy_timer > 0);
}


/**
 * Start / add 20 seconds of Frenzy time.
 */
void frenzy_add_time (void)
{
	if (frenzy_timer == 0)
	{
		task_create_gid1 (GID_FRENZY_RUNNING, frenzy_running);
	}
	frenzy_timer += 20;
	if (frenzy_timer > 99)
		frenzy_timer = 99;
}

void frenzy_start (void)
{
	score (SC_100K);
	frenzy_add_time ();
}


void super_frenzy_start (void)
{
	score (SC_1M);
	flag_on (FLAG_SUPER_FRENZY_RUNNING);
	frenzy_add_time ();
}


CALLSET_ENTRY (frenzy, display_update)
{
	if (frenzy_timer > 0)
	{
		deff_start_bg (DEFF_FRENZY_RUNNING, 0);
	}
}


CALLSET_ENTRY (frenzy, music_refresh)
{
	if (frenzy_timer > 0)
		music_request (MUS_FRENZY, PRI_GAME_MODE5);
}


CALLSET_ENTRY (frenzy, lamp_update)
{
	lamp_flash_if (LM_FRENZY, frenzy_lit_p ());
}


CALLSET_ENTRY (frenzy, any_pf_switch)
{
	if (frenzy_timer > 0)
	{
		if (flag_test (FLAG_SUPER_FRENZY_RUNNING))
		{
			score_add (frenzy_total, score_table[SC_100K]);
		}
		else
		{
			score_add (frenzy_total, score_table[SC_50K]);
		}
	}
}


CALLSET_ENTRY (frenzy, sw_trap_door)
{
	if (frenzy_lit_p ())
	{
		flag_off (FLAG_FRENZY_LIT);
		frenzy_start ();
	}
}


CALLSET_ENTRY (frenzy, start_ball)
{
	frenzy_timer = 0;
	score_zero (frenzy_total);
	flag_off (FLAG_SUPER_FRENZY_RUNNING);
}

CALLSET_ENTRY (frenzy, end_ball)
{
	frenzy_timer = 0;
}

