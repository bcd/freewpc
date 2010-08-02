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
bool autofire_busy;

void sw_autofire (void)
{
	/* TODO : balls landing here when not expected
	still need to be ejected.  See amode_start below.  This
	would also need to happen if the autofire monitor
	failed to kick properly; some retry logic is needed. */
	sol_stop (SOL_RAMP_DIVERTOR);
	score (SC_100);
}

CALLSET_ENTRY (autofire, sw_autofire1)
{
	sw_autofire ();
}


CALLSET_ENTRY (autofire, sw_autofire2)
{
	sw_autofire ();
}


/** A task that manages the autolaunching of balls.
Upon entry, the autofire divertor solenoid is already pulsing
and a ball is being kicked from the trough. */
void autofire_monitor (void)
{
	/* Open the divertor to catch the ball.  Because it may be
	coming from either the trough or a ramp divert, the
	timings are variable. */
	if (shooter_div_delay_time)
		task_sleep_sec (shooter_div_delay_time);
	shooter_div_start ();
	autofire_busy = TRUE;
	/* TODO - If the autofire switch trips during the 'open
	time', we can abort this delay early and go ahead and
	close the divertor.  This is safe because only one
	ball can appear here at a time. */
	task_sleep_sec (shooter_div_open_time);
	shooter_div_stop ();

	/* Wait a little longer for the ball to settle */
	task_sleep (TIME_500MS);
	
	/* Wait until allowed to kickout */
	while (kickout_locks > 0)
		task_sleep (TIME_100MS);

	/* If Right inlane -> Left ramp combo, start tnf mode */
	if (timer_kill_gid (GID_LEFT_RAMP_AUTOFIRE) && single_ball_play ())
	{
		callset_invoke (tnf_start);
		deff_start_sync (DEFF_TNF);
	}
	/* Open diverter again */
	shooter_div_start ();
	/* Wait for the diverter to fully open before firing */
	task_sleep_sec (1);
	/* TODO If the switch fails, it won't fire the ball */
	if (switch_poll_logical (SW_AUTOFIRE2))
	{	
		sol_request (SOL_AUTOFIRE);
		if (in_live_game && single_ball_play ())
		{
			sound_send (SND_EXPLOSION_1);
			leff_start (LEFF_STROBE_UP);
			/* Clear the magnet so we can fire a ball */
			timer_restart_free (GID_BALL_LAUNCH, TIME_3S);
			magnet_disable_catch (MAG_RIGHT);
		}
		/* Say that the ball is heading into the right loop */
		event_can_follow (autolaunch, right_loop, TIME_4S);
	}
	/* Wait for the ball to clear the divertor */
	task_sleep (TIME_700MS);
	shooter_div_stop ();
	autofire_busy = FALSE;
	task_exit ();
}	


/** Called just before the trough kicks a ball when it ought to
go to the autofire lane rather than the manual plunger. */
void autofire_open_for_trough (void)
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

	dbprintf ("Shooter divertor open to catch\n");
	shooter_div_delay_time = 0;
	shooter_div_open_time = 3;
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
		task_sleep_sec (1);		
		sol_request (SOL_BALL_SERVE);
	}
	/* Make sure a ball can be served from the trough */
	else
	{
		/* The normal way to kick a ball from the trough. */
		device_request_kick (device_entry (DEVNO_TROUGH));
	}
}



/** Signals that a ball is headed to the autofire lane from the ramp
divertor and that it needs to be caught in the autoplunger, rather than
falling into the manual plunger lane */
void autofire_catch (void)
{
	/* TODO - don't always want to launch right away, e.g. for
	a start multiball animation */
	shooter_div_delay_time = 0;
	shooter_div_open_time = 4;
	task_create_gid1_while (GID_AUTOFIRE_HANDLER, autofire_monitor, TASK_DURATION_INF);
}


CALLSET_ENTRY (autofire, dev_trough_kick_attempt)
{
	/* Decide whether or not to open the autofire divertor before
	kicking the ball.  If so, then also wait until the autofire
	lane is clear before allowing the ball to go. */

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
		magnet_disable_catch (MAG_RIGHT);
	}
	else
	{
		dbprintf ("no.\n");
	}
}

CALLSET_ENTRY (autofire, clear_autofire)
{
	/* Used to empty autofire if found full
	 * during attract mode */
	shooter_div_start ();
	task_sleep (TIME_1S);
	if (switch_poll_logical (SW_AUTOFIRE2))
		sol_request (SOL_AUTOFIRE);
	task_sleep (TIME_1S);
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
	autofire_request_count = 0;
	autofire_busy = FALSE;
}

CALLSET_ENTRY (autofire, init)
{
	autofire_request_count = 0;
	autofire_busy = FALSE;
}

