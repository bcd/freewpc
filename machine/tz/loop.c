/*
 * Copyright 2006, 2007, 2009 by Brian Dominy <brian@oddchange.com>
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
#include <sys/time.h>
/* Various loop counts, reset at the start of each ball */
__local__ U8 loops;
__local__ U8 powerball_loops;
__local__ U8 spiral_loops;

U16	start_loop_time;
__local__ U8	loop_time;
//__local__ U8	loop_speed_timer_value;
//extern bool left_loopmag_enabled;
//extern bool lower_right_loopmag_enabled;
//extern bool upper_right_loopmag_enabled;

extern __local__ U8 gumball_enable_count;
extern __local__ U8 thing_flips_enable_count;

extern U8 spiral_round_timer;
extern U8 fastlock_round_timer;

extern void thing_flips (void);
extern void award_spiral_loop (void);

/* Functions to stop leffs/deffs during certain game situations */
//TODO They don't work, change spiralaward to a task
static bool can_show_loop_leff (void)
{
	if (free_timer_test (TIM_SPIRALAWARD))
		return FALSE;
	else
		return TRUE;
}

static bool can_show_loop_deff (void)
{
	if (fastlock_running ())
		return FALSE;
	else if (free_timer_test (TIM_SPIRALAWARD))
		return FALSE;
	else
		return TRUE;
}

void enter_loop (void)
{
	if (in_live_game)
	{
		score (SC_1K);
		sound_send (SND_LOOP_ENTER);
	}
}

/* Loop scoring rules */
static void award_loop (void)
{
	/* loops includes powerball and spiral_loops */
	loops++;
	if (flag_test (FLAG_POWERBALL_IN_PLAY) && !multi_ball_play ())
	{
		powerball_loops++;

		if (powerball_loops < 3)
	{
			sound_send (SND_ADDAMS_FASTLOCK_STARTED);
		score (SC_5M);
	}
		else if (powerball_loops == 3)
		{
			sound_send (SND_LOAD_GUMBALL_NOW);
			gumball_enable_count++;
		//	flag_on (MAGNA_MB_LIT);
			score (SC_20M);
//			powerball_loops = 0;
		}
		deff_start (DEFF_PB_LOOP);
		return;
	}
	if (timed_mode_timer_running_p (GID_SPIRAL_ROUND_RUNNING, &spiral_round_timer))
	{
		spiral_loops++;
		award_spiral_loop ();
	}
	else
	/* Plain Old Loop */
	{
		if (loops < 2)
		score (SC_100K);
		else if (loops > 2)
			score (SC_250K);
		else if (loops > 4)
			score (SC_500K);
		else if (loops > 9)
		{	
			score (SC_1M);
			sound_send (SND_THUNDER1);
		}
		sound_send (SND_SPIRAL_AWARDED);
		/* Don't show deff during certain modes */
		if (can_show_loop_deff ())
			deff_start (DEFF_LOOP);
	}
}

static void abort_loop (void)
{
	if (in_live_game)
	{
		score (SC_1K);
		sound_send (SND_SPIRAL_SAME_SIDE_EXIT);
	}
}

static void award_left_loop (void)
{
	if (in_live_game)
	{
		if (can_show_loop_leff ())
			leff_start (LEFF_LEFT_LOOP);
		award_loop ();
	}
}


static void award_right_loop (void)
{
	if (in_live_game)
	{
		event_can_follow (right_loop, piano, TIME_3S);
		award_loop ();
		/* Start timer for loop -> piano */
		if (can_show_loop_leff ())
			leff_start (LEFF_RIGHT_LOOP);
	}
}

void loop_deff (void)
{
	
	dmd_alloc_low_clean ();
	psprintf ("1 LOOP", "%d LOOPS", loops);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	//TODO Doesn't always work
	sprintf_score(score_deff_get ());	
	font_render_string_center (&font_mono5, 64, 18, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void start_loop_speed_timer (void)
{
	start_loop_time = (get_sys_time ());
}

void stop_loop_speed_timer (void)
{
	loop_time = (get_elapsed_time (start_loop_time));
}

CALLSET_ENTRY (loop, sw_left_magnet)
{
	/* Cannot detect loops reliably during multiball */
	//if (multi_ball_play ())
	//	return;
	if (task_kill_gid (GID_LEFT_LOOP_ENTERED))
	{
		/* Left loop aborted */
		abort_loop ();
	}
	else if (task_kill_gid (GID_RIGHT_LOOP_ENTERED))
	{
		/* Right loop completed */
		//TODO Check for multiball here?
		//start_flipper_timer (); to train via player.
		stop_loop_speed_timer ();
		/* Inform thingfl.c and fastlock.c that a loop has been done. */
		callset_invoke (thing_flips);
		//left_magnet_grab_start ();
	
		award_right_loop ();
		fastlock_right_loop_completed ();
		spiralaward_right_loop_completed ();
	}
	else
	{
		/* Left loop started */
		timer_restart_free (GID_LEFT_LOOP_ENTERED, TIME_3S);
		start_loop_speed_timer ();
		enter_loop ();
	}

}


CALLSET_ENTRY (loop, sw_upper_right_magnet)
{
	/* Could autodetect present of third magnet using this switch? */
}


CALLSET_ENTRY (loop, sw_lower_right_magnet)
{

	/* Cannot detect loops reliably during multiball */
	//if (multi_ball_play ())
	//	return;

	if (event_did_follow (dev_lock_kick_attempt, right_loop))
	{
		/* Ball has just come out of lock, ignore */
		return;
	}
	else if (event_did_follow (autolaunch, right_loop))
	{
		/* Ignore right loop switch after an autolaunch */
		enter_loop ();
	}
	else if (task_kill_gid (GID_LEFT_LOOP_ENTERED))
	{
		/* Left loop completed */
		//TODO put hooks for thingfl camera and fastlock left loop.
		stop_loop_speed_timer ();
		fastlock_right_loop_completed ();
		award_left_loop ();
	}
	else if (task_kill_gid (GID_RIGHT_LOOP_ENTERED))
	{
		/* Right loop aborted */
		abort_loop ();
	}
	else
	{
		/* Right loop started */
		timer_restart_free (GID_RIGHT_LOOP_ENTERED, TIME_3S);
		start_loop_speed_timer ();
		enter_loop ();
	}

	/* Inform gumball module that a ball may be approaching */
	sw_gumball_right_loop_entered ();
}

CALLSET_ENTRY (loop, start_ball)
{
	/* Initialise loop counts */
	loops = 0;
	powerball_loops = 0;
	spiral_loops = 0;
	loop_time = 1;
	start_loop_time = 1;
}

CALLSET_ENTRY (loop, init)
{
	//left_loopmag_enabled = FALSE;
	//lower_right_loopmag_enabled = FALSE;
	//upper_right_loopmag_enabled = FALSE;
}

CALLSET_ENTRY (loop, end_ball)
{
	timer_kill_gid (GID_LEFT_LOOP_ENTERED);
	timer_kill_gid (GID_RIGHT_LOOP_ENTERED);
}

