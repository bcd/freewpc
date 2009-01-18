/*
 * Copyright 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/** The frequency to run the idle checks */
#define IDLE_PERIOD (TIME_100MS * IRQS_PER_TICK)

/** The next time at which we should run the idle event.
This is calculated as 100ms from the last timeout. */
U16 idle_ready_time;

/** The number of 100ms events generated.  When this
reaches 10, we clear it and consider that one second
has passed. */
U8 idle_second_timer;

/** The number of 1sec events generated.  When this
reaches 10, we clear it and consider that 10 seconds
have passed. */
U8 idle_10second_timer;


/** Runs the idle functions.   This function is called
 * whenever there are no tasks ready to run, at a
 * rate of once per 1ms.
 */
void do_idle (void)
{
	callset_invoke (idle);

	/* See if at least 100ms has elapsed.
	If so, we advance the timeout for the next check.
	If more than 200ms elapsed, we will only process
	1 'tick' on the current call, and do it again
	on the next run. */
	if (time_reached_p (idle_ready_time))
		idle_ready_time += IDLE_PERIOD;
	else
		return;

	/* Throw the 100ms event */
	callset_invoke (idle_every_100ms);

	/* Throw the 1 second event every 10 calls */
	idle_second_timer++;
	if (idle_second_timer >= 10)
	{
		idle_second_timer -= 10;
		callset_invoke (idle_every_second);

		/* Throw the 10 second event if that has elapsed */
		idle_10second_timer++;
		if (idle_10second_timer >= 10)
		{
			idle_10second_timer -= 10;
			callset_invoke (idle_every_ten_seconds);
		}
	}
}


CALLSET_ENTRY (idle, init)
{
	idle_ready_time = get_sys_time () + IDLE_PERIOD;
	idle_second_timer = 0;
	idle_10second_timer = 0;
}
