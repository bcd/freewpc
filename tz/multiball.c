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

__local__ U8 mball_locks_lit;
__local__ U8 mball_locks_made;
__local__ U8 mballs_played;


void lock_is_lit_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "LOCK IS LIT");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void multiball_is_lit_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("BALL %d LOCKED", mball_locks_made);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	font_render_string_center (&font_mono5, 64, 18, "SHOOT PIANO TO");
	font_render_string_center (&font_mono5, 64, 27, "START MULTIBALL");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


void multiball_start_deff (void)
{
	kickout_lock (KLOCK_DEFF);
	sound_send (SND_DONT_TOUCH_THE_DOOR_AD_INF);
	printf ("MULTIBALL");
	dmd_alloc_low_clean ();
	dmd_show_low ();
	task_sleep_sec (1);
	flash_and_exit_deff (20, TIME_100MS);
}


void multiball_running_deff (void)
{
	for (;;)
	{
	}
}


void mball_lock_lamp_update (void)
{
	if (mball_locks_lit)
		lamp_tristate_flash (LM_LOCK_ARROW);
}

void mball_light_lock (void)
{
	if (mball_locks_lit < 2)
	{
		mball_locks_lit++;
		lamp_tristate_flash (LM_LOCK_ARROW);
		sound_send (SND_GUMBALL_COMBO);
		deff_start (DEFF_LOCK_LIT);
	}
}


void mball_check_light_lock (void)
{
	if (lamp_test (LM_GUM) && lamp_test (LM_BALL))
	{
		mball_light_lock ();
		timed_game_extend (15);
	}
}


CALLSET_ENTRY (mball, mball_start)
{
	if (!flag_test (FLAG_MULTIBALL_RUNNING))
	{
		flag_on (FLAG_MULTIBALL_RUNNING);
		deff_start (DEFF_MB_START);
		leff_start (LEFF_MB_RUNNING);
		// deff_start (DEFF_MB_RUNNING);
		callset_invoke (music_update);
		mball_locks_lit = 0;
		mball_locks_made = 0;
		mballs_played++;
		lamp_tristate_off (LM_LOCK_ARROW);
		lamp_off (LM_GUM);
		lamp_off (LM_BALL);
		device_request_empty (device_entry (DEVNO_LOCK));
	}
}


CALLSET_ENTRY (mball, mball_stop)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
		flag_off (FLAG_MULTIBALL_RUNNING);
		deff_stop (DEFF_MB_RUNNING);
		leff_stop (LEFF_MB_RUNNING);
		callset_invoke (music_update);
	}
}


CALLSET_ENTRY (mball, sw_left_ramp_exit)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING));
	else if (!lamp_test (LM_GUM))
	{
		lamp_on (LM_GUM);
		mball_check_light_lock ();
	}
}

CALLSET_ENTRY (mball, sw_right_ramp)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING));
	else if (!lamp_test (LM_BALL))
	{
		lamp_on (LM_BALL);
		mball_check_light_lock ();
	}
}


CALLSET_ENTRY (mball, piano)
{
	if (lamp_flash_test (LM_PIANO_JACKPOT))
	{
		lamp_tristate_off (LM_PIANO_JACKPOT);
		callset_invoke (mball_start);
	}
}


CALLSET_ENTRY (mball, any_pf_switch)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
		score (SC_20K);
	}
}


CALLSET_ENTRY (mball, single_ball_play)
{
	callset_invoke (mball_stop);
}


CALLSET_ENTRY (mball, lock)
{
	if (mball_locks_lit > 0)
	{
		mball_locks_lit--;
		device_lock_ball (device_entry (DEVNO_LOCK));
		enable_skill_shot ();
		sound_send (SND_FAST_LOCK_STARTED);
		if (mball_locks_lit == 0)
		{
			lamp_off (LM_GUM);
			lamp_off (LM_BALL);
			lamp_tristate_off (LM_LOCK_ARROW);
		}
		mball_locks_made++;
		lamp_on (LM_LOCK1);
		if (mball_locks_made == 2)
			lamp_on (LM_LOCK2);
		lamp_tristate_flash (LM_PIANO_JACKPOT);
		deff_start (DEFF_MB_LIT);
	}
}

CALLSET_ENTRY (mball, start_player)
{
	lamp_off (LM_GUM);
	lamp_off (LM_BALL);
	lamp_off (LM_LOCK1);
	lamp_off (LM_LOCK2);
	lamp_tristate_off (LM_LOCK_ARROW);
	lamp_tristate_off (LM_PIANO_JACKPOT);
	mball_locks_lit = 0;
	mball_locks_made = 0;
	mballs_played = 0;
	flag_off (FLAG_MULTIBALL_RUNNING);
}

