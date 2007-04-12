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


U8 autofire_request_count;


CALLSET_ENTRY (autofire, sw_autofire1)
{
	score (SC_100);
}


CALLSET_ENTRY (autofire, sw_autofire2)
{
	score (SC_100);
}


__taskentry__ void autofire_handler (void)
{
	task_set_flags (TASK_PROTECTED);
	while (autofire_request_count > 0)
	{
		while (kickout_locks > 0)
			task_sleep (TIME_33MS);

		/* Open autofire diverter */
		sol_on (SOL_SHOOTER_DIV);

		/* Request kick from trough */
		device_request_kick (device_entry (DEVNO_TROUGH));

		/* Wait for autofire switch to go off once */
		/* TODO : add timeout here */
		task_sleep_sec (1);
#if 0
		while (!switch_poll_logical (SW_AUTOFIRE2))
			task_sleep (TIME_66MS);
#else
		task_sleep_sec (1);
#endif

		/* Close autofire diverter */
		sol_off (SOL_SHOOTER_DIV);

		/* Wait for the ball to settle */
		task_sleep_sec (1);

retry:
		/* Disable right orbit shots */

		/* Open diverter again and kick ball */
		sol_on (SOL_SHOOTER_DIV);
		task_sleep (TIME_100MS * 5);

		sol_pulse (SOL_AUTOFIRE);
		if (in_live_game)
			sound_send (SND_EXPLOSION_1);
		event_can_follow (autolaunch, right_loop, TIME_4S);
		task_sleep (TIME_100MS * 5);

		/* Close diverter */
		sol_off (SOL_SHOOTER_DIV);

		/* Check that ball actually launched OK.
		 * If it didn't, then retry the kick. */
		task_sleep_sec (2);
		if (switch_poll_logical (SW_AUTOFIRE2))
			goto retry;

		/* Decrement request count */
		autofire_request_count--;
	}
	task_exit ();
}


void autofire_add_ball (void)
{
	autofire_request_count++;
	task_create_gid1 (GID_AUTOFIRE_HANDLER, autofire_handler);
}


/* Signals that a ball is headed to the autofire lane from the ramp
divertor and that it needs to be caught in the autoplunger, rather than
falling into the manual plunger lane */
void autofire_catch (void)
{
}


CALLSET_ENTRY (autofire, dev_trough_kick_attempt)
{
	/* Decide whether or not to open the autofire divertor before
	kicking the ball.  If so, then also wait until the autofire
	lane is clear before allowing the ball to go. */

	/* The default strategy is to autofire only when live_balls >= 2. */
	if (live_balls >= 2)
	{
		/* Need to open the divertor */
	}
}


CALLSET_ENTRY (autofire, init)
{
	autofire_request_count = 0;
}

