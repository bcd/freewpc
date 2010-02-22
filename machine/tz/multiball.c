/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
/* Used to start a mode if multiball ends without picking up a jackpot */
bool mball_jackpot_uncollected;
bool mball_restart_collected;
U8 mball_restart_timer;

extern U8 live_balls;
extern U8 gumball_enable_count;
extern U8 autofire_request_count;
extern bool fastlock_running (void);
extern void reset_unlit_shots (void);
extern void award_unlit_shot (U8 unlit_called_from);

void mball_restart_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 16, "SHOOT LOCK TO RESTART");
		font_render_string_center (&font_fixed6, 64, 4, "MULTIBALL");
		sprintf ("%d", mball_restart_timer);
		font_render_string_center (&font_fixed6, 64, 24, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
}


void mball_restart_begin (void)
{
	deff_start (DEFF_MBALL_RESTART);
}

void mball_restart_expire (void)
{
	deff_stop (DEFF_MBALL_RESTART);
}

void mball_restart_end (void)
{
}

void mball_restart_task (void)
{
	timed_mode_task (mball_restart_begin, mball_restart_expire, 
		mball_restart_end, &mball_restart_timer, 30, 3);
}

CALLSET_ENTRY (mball, display_update)
{
	if (timed_mode_timer_running_p (GID_MBALL_RESTART_RUNNING,
		&mball_restart_timer))
		deff_start_bg (DEFF_MBALL_RESTART, 0);
	else if (flag_test (FLAG_MULTIBALL_RUNNING))
		deff_start_bg (DEFF_MB_RUNNING, 0);
}

CALLSET_ENTRY (mball, music_refresh)
{
	if (timed_mode_timer_running_p (GID_MBALL_RESTART_RUNNING,
		&mball_restart_timer))
		music_request (MUS_FASTLOCK_COUNTDOWN, PRI_GAME_MODE1);
	else if (flag_test (FLAG_MULTIBALL_RUNNING))
		music_request (MUS_MULTIBALL, PRI_GAME_MODE1 + 12);
}


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


void jackpot_relit_deff (void)
{
	sample_start (SND_GET_THE_EXTRA_BALL, SL_1S);
	sound_send (SND_JACKPOT);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "JACKPOT RELIT");
	dmd_show_low ();
	flash_and_exit_deff (50, TIME_100MS);
}

void mb_running_deff (void)
{
	for (;;)
	{
		score_update_start ();
		//dmd_alloc_low_high ();
		
		dmd_alloc_pair ();
		dmd_clean_page_low ();
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		if (flag_test (FLAG_MB_JACKPOT_LIT))
		{
			font_render_string_center (&font_var5, 64, 27, "SHOOT PIANO FOR JACKPOT");
		}
		else
		{
			font_render_string_center (&font_var5, 64, 27, "SHOOT LOCK TO RELIGHT");
		}
		dmd_copy_low_to_high ();
		font_render_string_center (&font_fixed6, 64, 4, "MULTIBALL");
		dmd_show_low ();
		while (!score_update_required ())
		{
			task_sleep (TIME_133MS);
			dmd_show_other ();
		}
	}
}

/* Check to see if we can light the lock/lock a ball */
bool can_lock_ball (void)
{	
	if ((mball_locks_lit > 0) 
		&& !flag_test (FLAG_MULTIBALL_RUNNING) 
		&& !flag_test (FLAG_SSSMB_RUNNING) 
		&& !flag_test (FLAG_CHAOSMB_RUNNING)
		&& !multi_ball_play ()
		&& !flag_test (FLAG_POWERBALL_IN_PLAY))
		return TRUE;
	else
		return FALSE;
}


bool can_light_lock (void)
{
	//if (!multi_ball_play ()	&& mball_locks_lit)
	if (can_lock_ball ())
		return TRUE;
	else if (fastlock_running ())
		return TRUE;
	else if (flag_test (FLAG_MULTIBALL_RUNNING) && !flag_test (FLAG_MB_JACKPOT_LIT))
		return TRUE;
	else if (timed_mode_timer_running_p (GID_MBALL_RESTART_RUNNING, &mball_restart_timer))
		return TRUE;
	else
		return FALSE;
}

/* Rules to say whether we can start multiball */
bool multiball_ready (void)
{
	/* Don't allow during certain conditions */
	if (flag_test (FLAG_MULTIBALL_RUNNING) 
		|| flag_test (FLAG_SSSMB_RUNNING)
		|| flag_test (FLAG_CHAOSMB_RUNNING)
		|| multi_ball_play ())
		return FALSE;
	/* Require one locked ball first multiball, 2 locks after */
	else if ((mball_locks_made > 0) && (mballs_played == 0))
		return TRUE;
	else if ((mball_locks_made > 1) && (mballs_played > 0))
		return TRUE;
	else
		return FALSE;

}

CALLSET_ENTRY (mball, lamp_update)
{
	/* Light the lock if it can be collected */
	if (can_light_lock ())
		lamp_tristate_flash (LM_LOCK_ARROW);
	else	
		lamp_tristate_off (LM_LOCK_ARROW);

	/* Flash the appropiate lamp when multiball is ready */
	if (multiball_ready ())
		lamp_tristate_flash (LM_MULTIBALL);

	/* Turn on and flash door lock lamps during game situations */
	if (mball_locks_made == 0 && mball_locks_lit == 0)
	{
		lamp_tristate_off (LM_LOCK1);
		lamp_tristate_off (LM_LOCK2);
	}
	else if (mball_locks_made == 0 && mball_locks_lit == 1)
	{
		lamp_tristate_flash (LM_LOCK1);
		lamp_tristate_off (LM_LOCK2);
	}
	else if (mball_locks_made == 0 && mball_locks_lit == 2)
	{
		lamp_tristate_flash (LM_LOCK1);
		lamp_tristate_flash (LM_LOCK2);

	}
	else if (mball_locks_made == 1 && mball_locks_lit == 1)
	{
		lamp_tristate_on (LM_LOCK1);
		lamp_tristate_off (LM_LOCK2);
	}
	else if (mball_locks_made == 1 && mball_locks_lit == 2)
	{
		lamp_tristate_on (LM_LOCK1);
		lamp_tristate_flash (LM_LOCK2);
	}
	else
	{
		lamp_tristate_on (LM_LOCK1);
		lamp_tristate_on (LM_LOCK2);
	}
	
	/* Flash the Piano Jackpot lit when Jackpot is lit */
	if (flag_test (FLAG_MB_JACKPOT_LIT))
		lamp_tristate_flash (LM_PIANO_JACKPOT);
	else
		lamp_tristate_off (LM_PIANO_JACKPOT);
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

/* Check to see if GUMBALL has been completed and light lock */
void mball_check_light_lock (void)
{
	if (lamp_test (LM_GUM) && lamp_test (LM_BALL))
	{
		mball_light_lock ();
		gumball_enable_count++;
		//timed_game_extend (15);
	}
}

 /* How we want the balls released and in what order */
void mball_start_3_ball (void)
{
	/* Don't start if another multiball is running */
//	if (multi_ball_play ())
//		return;
	if (live_balls == 3)
		return;
	/* Check lock and empty accordingly */
	switch (device_recount (device_entry (DEVNO_LOCK)))
	{	
		/* No balls in lock, fire 2 from trough */
		case 0:
			autofire_add_ball ();	
			autofire_add_ball ();	
			break;
		/* 1 ball in lock, fire 2 from trough 
		 *  1 ball may already be in autofire */
		case 1:
			autofire_add_ball ();	
		 	task_sleep_sec (3);
			task_sleep (TIME_500MS);
			device_unlock_ball (device_entry (DEVNO_LOCK));
		 	//task_sleep_sec (1);
			break;
		/* 2 balls in lock, fire 1 from trough */
		case 2:
			device_unlock_ball (device_entry (DEVNO_LOCK));
			task_sleep_sec (1);	
			device_unlock_ball (device_entry (DEVNO_LOCK));
			break;
	}
	/* This should add in an extra ball if the above wasn't enough */
	device_multiball_set (3);
}

CALLSET_ENTRY (mball, mball_start)
{
	if (!flag_test (FLAG_MULTIBALL_RUNNING))
	{
		reset_unlit_shots ();
		flag_on (FLAG_MULTIBALL_RUNNING);
		flag_on (FLAG_MB_JACKPOT_LIT);
		music_refresh ();
		deff_start (DEFF_MB_START);
		leff_start (LEFF_MB_RUNNING);
		mball_locks_lit = 0;
		mball_locks_made = 0;
		mball_jackpot_uncollected = TRUE;
		mballs_played++;
		lamp_off (LM_GUM);
		lamp_off (LM_BALL);
		
		if (!flag_test (FLAG_SUPER_MB_RUNNING))
		{	
			mball_start_3_ball ();	
			ballsave_add_time (10);
		}
	}
}

CALLSET_ENTRY (mball, mball_stop)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
		flag_off (FLAG_MULTIBALL_RUNNING);
		flag_off (FLAG_SUPER_MB_RUNNING);
		flag_off (FLAG_MB_JACKPOT_LIT);
		deff_stop (DEFF_MB_START);
		deff_stop (DEFF_MB_RUNNING);
		deff_stop (DEFF_JACKPOT_RELIT);
		leff_stop (LEFF_MB_RUNNING);
		/* TODO Hacky, fix me */
		lamp_off (LM_GUM);
		lamp_off (LM_BALL);
		music_refresh ();
		//autofire_request_count = 0;
		/* If a jackpot wasn't collected, offer a restart */
		if (mball_jackpot_uncollected && !mball_restart_collected)
			timed_mode_start (GID_MBALL_RESTART_RUNNING, mball_restart_task);
	}
}

/* Called from leftramp.c */
void mball_left_ramp_exit (void)
{
	if (multiball_ready ())
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
	if (multi_ball_play ());
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
		mball_jackpot_uncollected = FALSE;
		//TODO Score ladder for jackpots
		
		score (SC_20M);
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
	//collect_extra_ball ();
	/* Tell fastlock that the lock was entered */
	callset_invoke (fastlock_jackpot_collected);

	/* Collect multiball jackpot if lit */
	if ((flag_test (FLAG_MULTIBALL_RUNNING)) && !flag_test (FLAG_MB_JACKPOT_LIT))
	{
		flag_on (FLAG_MB_JACKPOT_LIT);
		deff_start (DEFF_JACKPOT_RELIT);
	}
	
	/* Check to see if mball_restart is running */
	if (timed_mode_timer_running_p (GID_MBALL_RESTART_RUNNING, &mball_restart_timer))
	{
		sound_send (SND_CRASH);
		score (SC_5M);
		timed_mode_stop (&mball_restart_timer);
		mball_restart_collected = TRUE;
		if (!multi_ball_play ())
			callset_invoke (mball_start);
	}
	/* Lock check should pretty much always go last */
	else if (can_lock_ball ())
	{
		bounded_decrement (mball_locks_lit, 0);
		device_lock_ball (device_entry (DEVNO_LOCK));
		enable_skill_shot ();
		sound_send (SND_FAST_LOCK_STARTED);
		if (mball_locks_lit == 0)
		{
			lamp_off (LM_GUM);
			lamp_off (LM_BALL);
		}
		mball_locks_made++;
		deff_start (DEFF_MB_LIT);
		reset_unlit_shots ();
	}
	else
		/* inform unlit.c that a shot was missed */
		award_unlit_shot (SW_LOCK_LOWER);
}

CALLSET_ENTRY (mball, end_ball)
{
	timed_mode_stop (&mball_restart_timer);
}

CALLSET_ENTRY (mball, start_player)
{
	lamp_off (LM_GUM);
	lamp_off (LM_BALL);
	lamp_tristate_off (LM_MULTIBALL);
	lamp_off (LM_MULTIBALL);
	mball_locks_lit = 0;
	mball_locks_made = 0;
	mballs_played = 0;
	mball_restart_collected = FALSE;
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

CALLSET_ENTRY (mball, ball_search)
{
}
