/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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
#include <status.h>

__local__ U8 mball_locks_lit;
__local__ U8 mball_locks_made;
__local__ U8 mballs_played;

void lock_lit_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "LOCK IS LIT");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void mb_lit_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("BALL %d LOCKED", mball_locks_made);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	font_render_string_center (&font_mono5, 64, 20, "SHOOT LEFT RAMP");
	font_render_string_center (&font_mono5, 64, 26, "FOR MULTIBALL");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}


void mb_start_deff (void)
{
	kickout_lock (KLOCK_DEFF);
	sound_send (SND_DONT_TOUCH_THE_DOOR_AD_INF);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "MULTIBALL");
	dmd_show_low ();
	flash_and_exit_deff (50, TIME_100MS);
}


void mb_running_deff (void)
{
	for (;;)
	{
		score_update_start ();
		dmd_alloc_pair ();
		dmd_clean_page_low ();
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "SHOOT PIANO FOR JACKPOT");
		dmd_copy_low_to_high ();
		font_render_string_center (&font_fixed6, 64, 4, "MULTIBALL");
		dmd_show_low ();
		do
		{
			task_sleep (TIME_133MS);
			dmd_show_other ();
			task_sleep (TIME_133MS);
			dmd_show_other ();
		} while (!score_update_required ());
	}
}


CALLSET_ENTRY (mball, lamp_update)
{
	if (mball_locks_lit && !flag_test (FLAG_SSSMB_RUNNING))
		lamp_tristate_flash (LM_LOCK_ARROW);
	else
		lamp_tristate_off (LM_LOCK_ARROW);

	if (mball_locks_made == 0)
	{
		lamp_off (LM_LOCK1);
		lamp_off (LM_LOCK2);
	}
	else if (mball_locks_made == 1)
	{
		lamp_on (LM_LOCK1);
		lamp_off (LM_LOCK2);
	}
	else
	{
		lamp_on (LM_LOCK1);
		lamp_on (LM_LOCK2);
	}

	if (flag_test (FLAG_MB_JACKPOT_LIT))
		lamp_tristate_flash (LM_PIANO_JACKPOT);
	else
		lamp_tristate_off (LM_PIANO_JACKPOT);
}


CALLSET_ENTRY (mball, display_update)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
		deff_start_bg (DEFF_MB_RUNNING, 0);
}


void mball_light_lock (void)
{
	if (mball_locks_lit < 2)
	{
		mball_locks_lit++;
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


CALLSET_ENTRY (mball, music_refresh)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
		music_request (MUS_MULTIBALL, PRI_GAME_MODE1 + 12);
};


CALLSET_ENTRY (mball, mball_start)
{
	if (!flag_test (FLAG_MULTIBALL_RUNNING))
	{
		flag_on (FLAG_MULTIBALL_RUNNING);
		flag_on (FLAG_MB_JACKPOT_LIT);
		music_refresh ();
		deff_start (DEFF_MB_START);
		leff_start (LEFF_MB_RUNNING);
		mball_locks_lit = 0;
		mball_locks_made = 0;
		mballs_played++;
		lamp_off (LM_GUM);
		lamp_off (LM_BALL);
		device_request_empty (device_entry (DEVNO_LOCK));
		ballsave_add_time (10);
	}
}


CALLSET_ENTRY (mball, mball_stop)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
		flag_off (FLAG_MULTIBALL_RUNNING);
		flag_off (FLAG_MB_JACKPOT_LIT);
		deff_stop (DEFF_MB_START);
		deff_stop (DEFF_MB_RUNNING);
		leff_stop (LEFF_MB_RUNNING);
		music_refresh ();
	}
}


CALLSET_ENTRY (mball, sw_left_ramp_exit)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING));
	else if (flag_test (FLAG_SSSMB_RUNNING));
	else if (flag_test (FLAG_CHAOSMB_RUNNING));
	else if (lamp_flash_test (LM_MULTIBALL))
	{
		lamp_tristate_off (LM_MULTIBALL);
		callset_invoke (mball_start);
	}
	else if (!lamp_test (LM_GUM))
	{
		lamp_on (LM_GUM);
		mball_check_light_lock ();
	}
}

/* TODO - on a missed left ramp exit switch, use the
 * autoplunger switch to start multiball instead */

CALLSET_ENTRY (mball, sw_right_ramp)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING));
	else if (!lamp_test (LM_BALL))
	{
		lamp_on (LM_BALL);
		mball_check_light_lock ();
	}
}


CALLSET_ENTRY (mball, sw_piano)
{
	if (flag_test (FLAG_MB_JACKPOT_LIT))
	{
		flag_off (FLAG_MB_JACKPOT_LIT);
		deff_start (DEFF_JACKPOT);
		score (SC_20M);
		/* TODO : there is no relight jackpot rule */
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


CALLSET_ENTRY (mball, dev_lock_enter)
{
	if (flag_test (FLAG_SSSMB_RUNNING));
	else if (mball_locks_lit > 0)
	{
		mball_locks_lit--;
		device_lock_ball (device_entry (DEVNO_LOCK));
		enable_skill_shot ();
		sound_send (SND_FAST_LOCK_STARTED);
		if (mball_locks_lit == 0)
		{
			lamp_off (LM_GUM);
			lamp_off (LM_BALL);
		}
		mball_locks_made++;
		lamp_tristate_flash (LM_MULTIBALL);
		deff_start (DEFF_MB_LIT);
	}
}

CALLSET_ENTRY (mball, start_player)
{
	lamp_off (LM_GUM);
	lamp_off (LM_BALL);
	lamp_tristate_off (LM_MULTIBALL);
	mball_locks_lit = 0;
	mball_locks_made = 0;
	mballs_played = 0;
	flag_off (FLAG_MULTIBALL_RUNNING);
	flag_off (FLAG_MB_JACKPOT_LIT);
}


CALLSET_ENTRY (mball, status_report)
{
	status_page_init ();
	sprintf ("%d LOCKS LIT", mball_locks_lit);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	sprintf ("%d BALLS LOCKED", mball_locks_made);
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	status_page_complete ();
}

