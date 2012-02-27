/*
 * Copyright 2009-2010 by Brian Dominy <brian@oddchange.com>
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

#include <sys/time.h>
#include <stdlib.h>
#include <freewpc.h>
#include <native/log.h>
#include <simulation.h>

bool linux_irq_enable;
bool linux_firq_enable;
extern void do_firq (void);
extern void do_irq (void);


/**
 * A counter that represents the simulation time, in milliseconds.
 */
unsigned long realtime_counter;


/**
 * Returns the current simulation time.
 */
unsigned long realtime_read (void)
{
	return realtime_counter;
}

/** Realtime callback function.
 *
 * This event simulates an elapsed 1ms.
 */
void realtime_tick (void)
{
#define FIRQ_FREQ 8
#define PERIODIC_FREQ 16

	static unsigned long next_firq_time = FIRQ_FREQ;
	static unsigned long next_periodic_time = PERIODIC_FREQ;

#ifdef CONFIG_SIM
	/* Update all of the simulator modules that need periodic processing */
	sim_time_step ();
#endif

	/* Simulate an IRQ every 1ms */
	if (linux_irq_enable)
	{
#ifdef CONFIG_GEN_RTT
		exec_rtt ();
#else
		tick_driver ();
#endif
	}

#ifdef CONFIG_FIRQ
	/* Simulate an FIRQ every 8ms */
	if (linux_firq_enable)
	{
		while (realtime_read () >= next_firq_time)
		{
			do_firq ();
			next_firq_time += FIRQ_FREQ;
		}
	}
#endif

	/* Call periodic processes every 16ms */
	if (realtime_read () >= next_periodic_time)
	{
		db_periodic ();
		if (likely (periodic_ok))
			do_periodic ();
		next_periodic_time += PERIODIC_FREQ;
	}
}


/**
 * Implement a realtime loop on a non-realtime OS.
 *
 * This function never returns.  It is called from a separate thread context to
 * simulate what would be done at interrupt time on a realtime OS.  Instead,
 * it sleeps for short durations, and on wakeup invokes the target's realtime
 * functions multiple times, depending on how long the sleep lasted.
 */
void realtime_loop (void)
{
	struct timeval prev_time, curr_time;
	int usecs_elapsed = 0;
#ifdef CONFIG_DEBUG_LATENCY
	int latency;
#endif

	gettimeofday (&prev_time, NULL);
	for (;;)
	{
		/* Delay a small amount on most iterations of the loop.
			If we do not ever sleep, then this task will consume all of the CPU.

			Our goal is to sleep for 1ms (1000 usecs) evenly on every iteration.
			However we adjust this value in two ways:  First, we always try to
			sleep slightly less than that, since this function will consume some
			CPU time.  The rough estimate for this is 100 usecs.

			Second, handle actual delays that are fractional.  For example, if we
			actually sleep for 2.9ms, we will only simulate two interrupts, and
			900 usec is ignored.  So for the next iteration, don't sleep for that
			extra 900 usec.

			Because of the adjustments, usecs_asked can go negative; if it does,
			we won't sleep at all. */
		int usecs_asked = 1000 - usecs_elapsed - 100;
		if (usecs_asked > 0)
		{
#if defined(CONFIG_PTH)
			pth_nap (pth_time (0, usecs_asked));
#elif defined(CONFIG_PTHREADS)
			usleep (usecs_asked);
#else
#error "No thread library supported for realtime yet"
#endif
		}

		/* Now see how long we actually slept.  This takes into account the
		actual sleep time, which is typically longer on a multitasking OS,
		and also the length of time that the previous iteration took.  Even
		if we did not sleep at all above, usecs_elapsed will be positive here. */
		gettimeofday (&curr_time, NULL);
		usecs_elapsed = curr_time.tv_usec - prev_time.tv_usec;
		if (usecs_elapsed < 0)
			usecs_elapsed += 1000000;

#ifdef CONFIG_DEBUG_LATENCY
		/* This is for debugging only to see how good your native OS is.
		Print a message when the latency is more than 0.5ms than we requested . */
		latency = usecs_elapsed - usecs_asked;
		if (latency > 200)
			print_log ("latency %d usec\n", latency);
#endif
		prev_time = curr_time;

		/* Invoke realtime tick at least once every time through the loop.
		So if we slept < 1ms (either the OS lied to us, or we didn't sleep at all),
		we'll make forward progress. */
		realtime_counter++;
		realtime_tick ();
		usecs_elapsed -= usecs_asked;

		if (usecs_elapsed > 20000)
		{
		}

		/* If any remaining millseconds occurred during the wait, handle them */
		while (usecs_elapsed >= 1000)
		{
			realtime_counter++;
			realtime_tick ();
			usecs_elapsed -= 1000;
		}

		/* Whatever was left over will be subtracted from the next delay, so
		that we stay on schedule */
	}
}


CALLSET_ENTRY (native_realtime, init)
{
	realtime_counter = 0;
}

