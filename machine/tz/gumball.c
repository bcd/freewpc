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

//TODO If left mag sw is thrown during divertor open, close divertor immediately
bool gumball_enable_from_trough;

bool gumball_geneva_tripped;
bool gumball_exit_tripped;
bool gumball_running;
bool powerball_loaded_into_gumball;
U8 gumball_pending_releases;
U8 timeout;

extern sound_code_t music_active;

/* How many times can the player enter the Gumball */
__local__ U8 gumball_enable_count;
U8 gumball_collected_count;
U8 gumball_score;
//extern U8 fastlock_round_timer;
//extern void spiralaward_right_loop_completed (void);
//extern U8 mpf_round_timer;
//extern void mball_start_3_ball (void);

extern U8 pb_location;

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

	if (flag_test(FLAG_POWERBALL_IN_PLAY))
	{	
		score (SC_20M);
		gumball_score =+ 20;
	}
	
}

bool gumball_load_is_enabled (void)
{
	if (in_live_game && (gumball_enable_count > 0) 
		&& !multi_ball_play ())
		return TRUE;
	/* If powerball is out during single ball play, enable */
	else if (flag_test (FLAG_POWERBALL_IN_PLAY) && !multi_ball_play ())
		return TRUE;
	/* Allow loading from the trough */
	else if (gumball_enable_from_trough)
		return TRUE;
	else
		return FALSE;
}

void gumball_load_from_trough (void)
{
	extern void autofire_add_ball (void);

	dbprintf ("Gumball load requested\n");
	gumball_enable_from_trough = TRUE;
	autofire_add_ball ();
}

void gumball_release_task (void)
{
	event_should_follow (gumball_release, gumball_exit, TIME_3S);
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
	task_create_gid1 (GID_GUMBALL_RELEASE, gumball_release_task);
}

void gumball_divertor_open (void)
{
	gumball_div_start ();
}


void gumball_divertor_close (void)
{
	gumball_div_stop ();
}


void sw_gumball_right_loop_entered (void)
{
	if (gumball_load_is_enabled ())
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
	gumball_exit_tripped = TRUE;
	sound_send (SND_GUMBALL_LOADED);
	if (event_did_follow (gumball_geneva, gumball_exit) 
		|| event_did_follow (gumball_release, gumball_exit))
	{
		/* A ball successfully came out of the gumball machine.*/
		/* Signal the release motor to stop */
		/* If the geneva switch is broken this isn't going to work! */
		device_switch_can_follow (gumball_exit, camera, TIME_4S);
		event_can_follow (gumball_exit, camera, TIME_4S);
		event_can_follow (gumball_exit, slot, TIME_4S);
	}
	/* Add on another 5 million if the ball manages to
	 * hit the gumball exit switch as it leaves the MPF */
}

CALLSET_ENTRY (gumball, sw_gumball_geneva)
{
	dbprintf ("Geneva tripped.\n");
	/* Trigger on the second edge change */
	/* Don't trigger too early */
	if (timeout < 10)
		gumball_geneva_tripped = TRUE;
	event_should_follow (gumball_geneva, gumball_exit, TIME_2S);
}

CALLSET_ENTRY (gumball, sw_gumball_enter)
{
	/* Ball has entered the gumball machine. */
	dbprintf ("Gumball entered.\n");
	if (gumball_enable_from_trough)
		gumball_enable_from_trough = FALSE;
	if (in_live_game)
	{
		if (!multi_ball_play ())
			leff_start (LEFF_GUMBALL_STROBE);
		gumball_running = TRUE;
		gumball_collected_count++;
		award_gumball_score ();
		gumball_release ();
		bounded_decrement (gumball_enable_count, 0);
		/* Powerball was loaded into Gumball */
		if (powerball_loaded_into_gumball == TRUE)
		{
			powerball_loaded_into_gumball = FALSE;
			callset_invoke (powerball_in_gumball);	
			/* Do a dodgy multiball combo */
			flag_on (FLAG_SUPER_MB_RUNNING);
			ballsave_add_time (5);
			
			switch (random_scaled (1))
			{
				case 0:
					callset_invoke (sssmb_start);
					break;
				case 1:
					callset_invoke (chaosmb_start);
					break;
			}
			callset_invoke (mball_start);
			callset_invoke (mball_start_3_ball);
		}
		if (!flag_test (FLAG_SUPER_MB_RUNNING))
			deff_start (DEFF_GUMBALL);
	}
}

CALLSET_ENTRY (gumball, music_refresh)
{
}

CALLSET_ENTRY (gumball, sw_gumball_lane)
{
	/* Ball is approaching popper.
	 * Gumball diverter can be closed now. */
	gumball_divertor_close ();
	//gumball_load_disable ();
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
	gumball_load_from_trough ();
	task_exit ();
}

void gumball_deff (void)
{
	sound_send (SND_GUMBALL_LOAD_START);
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
	sound_send (SND_GUMBALL_LOAD_END);
	task_sleep_sec (2);
	dmd_alloc_low_clean ();
	psprintf("1 GUMBALL", "%d GUMBALLS", gumball_collected_count);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	sprintf("%d MILLION", gumball_score);
	font_render_string_center (&font_fixed6, 64, 18, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (1);
	//TODO BUG?
	music_request (music_active, PRI_JACKPOT);
	deff_exit ();
}

CALLSET_ENTRY (gumball, lamp_update)
{
	if (gumball_load_is_enabled ())
	{
		magnet_disable_catch (MAG_RIGHT);
		lamp_tristate_flash (LM_GUMBALL_LANE);
	}
	else
		lamp_tristate_off (LM_GUMBALL_LANE);
}

CALLSET_ENTRY (gumball, sw_far_left_trough)
{
	if (!in_test)
		task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}


CALLSET_ENTRY (gumball, start_ball)
{
	task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
	gumball_score = 0;
	gumball_collected_count = 0;
	flag_off (FLAG_SUPER_MB_RUNNING);
}


CALLSET_ENTRY (gumball, start_player)
{
	gumball_enable_count = 0;
	gumball_enable_from_trough = FALSE;
}

CALLSET_ENTRY (gumball, amode_start)
{
	task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}


CALLSET_ENTRY (gumball, empty_balls_test)
{
	U8 count;
	for (count = 3; count > 0; --count)
		gumball_release ();
}


CALLSET_ENTRY (gumball, ball_search)
{
	/* TODO : when ball searching at game start, see if the
	extra balls are in the gumball and try to release 1. */
	//if (sw_gumball_enter = enabled for 2 seconds and trough + lock = 2, then empty
	if (switch_poll_logical (SW_GUMBALL_ENTER))
		gumball_release ();
}

CALLSET_ENTRY (gumball, door_start_light_gumball)
{
	gumball_enable_count++;
}

CALLSET_ENTRY (gumball, single_ball_play)
{
	flag_off (FLAG_SUPER_MB_RUNNING);
	callset_invoke (sssmb_stop);
	callset_invoke (mball_stop);
}

CALLSET_ENTRY (gumball, init)
{
	flag_off (FLAG_SUPER_MB_RUNNING);
	gumball_enable_from_trough = FALSE;
	gumball_pending_releases = 0;
	powerball_loaded_into_gumball = FALSE;
}

CALLSET_ENTRY (gumball, sw_gumball_popper)
{
	if (flag_test (FLAG_POWERBALL_IN_PLAY) && switch_poll_logical (SW_TROUGH_PROXIMITY))
		powerball_loaded_into_gumball = TRUE;
	else
		powerball_loaded_into_gumball = FALSE;

	/* A right loop was completed, TODO Could be BUGGY */
	timer_restart_free (GID_GUMBALL, TIME_1S);
	callset_invoke (sw_left_magnet);
}

CALLSET_ENTRY (gumball, status_report)
{
	status_page_init ();
	sprintf ("%d GUMBALLS ENABLED", gumball_enable_count);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	sprintf ("%d COLLECTED", gumball_collected_count);
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	status_page_complete ();
}
