/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com
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
//TODO Pressing extra ball during mpf could do something
#include <freewpc.h>
#include <bridge_open.h>
/** The number of balls enabled to go to the MPF */
__local__ U8 mpf_enable_count;

/** Number of balls currently on the mini-playfield */
U8 mpf_ball_count;
U8 mpf_round_timer;
U8 mpf_award;
U8 __local__ mpf_level;
bool mpf_active;

/* Where the powerball is */
//extern U8 pb_location;
extern U8 unlit_shot_count;
void mpf_round_deff (void)
{
	mpf_award = 10;
	for (;;)
	{
		
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 5, "BATTLE THE POWER");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		sprintf ("%d,000,000", (mpf_award * mpf_level));
		font_render_string_center (&font_var5, 64, 27, sprintf_buffer);
		sprintf ("%d", mpf_round_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_show_low ();
		task_sleep_sec (1);
		if (mpf_award > 5)
			bounded_decrement (mpf_award, 0);
	}
}

void mpf_award_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%d,000,000", (mpf_award * mpf_level));
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	sound_send (SND_EXPLOSION_3);
	task_sleep_sec (2);
	deff_exit ();
}

/* Task to pulse the mpf magnets
 * Gets killed when a ball exits */
void mpf_ballsearch_task (void)
{
	U8 i = 0;
	while (mpf_ball_count > 0 && i < 3)
	{
		task_sleep_sec (5);
		sol_request (SOL_MPF_RIGHT_MAGNET);
		task_sleep (TIME_500MS);
		sol_request (SOL_MPF_LEFT_MAGNET);
		task_sleep (TIME_500MS);
		sol_request (SOL_MPF_RIGHT_MAGNET);
		task_sleep (TIME_500MS);
		sol_request (SOL_MPF_LEFT_MAGNET);
		task_sleep (TIME_500MS);
		i++;
	}
	task_exit ();
		
}

void mpf_round_begin (void)
{
	deff_start (DEFF_MPF_ROUND);
}

void mpf_round_expire (void)
{
	mpf_active = FALSE;
	deff_stop (DEFF_MPF_ROUND);
	/* Start a task to pulse the magnets
	 * if a ball gets stuck */
	task_recreate_gid (GID_MPF_BALLSEARCH, mpf_ballsearch_task);	
}

void mpf_round_end (void)
{
	mpf_active = FALSE;
}

void mpf_round_task (void)
{
	timed_mode_task (mpf_round_begin, mpf_round_expire, mpf_round_end,
		&mpf_round_timer, 10, 3);
}

CALLSET_ENTRY (mpf, display_update)
{
	if (timed_mode_timer_running_p (GID_MPF_ROUND_RUNNING,
		&mpf_round_timer) && (mpf_ball_count > 0))
		deff_start_bg (DEFF_MPF_ROUND, 0);
}

CALLSET_ENTRY (mpf, music_refresh)
{
	if (timed_mode_timer_running_p (GID_MPF_ROUND_RUNNING,
		&mpf_round_timer))
		music_request (MUS_POWERFIELD, PRI_GAME_MODE1);
}

CALLSET_ENTRY (mpf, end_ball)
{
	timed_mode_stop (&mpf_round_timer);
}

bool mpf_ready_p (void)
{
	return (mpf_enable_count > 0)
		/* Don't allow if PB might be on playfield */
		&& !flag_test (FLAG_POWERBALL_IN_PLAY)
		//&& !(pb_location & PB_MAYBE_IN_PLAY)
		&& !flag_test (FLAG_MULTIBALL_RUNNING)
		&& !flag_test (FLAG_QUICK_MB_RUNNING)
		&& !flag_test (FLAG_BTTZ_RUNNING)
		&& !flag_test (FLAG_CHAOSMB_RUNNING)
		&& !flag_test (FLAG_SSSMB_RUNNING);

}


CALLSET_ENTRY (mpf, lamp_update)
{
	if (mpf_ready_p ())
		lamp_tristate_on (LM_RAMP_BATTLE);
	else
		lamp_tristate_off (LM_RAMP_BATTLE);
}

/* Closing this switch does not imply that the
 * mpf award was collected, but we can tell camera.c
 * to expect a ball coming from the mpf */
CALLSET_ENTRY (mpf, sw_mpf_top)
{
	event_should_follow (mpf_top, camera, TIME_5S);
	sound_send (SND_EXPLOSION_3);
	score (SC_500K);
}

/* Called from camera.c */
CALLSET_ENTRY (mpf, mpf_collected)
{
	bounded_decrement (mpf_ball_count, 0);
	/* Safe to enable as it covers all cases */
	flipper_enable ();
	leff_stop (LEFF_MPF_ACTIVE);
	score_multiple(SC_1M, (mpf_award * mpf_level));
	//flasher_pulse (FLASH_POWERFIELD);
	if (mpf_ball_count == 0)
	{
		mpf_active = FALSE;
		timed_mode_stop (&mpf_round_timer);
		task_kill_gid (GID_MPF_BALLSEARCH);
	}
	kickout_lock (KLOCK_DEFF);
	deff_start (DEFF_MPF_AWARD);
	kickout_lock (KLOCK_DEFF);
	callset_invoke (award_door_panel);
}

CALLSET_ENTRY (mpf, sw_mpf_enter)
{
	/* If tripped immediately after the right ramp opto, then a ball
	has truly entered the mini-playfield.  Note this may trip later
	on when a ball is already in play. */
	if (event_did_follow (right_ramp, mpf_enter))
	{
		mpf_active = TRUE;
		unlit_shot_count = 0;
		mpf_ball_count++;
		/* Add on 10 seconds for each extra ball */
		if (mpf_ball_count > 1)
			mpf_round_timer += 10;
		mpf_level++;
		bounded_decrement (mpf_enable_count, 0);
		if ((mpf_ball_count = 1))
		{	
			timed_mode_start (GID_MPF_ROUND_RUNNING, mpf_round_task);
			if (!multi_ball_play ())
			{
				/* Turn off GI and start lamp effect */
				leff_start (LEFF_MPF_ACTIVE);
				flipper_disable ();
				bridge_open_stop ();
			}
		}
	}
	/* A ball sneaked in during multiball */
	else if (multi_ball_play ())
	{
		sound_send (SND_WITH_THE_DEVIL);
		score (SC_5M);
		task_recreate_gid (GID_MPF_BALLSEARCH, mpf_ballsearch_task);	
	}
}

CALLSET_ENTRY (mpf, sw_mpf_exit)
{
	/* Stop the ball search timer */
	task_kill_gid (GID_MPF_BALLSEARCH);
	if (mpf_ball_count > 0)
		bounded_decrement (mpf_ball_count, 0);
	if (mpf_ball_count == 0)
	{
		mpf_active = FALSE;
		leff_stop (LEFF_MPF_ACTIVE);
		timed_mode_stop (&mpf_round_timer);
		/* This should be fine as we only disable in single ball play */
		flipper_enable ();
	}
	sound_send (SND_HAHA_POWERFIELD_EXIT);
}
//TODO Light show
CALLSET_ENTRY (mpf, sw_mpf_left)
{
	if (mpf_ball_count > 0)
	{	
		sound_send (SND_POWER_GRUNT_1);
		score (SC_250K);
	}
}

CALLSET_ENTRY (mpf, sw_mpf_right)
{
	if (mpf_ball_count > 0)
	{
		sound_send (SND_POWER_GRUNT_2);
		score (SC_250K);
	}
}


CALLSET_ENTRY (mpf, sw_right_ramp)
{
	/* If the mini-playfield is enabled, open the ramp
	divertor fully.  The ordinary catch and drop is bypassed. */
	event_should_follow (right_ramp, mpf_enter, TIME_3S);
}

CALLSET_ENTRY (mpf, start_player)
{
	mpf_enable_count = 1;
	mpf_level = 0;
	mpf_active = FALSE;
}

CALLSET_ENTRY (mpf, ball_search)
{
	sol_request (SOL_MPF_LEFT_MAGNET);
	task_sleep_sec (1);
	sol_request (SOL_MPF_RIGHT_MAGNET);
	task_sleep_sec (1);
}
