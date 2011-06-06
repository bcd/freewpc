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
#include <shooter_div.h>

/** A count of the number of pending autolaunches. */
U8 autofire_request_count;

/** Delay time in seconds before opening the shooter divertor */
U8 shooter_div_delay_time;

/** Delay time in seconds before closing the shooter divertor */
U8 shooter_div_open_time;

/* Flag to say whether the autofire is busy firing a ball */
//bool autofire_busy;
extern bool hold_balls_in_autofire;

CALLSET_ENTRY (autofire, sw_autofire1, sw_autofire2)
{
	sol_stop (SOL_RAMP_DIVERTOR);
	/* Don't close the divertor if we are trying to launch the ball */
	if (!task_find_gid (GID_AUTOFIRE_HANDLER))
		shooter_div_stop ();
	score (SC_100);
}

/* Function to wait 6 seconds or until the autofire is detected as loaded */
static void autofire_ball_catch_wait (void)
{
	U8 timeout = 60; /* 6 seconds */
	while (!switch_poll_logical (SW_AUTOFIRE2) 
		&& --timeout != 0)
	{
		task_sleep (TIME_100MS);
	}
}

CALLSET_ENTRY (autofire, autofire_launch)
{
	sol_request (SOL_AUTOFIRE);
	if (in_live_game && single_ball_play ())
	{
		sound_send (SND_EXPLOSION_1);
		leff_start (LEFF_STROBE_UP);
	}
	/* Say that the ball is heading into the right loop 
	 * This stops the right magnet from trying to grab the ball */
	timer_restart_free (GID_BALL_LAUNCH, TIME_2S);
	event_can_follow (autolaunch, right_loop, TIME_4S);
	/* Clear the magnet so we can fire a ball */
	magnet_disable_catch (MAG_RIGHT);
}

/** A task that manages the autolaunching of balls.
Upon entry, the autofire divertor solenoid is already pulsing
and a ball is being kicked from the trough. */
void autofire_monitor (void)
{
	//autofire_busy = TRUE;
	/* Open the divertor to catch the ball.  Because it may be
	coming from either the trough or a ramp divert, the
	timings are variable. */
	if (shooter_div_delay_time)
		task_sleep_sec (shooter_div_delay_time);
	
	//if (autofire_full ()
	//	don't open to catch 
	
	if (!switch_poll_logical (SW_AUTOFIRE2))
	shooter_div_start ();

	/* Wait for the ball to enter the autofire */	
	autofire_ball_catch_wait ();	
	shooter_div_stop ();
	
	/* Wait a little longer for the ball to settle 
	 * and the divertor to close */
	
	/* If Right inlane -> Left ramp combo, start tnf mode */
	if (task_kill_gid (GID_TNF_APPROACHING) && single_ball_play ())
	{
		callset_invoke (tnf_start);
	}
	
	/* Wait until allowed to kickout */
	task_sleep (TIME_200MS);
	while (kickout_locks || task_find_gid (GID_LOCK_KICKED))
		task_sleep (TIME_200MS);
	
	/* Open diverter again */
	shooter_div_start ();
	
	/* Wait for the diverter to fully open before firing */
	task_sleep (TIME_700MS);	
	
	/* Launch the ball */
	if (feature_config.fire_empty == NO)
	{
		if (switch_poll_logical (SW_AUTOFIRE2))
			callset_invoke (autofire_launch);
	}
	else
		callset_invoke (autofire_launch);
	
	/* Wait for the ball to clear before closing the divertor */
	task_sleep (TIME_700MS);
	shooter_div_stop ();
	//autofire_busy = FALSE;
	task_exit ();
}	

static void autofire_sleep_while_busy (void)
{
	/* Do not proceed if another ball is in the process of
	being autofired. */
	while (task_find_gid (GID_AUTOFIRE_HANDLER))
	{
		dbprintf ("Autofire waiting for previous.\n");
		/* TODO : these sleeps should be done in the spawned task,
		not in the interface */
		task_sleep_sec (1);
	}
}

/** Called just before the trough kicks a ball when it ought to
go to the autofire lane rather than the manual plunger. */
void autofire_open_for_trough (void)
{
	autofire_sleep_while_busy ();
	dbprintf ("Shooter divertor open to catch\n");
	shooter_div_delay_time = 0;
	shooter_div_open_time = 2;
	task_create_gid_while (GID_AUTOFIRE_HANDLER, autofire_monitor, TASK_DURATION_INF);
}


/** Request that a new ball be autolaunched into play. */
void autofire_add_ball (void)
{
	autofire_request_count++;
	
	if (!in_game || switch_poll_logical (SW_FAR_LEFT_TROUGH))
	{
		/* For special situations.  If not in game, the
		kick_attempt hook won't be called to open for trough.
		If far left trough is set, the kick will appear failed
		because the trough count won't change (even though a
		ball was successfully kicked).  In these cases, do it
		manually.  However, you get no retry capability here. */
		autofire_open_for_trough ();
		/* Wait for divertor to open */
		task_sleep_sec (2);		
		sol_request (SOL_BALL_SERVE);
	}
	else
	{
		/* The normal way to kick a ball from the trough.
		 * dev_trough_kick_attempt will be called next */
		device_request_kick (device_entry (DEVNO_TROUGH));
	}
}



/** Signals that a ball is headed to the autofire lane from the ramp
divertor and that it needs to be caught in the autoplunger, rather than
falling into the manual plunger lane */
void autofire_catch (void)
{
	shooter_div_delay_time = 0;
	shooter_div_open_time = 4;
	task_create_gid1_while (GID_AUTOFIRE_HANDLER, autofire_monitor, TASK_DURATION_INF);
}


CALLSET_ENTRY (autofire, dev_trough_kick_attempt)
{
	/* Decide whether or not to open the autofire divertor before
	kicking the ball.  If so, then also wait until the autofire
	lane is clear before allowing the ball to go. */

	while (task_find_gid (GID_MUTE_AND_PAUSE))
		task_sleep (TIME_100MS);
	
	dbprintf ("need to autofire? live=%d, request=%d ...",
		live_balls, autofire_request_count);
	if (live_balls || autofire_request_count)
	{
		dbprintf ("yes.\n");
		bounded_decrement (autofire_request_count, 0);

		/* Need to open the divertor */
		autofire_open_for_trough ();
		/* Wait for the divertor to open */	
		task_sleep_sec (1);
	}
	else
	{
		dbprintf ("no.\n");
	}
	/* Autofire solenoid will now be pulsed */
}

CALLSET_ENTRY (autofire, clear_autofire)
{
	/* Used to empty autofire if found full
	 * during attract mode */
	shooter_div_start ();
	task_sleep_sec (2);
	if (feature_config.fire_empty == NO)
	{
		if (switch_poll_logical (SW_AUTOFIRE2))
			sol_request (SOL_AUTOFIRE);
	}
	else
		sol_request (SOL_AUTOFIRE);
	task_sleep_sec (1);
	shooter_div_stop ();
}

CALLSET_ENTRY (autofire, ball_search)
{
	/* The shooter divertor/autofire are both kicked here
	since there is a dependency between the two.  The main
	ball search routine is told not to kick either one of them. */
	callset_invoke (clear_autofire);
}

CALLSET_ENTRY (autofire, start_ball)
{
	/* TODO Bug in trough mech, multidrain can cause all other balls to be autofired 
	 * live_balls > 0 even at the start of a ball*/
//	live_balls = 0;
	autofire_request_count = 0;
//	autofire_busy = FALSE;
}

CALLSET_ENTRY (autofire, init)
{
	autofire_request_count = 0;
//	autofire_busy = FALSE;
}

