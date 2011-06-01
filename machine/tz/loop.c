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
#include <sys/time.h>
/* Various loop counts, reset at the start of each ball */
U8 loops;
U8 powerball_loops;

U16 start_loop_time;
U8 loop_time;
/* Used to pass loop score to deff */
score_t loop_score;

extern __local__ U8 gumball_enable_count;
extern __local__ U8 thing_flips_enable_count;
extern U8 balls_needed_to_load;
extern void thing_flips (void);
 

/* Functions to stop leffs/deffs during certain game situations */
static inline bool can_show_loop_leff (void)
{
	if (task_find_gid (GID_SPIRALAWARD))
		return FALSE;
	else
		return TRUE;
}

static inline void enter_loop (void)
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
	bounded_increment (loops, 255);
	event_can_follow (loop, ball_grab, TIME_400MS);
	callset_invoke (award_spiral_loop);
	if (fastlock_running ())
		timer_restart_free (GID_FASTLOCK_LOOP_AWARDED, TIME_5S);	
	if (global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY) && !multi_ball_play ())
	{
		bounded_increment (powerball_loops, 3);

		if (powerball_loops < 3)
		{
			sound_send (SND_ADDAMS_FASTLOCK_STARTED);
			score_add (loop_score, score_table[SC_5M]);
			score (SC_5M);
		}
		else if (powerball_loops == 3)
		{
			sound_send (SND_LOAD_GUMBALL_NOW);
			gumball_enable_count++;
			score (SC_20M);
		}
		deff_start (DEFF_PB_LOOP);
	}
	else
	{
		/* Plain Old Loop */
		score_zero (loop_score);
		if (loops < 2)
		{
			score_add (loop_score, score_table[SC_100K]);
			score (SC_100K);
		}
		else if (loops < 4)
		{
			score_add (loop_score, score_table[SC_250K]);
			score (SC_250K);
		}
		else if (loops < 8)
		{
			score_add (loop_score, score_table[SC_500K]);
			score (SC_500K);
		}
		else if (loops < 10)
		{	
			score_add (loop_score, score_table[SC_750K]);
			score (SC_750K);
		}
		else
		{
			score_add (loop_score, score_table[SC_1M]);
			score (SC_1M);
			sound_send (SND_THUNDER1);
		}
		fastlock_loop_completed ();

		if (!task_find_gid (GID_SPIRALAWARD) || !task_find_gid (GID_GUMBALL))
		{	
			sound_send (SND_SPIRAL_AWARDED);
			deff_start (DEFF_LOOP);
		}
	}
}

static inline void abort_loop (void)
{
	if (in_live_game)
	{
		score (SC_1K);
		sound_send (SND_SPIRAL_SAME_SIDE_EXIT);
	}
}

CALLSET_ENTRY (loop, award_left_loop)
{
	if (!in_live_game)
		return;
	if (can_show_loop_leff ())
		leff_start (LEFF_LEFT_LOOP);
	award_loop ();
}

CALLSET_ENTRY (loop, award_right_loop)
{
	if (!in_live_game)
		return;
	if (can_show_loop_leff ())
		leff_start (LEFF_RIGHT_LOOP);
	award_loop ();
}

void pb_loop_deff (void)
{
	dmd_alloc_pair_clean ();
	U16 fno;
	U8 x;
	for (fno = IMG_LOOP_START; fno < IMG_LOOP_END; fno += 2)
	{
		/* How many steps before the end */
		x = IMG_LOOP_END - fno;
		dmd_map_overlay ();
		dmd_clean_page_low ();
		sprintf ("POWERBALL LOOP");
		font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
		sprintf_score (score_table[SC_10M]);
		font_render_string_center (&font_mono5, 64, 22, sprintf_buffer);
	
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		callset_invoke (score_overlay);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	/* Get rid of the last dirty frame */
	dmd_alloc_pair_clean ();
	sprintf ("POWERBALL LOOP");
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	sprintf_score (score_table[SC_10M]);
	font_render_string_center (&font_mono5, 64, 22, sprintf_buffer);
	dmd_copy_low_to_high ();
	callset_invoke (score_overlay);
	dmd_show2 ();
	task_sleep (TIME_600MS);
	dmd_show_low ();
	task_sleep (TIME_400MS);

	deff_exit ();

}


void loop_deff (void)
{
	dmd_alloc_pair_clean ();
	U16 fno;
	U8 x;
	for (fno = IMG_LOOP_START; fno < IMG_LOOP_END; fno += 2)
	{
		/* How many steps before the end */
		x = IMG_LOOP_END - fno;
		dmd_map_overlay ();
		dmd_clean_page_low ();

		psprintf ("1 LOOP", "%d LOOPS", loops);
		if ( x > 3 )
		{
			font_render_string_center (&font_fixed6, 64, 6 + x, sprintf_buffer);
			sprintf_score (loop_score);
			font_render_string_center (&font_mono5, 64, 26 - x, sprintf_buffer);
		}
		else
		{
			font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
			sprintf_score (loop_score);
			font_render_string_center (&font_mono5, 64, 22, sprintf_buffer);

		}
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		callset_invoke (score_overlay);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	/* Get rid of the last dirty frame */
	dmd_alloc_pair_clean ();
	psprintf ("1 LOOP", "%d LOOPS", loops);
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	sprintf_score (loop_score);
	font_render_string_center (&font_mono5, 64, 22, sprintf_buffer);
	dmd_copy_low_to_high ();
	callset_invoke (score_overlay);
	dmd_show2 ();
//	task_sleep_sec (1);
	task_sleep (TIME_600MS);
	dmd_show_low ();
	task_sleep (TIME_400MS);

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
	if (task_kill_gid (GID_LEFT_LOOP_ENTERED))
	{
		/* Left loop aborted */
		if (task_find_gid (GID_LEFT_TO_RIGHT_THROW))
		{
			callset_invoke (magnet_throw_left_same_side);
		}

		abort_loop ();
	}
	else if (task_kill_gid (GID_RIGHT_LOOP_ENTERED))
	{
		/* Right loop completed */
		stop_loop_speed_timer ();
		callset_invoke (award_right_loop);
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
	deff_stop (DEFF_SHOOT_RIGHT_LOOP);
	/* Inform gumball module that a ball may be approaching */
	if (!task_find_gid (GID_LEFT_LOOP_ENTERED))
		sw_gumball_right_loop_entered ();
	if (timer_find_gid (GID_LOCK_KICKED))
	{
		/* Ball has just come out of lock, ignore */
	}
	else if (event_did_follow (autolaunch, right_loop))
	{
		/* Ignore right loop switch after an autolaunch */
	}
	else if (task_kill_gid (GID_LEFT_LOOP_ENTERED))
	{
		/* Left loop completed */
		stop_loop_speed_timer ();
		callset_invoke (award_left_loop);
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
}

CALLSET_ENTRY (loop, start_ball)
{
	/* Initialise loop counts */
	loops = 0;
	powerball_loops = 0;
	loop_time = 1;
	start_loop_time = 1;
}

CALLSET_ENTRY (loop, end_ball)
{
	timer_kill_gid (GID_LEFT_LOOP_ENTERED);
	timer_kill_gid (GID_RIGHT_LOOP_ENTERED);
}
