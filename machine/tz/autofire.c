/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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


/** A count of the number of pending autolaunches. */
U8 autofire_request_count;


CALLSET_ENTRY (autofire, sw_autofire1)
{
	score (SC_100);
}


CALLSET_ENTRY (autofire, sw_autofire2)
{
	score (SC_100);
}


/** A task that manages the autolaunching of balls.
Upon entry, the autofire divertor solenoid is already pulsing
and a ball is being kicked from the trough. */
void autofire_monitor (void)
{
	/* Catch the ball */
	task_sleep_sec (1);
	sol_off (SOL_SHOOTER_DIV);

	/* Wait for the ball to settle */
	task_sleep_sec (1);
	
	/* Wait until allowed to kickout */
	while (kickout_locks > 0)
		task_sleep (TIME_33MS);

	/* Kick the ball */

	/* Open diverter again and kick ball */
	sol_on (SOL_SHOOTER_DIV);
	task_sleep (TIME_500MS);
	sol_pulse (SOL_AUTOFIRE);
	if (in_live_game)
		sound_send (SND_EXPLOSION_1);
	event_can_follow (autolaunch, right_loop, TIME_4S);
	task_sleep (TIME_500MS);
	sol_off (SOL_SHOOTER_DIV);
	task_sleep_sec (2);
	task_exit ();
}	


/** Called just before the trough kicks a ball when it ought to
go to the autofire lane rather than the manual plunger. */
void autofire_open_for_trough (void)
{
	while (task_find_gid (GID_AUTOFIRE_HANDLER))
		task_sleep_sec (1);
	sol_on (SOL_SHOOTER_DIV);
	task_sleep_sec (1);
	task_create_gid (GID_AUTOFIRE_HANDLER, autofire_monitor);
}


/** Request that a new ball be autolaunched into play. */
void autofire_add_ball (void)
{
	autofire_request_count++;
	device_request_kick (device_entry (DEVNO_TROUGH));
}


/** Signals that a ball is headed to the autofire lane from the ramp
divertor and that it needs to be caught in the autoplunger, rather than
falling into the manual plunger lane */
void autofire_catch (void)
{
	task_sleep_sec (1);
	sol_on (SOL_SHOOTER_DIV);
	task_sleep_sec (2);
	task_create_gid1 (GID_AUTOFIRE_HANDLER, autofire_monitor);
}


CALLSET_ENTRY (autofire, dev_trough_kick_attempt)
{
	/* Decide whether or not to open the autofire divertor before
	kicking the ball.  If so, then also wait until the autofire
	lane is clear before allowing the ball to go. */

	/* The default strategy is to autofire only when in multiball. */
	if (in_live_game
		&& (live_balls || autofire_request_count))
	{
		if (autofire_request_count > 0)
			autofire_request_count--;

		/* Need to open the divertor */
		autofire_open_for_trough ();
	}
	else
	{
		dbprintf ("Sending ball to manual plunger\n");
	}
}


CALLSET_ENTRY (autofire, init)
{
	autofire_request_count = 0;
}

