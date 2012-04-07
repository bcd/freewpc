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
 * \brief An implementation of short, free-running timers.
 *
 * Free timers are similar to the other timer API, but they do not have
 * tasks executing and are therefore more efficient.  The timer is just
 * a counter that is updated at IRQ level.  No action is taken upon
 * expiration, so the only usage is to query a free timer ID to see if
 * it is running.  Free timers are ideal for hardware debouncing such
 * as testing whether or not two switches have been tripped sequentially;
 * the first switch handler would start the timer, and the second will
 * query it.  The implementation is about as simple as can be.
 */

#include <freewpc.h>

/* The array of free timers.  Each byte indicates time-to-expiry in 33ms
 * units, for a max duration of about 8 seconds. */
U8 free_timers[MAX_FREE_TIMERS];


static inline U8 *free_timer_update (U8 *timer_ptr)
{
	if (*timer_ptr)
	{
		(*timer_ptr)--;
	}
	return timer_ptr+1;
}


/** Realtime timer update function.  Each timer value is
simply decremented by 1 tick (33ms) if it is nonzero. */
/* RTT(name=free_timer_rtt freq=32) */
void free_timer_rtt (void)
{
	if (MAX_FREE_TIMERS == 0)
		return;
	U8 *timer_ptr = free_timers;
	do {
		timer_ptr = free_timer_update (timer_ptr);
		timer_ptr = free_timer_update (timer_ptr);
		timer_ptr = free_timer_update (timer_ptr);
		timer_ptr = free_timer_update (timer_ptr);
	} while (timer_ptr < &free_timers[MAX_FREE_TIMERS]);
}


/** Restart a timer. */
void __free_timer_restart (U8 tid, U8 ticks)
{
	ticks /= 2;
	free_timers[tid] = ticks;
}


/** Start a timer.  If it is already started, do nothing. */
void __free_timer_start (U8 tid, U8 ticks)
{
	if (free_timers[tid] == 0)
	{
		ticks /= 2;
		free_timers[tid] = ticks;
	}
}


/** Stop a timer. */
void __free_timer_stop (U8 tid)
{
	free_timers[tid] = 0;
}


/** Test the value of a timer. */
U8 __free_timer_test (U8 tid)
{
	return free_timers[tid] / 2;
}


void free_timer_init (void)
{
	memset (free_timers, 0, sizeof (free_timers));
}

