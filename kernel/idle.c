/*
 * Copyright 2007-2011 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Idle processing
 *
 * This module runs at idle time -- when there are no tasks that need
 * to be scheduled.  It is the third tier of priorities, behind
 * interrupt-level and task-level.  The task scheduler will not call
 * these functions more frequently than once per 16ms though.
 *
 * Functions that need to run as often as possible should register for
 * the 'idle' event.  Most functions can get by running less often, though:
 * those should register one of the time-based idle events.  For example,
 * 'idle_every_100ms' would only run once every 100ms.
 *
 * Time-based idle functions are not guaranteed to run as soon as the
 * time has elapsed, if there are lots of tasks still runnable.  So you
 * cannot use idle to do any realtime processing.  However, this module
 * does track when it is falling behind, and will try to compensate.
 * For example, if the 100ms handlers don't get invoked until after 110ms,
 * next time it will run after only 90ms more has elapsed.  In the long run,
 * it will always average out to 100ms.
 *
 * Time events are implemented for 100ms, 1 second, and 10 seconds.
 * Any other intervals need to be implemented by hooking one of these
 * and counting the time units yourself.
 */

#include <freewpc.h>

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

#ifndef CONFIG_RTC
U8 idle_minute_timer;
#endif

/** Runs the periodic functions.   This function is called
 * about once every 16ms, but it may run less often when
 * there are many tasks running.
 */
void do_periodic (void)
{
	/* Switch processing is special, and will be called as
	often as possible.  Everything else is called less
	frequently. */
	switch_periodic ();

	/* See if at least 100ms has elapsed.
	If so, we advance the timeout for the next check.
	If more than 200ms elapsed, we will only process
	1 'tick' on the current call, and do it again
	on the next run. */
	if (time_reached_p (idle_ready_time))
		idle_ready_time += TIME_100MS;
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
#ifndef CONFIG_RTC
			idle_minute_timer++;
			if (idle_minute_timer >= 6)
			{
				idle_minute_timer -= 6;
				callset_invoke (minute_elapsed);
			}
#endif
		}
	}
}


CALLSET_ENTRY (idle, init)
{
	idle_ready_time = get_sys_time () + TIME_100MS;
	idle_second_timer = 0;
	idle_10second_timer = 0;
#ifndef CONFIG_RTC
	idle_minute_timer = 0;
#endif
}
