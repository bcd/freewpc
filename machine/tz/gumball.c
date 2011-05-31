/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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
#include <gumball_div.h>

bool gumball_enable_from_trough;

bool gumball_geneva_tripped;
bool gumball_exit_tripped;
bool gumball_running;
bool powerball_loaded_into_gumball;
U8 gumball_pending_releases;
U8 timeout;

/* How many balls are in the gumball */
__fastram__ U8 gumball_count;

/* How many times can the player enter the Gumball */
__local__ U8 gumball_enable_count;
U8 gumball_collected_count;
U8 gumball_score;

extern U8 pb_location;
extern bool hold_balls_in_autofire;

//TODO Start a timer to check if a ball came out, if not, turn again 

/*************************************************************/
/* Gumball APIs                                              */
/*************************************************************/
void award_gumball_score (void)
{
	callset_invoke (reset_unlit_shots);
	/* Scored in multiple of 5M, capped at 30M */
	if (gumball_collected_count > 5)
	{
		score(SC_30M);
		gumball_score = 30;
	}
	else
	{
		score_multiple(SC_5M, gumball_collected_count);
		/* Tell gumball_deff how much was scored */
		gumball_score = (5 * gumball_collected_count);
	}

	if (global_flag_test(GLOBAL_FLAG_POWERBALL_IN_PLAY))
	{	
		score (SC_5M);
		gumball_score =+ 5;
	}
	
}

/* Used for in game testing of whether the player can collect
 * the gumball */
bool gumball_load_is_enabled (void)
{
	if (!in_live_game)
		return FALSE;
	/* Don't enable if the powerball is likely to be out undetected 
	 * We should wait for a detection event first */
	else if (pb_maybe_in_play () )
		return FALSE;
	/* If powerball is out during single ball play, enable */
	else if ((global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY) || gumball_enable_count)
		&& !multi_ball_play ())
		return TRUE;
	else
		return FALSE;
}

void gumball_load_from_trough (void)
{
	dbprintf ("Gumball load requested\n");
	gumball_enable_from_trough = TRUE;
	autofire_add_ball ();
}

void gumball_release_task (void)
{
	event_should_follow (gumball_release, gumball_exit, TIME_4S);
	while (gumball_pending_releases > 0)
	{
		gumball_geneva_tripped = FALSE;
		gumball_exit_tripped = FALSE;

		/* Original timeout was 90x33ms = 3sec */
		/* Shorter timeout will work when geneva is broken */
		timeout = 25;
		while ((gumball_geneva_tripped == FALSE) && (gumball_exit_tripped == FALSE) && (--timeout > 0))
		{
			sol_request (SOL_GUMBALL_RELEASE);
			task_sleep (TIME_33MS);
		}
		sol_stop (SOL_GUMBALL_RELEASE);
		gumball_running = FALSE;
		bounded_decrement (gumball_pending_releases, 0);
	}
	task_exit ();
}


void gumball_release (void)
{
	gumball_pending_releases++;
	if (!task_find_gid (GID_GUMBALL_RELEASE))
		task_create_gid1 (GID_GUMBALL_RELEASE, gumball_release_task);
}

static void gumball_divertor_open_task (void)
{
	gumball_div_start ();
	task_sleep_sec (2);
	gumball_div_stop ();
	task_exit ();
}

void gumball_divertor_open (void)
{
	task_recreate_gid (GID_GUMBALL_DIV, gumball_divertor_open_task);
}

void gumball_divertor_close (void)
{
	gumball_div_stop ();
	task_kill_gid (GID_GUMBALL_DIV);
}

/* Called by the right loop magnet to see if we should divert the ball */
void sw_gumball_right_loop_entered (void)
{
	/* Open the divertor if trying to load the gumball*/
	if (gumball_enable_from_trough && event_did_follow (autolaunch, right_loop))
	{
		magnet_disable_catch (MAG_RIGHT);
		gumball_divertor_open ();
	}
	/* Don't open if autofired into play or dropped from the lock*/
	else if (event_did_follow (autolaunch, right_loop)
		|| timer_find_gid (GID_LOCK_KICKED))
	{
	}
	/* Don't open if the magnet is about to grab the ball
	 * but remembering that it will always let the powerball through */
	else if ((magnet_enabled (MAG_RIGHT) || magnet_busy (MAG_RIGHT))
		&& !global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY))
	{
	}
	else if (gumball_load_is_enabled ())
	{
		gumball_divertor_open ();
		if (in_live_game && !multi_ball_play ())
		{
			sound_send (SND_GUMBALL_ENTER);
		}
	}
}

/*************************************************************/
/* Switch Handlers                                           */
/*************************************************************/

CALLSET_ENTRY (gumball, sw_gumball_exit)
{
//	if (!global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING))
//		sound_send (SND_GUMBALL_LOADED);
	if (task_kill_gid (GID_GENEVA_TRIPPED) 
		|| event_did_follow (gumball_release, gumball_exit))
	{
		/* Start a timer to tell the slot where the ball came from */
		timer_restart_free (GID_GUMBALL_TO_SLOT, TIME_5S);
		/* Signal the release motor to stop */
		gumball_exit_tripped = TRUE;
		/* A ball successfully came out of the gumball machine.*/
		bounded_decrement (gumball_count, 0);
		if (feature_config.easy_lite_gumball == YES)
		{
			lamp_off (LM_GUM);
			lamp_off (LM_BALL);
		}
	}
}

CALLSET_ENTRY (gumball, sw_gumball_geneva)
{
	dbprintf ("Geneva tripped.\n");
	/* Trigger on the second edge change */
	/* Don't trigger too early */
	if (timeout < 10)
		gumball_geneva_tripped = TRUE;
	timer_restart_free (GID_GENEVA_TRIPPED, TIME_4S);
}

CALLSET_ENTRY (gumball, sw_gumball_enter)
{
	/* Ball has entered the gumball machine. */
	dbprintf ("Gumball entered.\n");
	gumball_enable_from_trough = FALSE;
	gumball_count++;
	if (in_live_game)
	{
		if (!multi_ball_play ())
			leff_start (LEFF_GUMBALL_STROBE);
		gumball_running = TRUE;
		gumball_collected_count++;
		award_gumball_score ();
		gumball_release ();

		if (powerball_loaded_into_gumball == TRUE)
		{
			powerball_loaded_into_gumball = FALSE;
			leff_start (LEFF_FLASH_GI2);
			callset_invoke (mball_start);
			callset_invoke (mball_start_3_ball);
			callset_invoke (powerball_in_gumball);	
			/* Do a dodgy multiball combo */
			global_flag_on (GLOBAL_FLAG_SUPER_MB_RUNNING);
			ballsave_add_time (5);
			
			if (!global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING)
				&& !global_flag_test (GLOBAL_FLAG_SSSMB_RUNNING))
			{
				/* random_scaled (N) returns from 0 - N-1 */
				switch (random_scaled (2))
				{
					case 0:
						callset_invoke (sssmb_start);
						break;
					case 1:
						callset_invoke (chaosmb_start);
						break;
				}
			}
		}
		else
		{
			bounded_decrement (gumball_enable_count, 0);
		}
		if (!global_flag_test (GLOBAL_FLAG_SUPER_MB_RUNNING))
			deff_start (DEFF_GUMBALL);
	}
}

CALLSET_ENTRY (gumball, sw_gumball_lane)
{
	/* Ball is approaching popper.
	 * Gumball diverter can be closed now. */
	gumball_divertor_close ();
	
	if (global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY))
		powerball_loaded_into_gumball = TRUE;
	else
		powerball_loaded_into_gumball = FALSE;
}

/* Called whenever the far left trough switch is tripped.
The sole purpose of this to determine when there are too 
many balls in the trough, and one needs to be fired into the
gumball out of the way.  If a ball remains on this switch for
3 seconds, then it is assumed there are 4 balls in the trough
and one must be loaded. */
void sw_far_left_trough_monitor (void)
{
	U8 timeout = TIME_3S / TIME_200MS;
	device_t *dev = device_entry (DEVNO_TROUGH);

 	while (task_find_gid (GID_DEVICE_PROBE))
		task_sleep_sec (1);

	dbprintf ("Far left trough check\n");
	/* Poll the switch for up to 3 seconds.  If it ever opens,
	then abort.  It must stay closed and the trough must
	remain full in order for us to continue. */
	while (timeout > 0)
	{
		task_sleep (TIME_200MS);
		if ((!switch_poll_logical (SW_FAR_LEFT_TROUGH))
			|| (dev->actual_count != dev->size))
		{
			dbprintf ("Far left trough not stable\n");
			task_exit ();
		}
		timeout--;
	}

	/* If a ball is known to be in play, then delay the
	load */
	while (valid_playfield)
	{
		dbprintf ("Far left trough : waiting to load\n");
		task_sleep_sec (1);
	}

	/* Start the load */
	dbprintf ("Far left trough stable : loading gumball\n");
	if (hold_balls_in_autofire)
	{
		autofire_add_ball ();
	}
	else
		gumball_load_from_trough ();
	task_exit ();
}

void gumball_deff (void)
{
	sound_send (SND_GUMBALL_LOAD_START);
	//music_effect_start (SND_GUMBALL_LOAD_START, SL_3S);
//	task_recreate_gid (GID_GUMBALL_MUSIC_BUG, gumball_music_bug_task);
	U16 fno;
	for (fno = IMG_GUMBALL_START; fno <= IMG_GUMBALL_END; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}
	
	/* Show last two frames two times */
	for (fno = 0 ; fno > 3; fno++)
	{
		dmd_alloc_pair ();
		frame_draw (IMG_GUMBALL_END - 2);
		dmd_show2 ();
		task_sleep (TIME_100MS);
		dmd_alloc_pair ();
		frame_draw (IMG_GUMBALL_END);
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}
	music_effect_start (SND_GUMBALL_LOAD_END, SL_3S);
	task_sleep_sec (2);
	for (fno = 0; fno < 20; fno++)
	{
		dmd_alloc_pair_clean ();
		dmd_clean_page_low ();
		psprintf("1 GUMBALL", "%d GUMBALLS", gumball_collected_count);
		font_render_string_center (&font_steel, 64, 7, sprintf_buffer);
		sprintf("%d MILLION", gumball_score);
		font_render_string_center (&font_steel, 64, 20, sprintf_buffer);
		dmd_copy_low_to_high ();
		callset_invoke (score_overlay);
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}
	deff_exit ();
}

CALLSET_ENTRY (gumball, lamp_update)
{
	if (gumball_load_is_enabled ())
	{
		lamp_tristate_flash (LM_GUMBALL_LANE);
	}
	else
		lamp_tristate_off (LM_GUMBALL_LANE);
}


CALLSET_ENTRY (gumball, sw_far_left_trough)
{
	if (!in_test)
		task_create_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}

/* Close the gumball divertor if a ball is
 * coming round the loop from the left */
CALLSET_ENTRY (gumball, sw_left_magnet)
{
	gumball_divertor_close ();
}

CALLSET_ENTRY (gumball, start_ball)
{
	task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
	gumball_score = 0;
	gumball_collected_count = 0;
	gumball_enable_from_trough = FALSE;
	global_flag_off (GLOBAL_FLAG_SUPER_MB_RUNNING);
}

CALLSET_ENTRY (gumball, end_ball)
{
	gumball_divertor_close ();
}

CALLSET_ENTRY (gumball, start_player, init)
{
	gumball_enable_count = 0;
}

CALLSET_ENTRY (gumball, amode_start)
{
	task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}

CALLSET_ENTRY (gumball, empty_balls_test)
{
	U8 count;
	for (count = 3; count > 0; --count)
	{	
		gumball_release ();
	}
}

CALLSET_ENTRY (gumball, start_far_left_trough_monitor)
{
	task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}

CALLSET_ENTRY (gumball, ball_search)
{
	/* TODO : when ball searching at game start, see if the
	extra balls are in the gumball and try to release 1. */
	//if (sw_gumball_enter = enabled for 2 seconds and trough + lock = 2, then empty
	task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}

CALLSET_ENTRY (gumball, door_start_light_gumball)
{
	gumball_enable_count++;
}

CALLSET_ENTRY (gumball, single_ball_play)
{
	global_flag_off (GLOBAL_FLAG_SUPER_MB_RUNNING);
}

CALLSET_ENTRY (gumball, init)
{
	global_flag_off (GLOBAL_FLAG_SUPER_MB_RUNNING);
	gumball_enable_from_trough = FALSE;
	gumball_pending_releases = 0;
	gumball_count = 3;
	powerball_loaded_into_gumball = FALSE;
}

void award_right_loop_task (void)
{
	callset_invoke (award_right_loop);
	task_exit ();
}

CALLSET_ENTRY (gumball, sw_gumball_popper)
{
	/* A right loop was completed, TODO Could be BUGGY */
	timer_restart_free (GID_GUMBALL, TIME_1S);
	if (task_kill_gid (GID_RIGHT_LOOP_ENTERED))
		task_create_anon (award_right_loop_task);
}

CALLSET_ENTRY (gumball, status_report)
{
	status_page_init ();
	sprintf ("%d GUMBALLS ENABLED", gumball_enable_count);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	sprintf ("%d COLLECTED", gumball_collected_count);
	font_render_string_center (&font_mono5, 64, 18, sprintf_buffer);
	sprintf ("%d BALLS IN GUMBALL", gumball_count);
	font_render_string_center (&font_var5, 64, 25, sprintf_buffer);
	status_page_complete ();
}
