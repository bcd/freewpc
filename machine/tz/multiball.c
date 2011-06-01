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
#include <eb.h>

__local__ U8 mball_locks_lit;
__local__ U8 mball_locks_made;
__local__ U8 mballs_played;

/* Current jackpot, in 10M's */
U8 jackpot_level;
/* Used to pass the jackpot level to deff */
U8 jackpot_level_stored;
/* Used to restart if multiball ends without picking up a jackpot */
bool mball_jackpot_uncollected;
bool mball_restart_collected;
extern struct timed_mode_ops mball_restart_mode;

extern U8 unlit_shot_count;
extern U8 live_balls;
extern U8 gumball_enable_count;
extern U8 autofire_request_count;
extern bool fastlock_running (void);
extern U8 lucky_bounces;

bool check_for_midnight (void)
{
	extern U8 hour;
	extern U8 minute;
	if ((hour == 0 && minute < 15)
		|| (hour == 23 && minute > 45))
		return TRUE;
	else
		return FALSE;
}

U8 mball_restart_timer;

void mball_restart_mode_init (void);
void mball_restart_mode_exit (void);

struct timed_mode_ops mball_restart_mode = {
	DEFAULT_MODE,
	.init = mball_restart_mode_init,
	.exit = mball_restart_mode_exit,
	.gid = GID_MBALL_RESTART_MODE,
	.music = MUS_FASTLOCK_COUNTDOWN,
	.deff_running = DEFF_MBALL_RESTART,
	.prio = PRI_MULTIBALL,
	.init_timer = 15,
	.timer = &mball_restart_timer,
	.grace_timer = 3,
	.pause = system_timer_pause,
};

static inline void call_number (U8 number)
{
	switch (number)
	{
		default:
		case 5:
			sound_send (SND_FIVE);
			break;
		case 4:
			sound_send (SND_FOUR);
			break;
		case 3:
			sound_send (SND_THREE);
			break;
		case 2:
			sound_send (SND_TWO);
			break;
		case 1:
			sound_send (SND_ONE);
			break;
	}
}

static void mball_restart_countdown_task (void)
{
	U8 last_number_called = 6;
	task_sleep_sec (8);
	while (mball_restart_timer)
	{
		if (last_number_called > mball_restart_timer)
		{
			last_number_called = mball_restart_timer;
			call_number (last_number_called);	
		}
		task_sleep (TIME_800MS);
	}
	task_exit ();
}


void mball_restart_mode_init (void)
{
	callset_invoke (stop_hurryup);
	task_create_gid (GID_MBALL_RESTART_MODE, mball_restart_countdown_task);
}

void mball_restart_mode_exit (void)
{
}

void mball_restart_deff (void)
{
	U16 fno;
	U8 j = 0;
	dmd_alloc_pair_clean ();
	//while (mball_restart_timer > 0)
	for (;;)
	{
		for (fno = IMG_BOLT_TESLA_START; fno < IMG_BOLT_TESLA_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_center (&font_var5, 64, 16, "SHOOT LOCK TO RESTART");
			j++;
			if (j > 255)
				j = 0;
			if (j % 2 != 0)
			{
				font_render_string_center (&font_fixed6, 64, 4, "MULTIBALL");
			}
			sprintf ("%d", mball_restart_timer);
			font_render_string_center (&font_fixed6, 64, 25, sprintf_buffer);
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
}


CALLSET_ENTRY (mball_restart, mball_restart_stop)
{
	if (timed_mode_running_p (&mball_restart_mode))
		timed_mode_end (&mball_restart_mode);
}

inline void mball_restart_start (void)
{
	timed_mode_begin (&mball_restart_mode);
}
/* Rules to say whether we can start multiball */
bool multiball_ready (void)
{
	/* Don't allow during certain conditions */
	if (global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING) 
		|| global_flag_test (GLOBAL_FLAG_SSSMB_RUNNING)
		|| global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING)
		|| multi_ball_play ()
		|| global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY))
		return FALSE;
	/* Require one locked ball first multiball, 2 locks after */
	else if ((mball_locks_made > 0) && (mballs_played == 0))
		return TRUE;
	else if ((mball_locks_made > 1) && (mballs_played > 0))
		return TRUE;
	else
		return FALSE;

}
CALLSET_ENTRY (mball, display_update)
{
	timed_mode_display_update (&mball_restart_mode);
	if (global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING))
		deff_start_bg (DEFF_MB_RUNNING, 0);
	else if (timed_mode_running_p (&mball_restart_mode))
		deff_start_bg (DEFF_MBALL_RESTART, 0);
}

CALLSET_ENTRY (mball, music_refresh)
{
	if (!in_game)
		return;
	timed_mode_music_refresh (&mball_restart_mode);
	if (global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING))
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
	if (multiball_ready ())
	{
		font_render_string_center (&font_mono5, 64, 20, "SHOOT LEFT RAMP");
		font_render_string_center (&font_mono5, 64, 26, "FOR MULTIBALL");
	}
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

void mb_start_deff (void)
{
	sound_send (SND_DONT_TOUCH_THE_DOOR_AD_INF);
	
		
	U16 fno;
	U8 i;
	for (i = 0; i < 6; i++)
	{
		U8 j = 0;
		for (fno = IMG_BOLT_TESLA_START; fno < IMG_BOLT_TESLA_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
		
			j++;
			if (j > 255)
				j = 0;
			if (j % 2 != 0)
			{
				font_render_string_center (&font_fixed10, 64, 16, "MULTIBALL");
			}
			else if (check_for_midnight ())
			{
				font_render_string_center (&font_fixed10, 64, 16, "MIDNIGHT");
			}
			else
			{
				font_render_string_center (&font_fixed6, 64, 16, "MULTI BALL");
			}
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			if (i < 3)
				task_sleep (TIME_100MS);
			else if (i < 5)
				task_sleep (TIME_66MS);
			else
				task_sleep (TIME_33MS);
		}
	}
	deff_exit ();
}

void jackpot_relit_deff (void)
{
	sound_send (0xFD);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "JACKPOT RELIT");
	dmd_show_low ();
	flash_and_exit_deff (50, TIME_100MS);
}

void mb_jackpot_collected_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_term6, 64, 10, "MB JACKPOT");
	printf_millions (jackpot_level_stored * 10);
	font_render_string_center (&font_fixed6, 64, 21, sprintf_buffer);
	dmd_show_low ();
	sound_send (SND_SKILL_SHOT_CRASH_3);
	task_sleep_sec (1);
	deff_exit ();
}

void mb_running_deff (void)
{
	U16 fno;
	U8 i = 0;
	dmd_alloc_pair_clean ();
	for (;;)
	{
		for (fno = IMG_BOLT_TESLA_START; fno < IMG_BOLT_TESLA_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			if (i >= 254)
				i = 0;
			i++;
			sprintf_current_score ();
			font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
			if (global_flag_test (GLOBAL_FLAG_MB_JACKPOT_LIT))
			{
				sprintf("SHOOT PIANO FOR %dM", (jackpot_level * 10));
				font_render_string_center (&font_var5, 64, 27, sprintf_buffer);
			}
			else
			{
				font_render_string_center (&font_var5, 64, 27, "SHOOT LOCK TO RELIGHT");
			}
			
			if (i % 2 != 0)
			{
				font_render_string_center (&font_fixed6, 64, 4, "MULTIBALL");
			}
			else if (check_for_midnight ())
			{
				font_render_string_center (&font_fixed6, 64, 4, "MIDNIGHT");
			}

			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_100MS);
		}
	}
}

/* Check to see if we can light the lock/lock a ball */
bool can_lock_ball (void)
{	
	if ( mball_locks_lit > 0 
		&& mball_locks_made < 2
		&& !global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING) 
		&& !global_flag_test (GLOBAL_FLAG_BTTZ_RUNNING) 
		&& !global_flag_test (GLOBAL_FLAG_SSSMB_RUNNING) 
		&& !global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING)
		&& !multi_ball_play ()
		&& !pb_in_lock ())
		return TRUE;
	else
		return FALSE;
}

bool can_light_lock (void)
{
	if (can_lock_ball ())
		return TRUE;
	else if (fastlock_running ())
		return TRUE;
	else if (global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING) && !global_flag_test (GLOBAL_FLAG_MB_JACKPOT_LIT))
		return TRUE;
	else if (timed_mode_running_p (&mball_restart_mode))
		return TRUE;
	else if (flag_test (FLAG_SNAKE_READY) && single_ball_play ())
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
	
	if (multiball_ready () && single_ball_play ())
		lamp_tristate_flash (LM_MULTIBALL);
	
	if (multi_ball_play ())
	{
		/* Flash the Piano Jackpot lamp when MB Jackpot is lit */
		if (global_flag_test (GLOBAL_FLAG_MB_JACKPOT_LIT))
			lamp_tristate_flash (LM_PIANO_JACKPOT);
		else if (!global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING))
			lamp_tristate_off (LM_PIANO_JACKPOT);
	}
	/* Turn on and flash door lock lamps during game situations */
	else if (mball_locks_made == 0 && mball_locks_lit == 0)
	{
		lamp_tristate_off (LM_LOCK1);
		lamp_tristate_off (LM_LOCK2);
	}
	else if (mball_locks_made == 0 && mball_locks_lit == 1)
	{
		lamp_tristate_flash (LM_LOCK1);
		lamp_tristate_off (LM_LOCK2);
	}
	else if (mball_locks_made == 0 && mball_locks_lit > 1)
	{
		lamp_tristate_flash (LM_LOCK1);
		lamp_tristate_flash (LM_LOCK2);
	}
	else if (mball_locks_made == 1 && mball_locks_lit == 0)
	{
		lamp_tristate_on (LM_LOCK1);
		lamp_tristate_off (LM_LOCK2);
	}
	else if (mball_locks_made == 1 && mball_locks_lit >= 1)
	{
		lamp_tristate_on (LM_LOCK1);
		lamp_tristate_flash (LM_LOCK2);
	}
	else if (mball_locks_made >= 2)
	{
		lamp_tristate_on (LM_LOCK1);
		lamp_tristate_on (LM_LOCK2);
	}
	
}

void mball_light_lock (void)
{
	if (mball_locks_lit  != 2 && mball_locks_made != 2)
	{
		sound_send (SND_GUMBALL_COMBO);
		deff_start (DEFF_LOCK_LIT);
	}
	bounded_increment (mball_locks_lit, 2);
}

/* Check to see if GUMBALL has been completed and light lock */
void mball_check_light_lock (void)
{
	if (lamp_test (LM_GUM) && lamp_test (LM_BALL))
	{
		mball_light_lock ();
		gumball_enable_count++;
	}
}

 /* How we want the balls released and in what order */
CALLSET_ENTRY (multiball, mball_start_3_ball)
{
	/* Don't start if another multiball is running */
	if (multi_ball_play () || live_balls == 3)
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
			device_unlock_ball (device_entry (DEVNO_LOCK));
			autofire_add_ball ();	
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

CALLSET_ENTRY (multiball, mball_start_2_ball)
{
	if (multi_ball_play () || live_balls > 1)
		return;
	/* Check lock and empty accordingly */
	switch (device_recount (device_entry (DEVNO_LOCK)))
	{	
		/* No balls in lock, fire 1 from trough */
		case 0:
			autofire_add_ball ();	
			break;
		/* 1/2 balls in lock, drop 1 */ 
		case 1:
		case 2:
			device_unlock_ball (device_entry (DEVNO_LOCK));
			break;
	}
	device_multiball_set (2);
}

CALLSET_ENTRY (mball, mball_start)
{
	if (!global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING))
	{
		magnet_reset ();
		callset_invoke (mball_restart_stop);
		unlit_shot_count = 0;
		global_flag_on (GLOBAL_FLAG_MULTIBALL_RUNNING);
		global_flag_on (GLOBAL_FLAG_MB_JACKPOT_LIT);
		music_refresh ();
		kickout_lock (KLOCK_DEFF);
		deff_start (DEFF_MB_START);
		/* Set the jackpot higher if two balls were locked */
		if (mball_locks_made > 1)
			jackpot_level = 3;
		else
			jackpot_level = 1;
		mball_locks_lit = 0;
		mball_locks_made = 0;
		mball_jackpot_uncollected = TRUE;
		mballs_played++;
		/* Turn off all the lamps for the leff to use */
		lamp_off (LM_MULTIBALL);
		lamp_off (LM_GUM);
		lamp_off (LM_BALL);
		lamp_off (LM_LOCK1);
		lamp_off (LM_LOCK2);
		if (check_for_midnight ())
			leff_start (LEFF_BONUS);
		else
			leff_start (LEFF_MB_RUNNING);
	}
}

CALLSET_ENTRY (mball, mball_stop)
{
	if (global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING))
	{
		global_flag_off (GLOBAL_FLAG_MULTIBALL_RUNNING);
		global_flag_off (GLOBAL_FLAG_SUPER_MB_RUNNING);
		global_flag_off (GLOBAL_FLAG_MB_JACKPOT_LIT);
		deff_stop (DEFF_MB_START);
		deff_stop (DEFF_MB_RUNNING);
		deff_stop (DEFF_JACKPOT_RELIT);
		leff_stop (LEFF_MB_RUNNING);
		leff_stop (LEFF_BONUS);
		lamp_off (LM_GUM);
		lamp_off (LM_BALL);
		lamp_tristate_off (LM_PIANO_JACKPOT);
		music_refresh ();
		/* If a jackpot wasn't collected, offer a restart */
		if (mball_jackpot_uncollected && !mball_restart_collected)
			mball_restart_start ();
	}
}

/* Called from leftramp.c */
void mball_left_ramp_exit (void)
{
	if (multiball_ready ())
	{
		leff_start (LEFF_STROBE_DOWN);
		leff_start (LEFF_FLASH_GI2);
		lamp_tristate_off (LM_MULTIBALL);
		callset_invoke (mball_start);
		callset_invoke (mball_start_3_ball);
	}
	else if (!lamp_test (LM_GUM) && !multi_ball_play ())
	{
		lamp_on (LM_GUM);
		mball_check_light_lock ();
	}
	event_can_follow (sw_left_ramp_exit, sw_shooter, TIME_3S);
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

/* If for some reason the ball ends up in the shooter lane, start multiball */
CALLSET_ENTRY (mball, sw_shooter)
{
	if (event_did_follow (sw_left_ramp_exit, sw_shooter) && multiball_ready ())
	{
		callset_invoke (mball_start_3_ball);
		callset_invoke (mball_start);
	}
}

CALLSET_ENTRY (mball, sw_piano)
{
	if (global_flag_test (GLOBAL_FLAG_MB_JACKPOT_LIT))
	{
		/* Piano jackpot was collected */
		magnet_disable_catch (MAG_LEFT);
		global_flag_off (GLOBAL_FLAG_MB_JACKPOT_LIT);
		/* Add anoither 10M to the jackpot if three balls are out */
		if (live_balls == 3)
			bounded_increment (jackpot_level, 5);
		jackpot_level_stored = jackpot_level;
		if (!check_for_midnight ())
			leff_start (LEFF_PIANO_JACKPOT_COLLECTED);
		deff_start (DEFF_JACKPOT);
		deff_start (DEFF_MB_JACKPOT_COLLECTED);
		mball_jackpot_uncollected = FALSE;
		/* Score it */
		score_multiple (SC_10M, jackpot_level);
		/* Increase the jackpot level */
		bounded_increment (jackpot_level, 5);
		timer_restart_free (GID_MB_JACKPOT_COLLECTED, TIME_3S);
	}
}

CALLSET_ENTRY (mball, powerball_jackpot)
{
	deff_start (DEFF_PB_JACKPOT);
	/* -1, as it probably has been incremented already */
	score_multiple (SC_10M, jackpot_level - 1);
	jackpot_level_stored = jackpot_level * 2;
}

CALLSET_ENTRY (mball, any_pf_switch)
{
	if (global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING))
	{
		score (SC_20K);
	}
	if (check_for_midnight () && global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING))
	{
		leff_start (LEFF_FLASHER_HAPPY);
	}
}


CALLSET_ENTRY (mball, single_ball_play)
{
	callset_invoke (mball_stop);
}

CALLSET_ENTRY (mball, dev_lock_enter)
{
	collect_extra_ball ();
	score (SC_50K);
	sound_send (SND_ROBOT_FLICKS_GUN);
	leff_start (LEFF_LOCK);

	/* Tell fastlock that the lock was entered */
	fastlock_lock_entered ();

	if (single_ball_play () && flag_test (FLAG_SNAKE_READY))
		callset_invoke (snake_start);
	
	/* Collect multiball jackpot if lit */
	if ((global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING)) && !global_flag_test (GLOBAL_FLAG_MB_JACKPOT_LIT))
	{
		global_flag_on (GLOBAL_FLAG_MB_JACKPOT_LIT);
		deff_start (DEFF_JACKPOT_RELIT);
	}
	
	/* Check to see if mball_restart is running */
	if (timed_mode_running_p (&mball_restart_mode))
	{
		sound_send (SND_CRASH);
		score (SC_5M);
		callset_invoke (mball_restart_stop);
		mball_restart_collected = TRUE;
		if (!multi_ball_play ())
		{
			callset_invoke (mball_start);
			callset_invoke (mball_start_3_ball);
		}
	}
	/* Lock check should pretty much always go last */
	else if (can_lock_ball ())
	{
		/* Right loop -> Locked ball lucky bounce handler */
		if (event_did_follow (right_loop, locked_ball))
		{
			sound_send (SND_LUCKY);
			score (SC_5M);
			deff_start (DEFF_LUCKY_BOUNCE);
			bounded_increment (lucky_bounces, 99);
			task_sleep_sec (1);
		}

		sound_send (SND_FAST_LOCK_STARTED);
		bounded_decrement (mball_locks_lit, 0);
		bounded_increment (mball_locks_made, 2);
		deff_start_sync (DEFF_MB_LIT);
		/* Lock 2 balls, drop a ball if it's full */
		if (device_recount (device_entry (DEVNO_LOCK)) <= 2)
		//if (!device_full_p (device_entry (DEVNO_LOCK)))
		{	
			device_lock_ball (device_entry (DEVNO_LOCK));
			enable_skill_shot ();
		}
		else 
		{
			//TODO leff as well?
			deff_start (DEFF_BALL_FROM_LOCK);
		}
		
		if (mball_locks_lit == 0)
		{
			lamp_off (LM_GUM);
			lamp_off (LM_BALL);
		}
		unlit_shot_count = 0;
	}
	else
		/* inform unlit.c that a shot was missed */
		award_unlit_shot (SW_LOCK_LOWER);
}

CALLSET_ENTRY (mball, end_ball)
{
	callset_invoke (mball_stop);
}

CALLSET_ENTRY (mball, start_ball)
{
	lamp_tristate_off (LM_MULTIBALL);
	lamp_off (LM_MULTIBALL);
	mball_restart_collected = FALSE;
}

CALLSET_ENTRY (mball, start_player)
{
	lamp_off (LM_GUM);
	lamp_off (LM_BALL);
	mball_locks_lit = 0;
	mball_locks_made = 0;
	mballs_played = 0;
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

CALLSET_ENTRY (mball, left_ball_grabbed)
{
	if (global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING) && global_flag_test (GLOBAL_FLAG_MB_JACKPOT_LIT))
	{
		deff_start (DEFF_SHOOT_JACKPOT);
	}
}
