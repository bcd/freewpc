/*
 * Copyright 2007 by Brian Dominy <brian@oddchange.com>
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

#define MAX_FREE_TIMERS 32

typedef U8 free_timer_id_t;

U8 free_timers[MAX_FREE_TIMERS];


/** Realtime timer update function.  Each timer value is
simply decremented by 1 if it is nonzero. */
void free_timer_rtt (void)
{
	U8 *timer_ptr = free_timers;
	do {
		if (*timer_ptr)
		{
			(*timer_ptr)--;
		}
	} while (++timer_ptr < &free_timers[MAX_FREE_TIMERS]);
}


void free_timer_restart (free_timer_id_t tid, U8 ticks)
{
	free_timers[tid] = ticks;
}


/** Start a timer.  If it is already started, do nothing. */
void free_timer_start (free_timer_id_t tid, U8 ticks)
{
	if (free_timers[tid] == 0)
	{
		free_timers[tid] = ticks;
	}
}


/** Stop a timer. */
void free_timer_stop (free_timer_id_t tid)
{
	free_timers[tid] = 0;
}


/** Test the value of a timer. */
U8 free_timer_test (free_timer_id_t tid)
{
	return free_timers[tid];
}

