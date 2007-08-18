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
#include <queue.h>

/* Keep this a power of 2 */
#define MAX_TIMERS 4


/** A hard timer entry.  Each entry keeps the number of IRQs remaining
until it expires, and a pointer to the function to be called at expiry. */
struct timer
{
	U8 timeout;
	void (*handler) (U8);
	U8 data;
};


/** An index to the first valid entry in the table.  If head==tail,
then the queue is empty though. */
U8 hard_timer_head;


/** An index to the last valid entry in the table. */
U8 hard_timer_tail;


struct timer timer_table[MAX_TIMERS];


static inline void increment_modulo (U8 *var, const U8 max)
{
	*var = (*var + 1) % max;
}


/** Returns TRUE if the timer queue is empty */
static inline bool timer_queue_empty_p (void)
{
	return hard_timer_head == hard_timer_tail;
}


/** Returns TRUE if the timer queue is full */
static inline bool timer_queue_full_p (void)
{
	return (( hard_timer_tail+1 ) % MAX_TIMERS) == hard_timer_head;
}


void hard_timer_rtt (void)
{
	/* Scan each entry in the queue, and decreases its timeout count
	by 1.  If any entry has reached zero, then 
	remove its entry and call its handler. */
	if (__builtin_expect (!!(hard_timer_head != hard_timer_tail), 0))
	{
		struct timer *tmr = &timer_table[hard_timer_head];
		if (tmr->timeout == 0
			|| --tmr->timeout == 0)
		{
			(*tmr->handler) (tmr->data);
			increment_modulo (&hard_timer_head, MAX_TIMERS);
			hard_timer_rtt ();
		}
	}
}


/** Add a new entry to the hard timer table, saying that HANDLER
should be called after TIMEOUT IRQs.  The handler must reside
in the system page.  If the timer table is full, it returns false
to the caller, indicating that it should be tried again. */
bool hard_timer_add (U8 timeout, void (*handler)(U8), U8 data)
{
	U8 offset;

	if (timer_queue_full_p ())
		return FALSE;

	offset = hard_timer_tail;
	timer_table[offset].timeout = timeout;
	timer_table[offset].handler = handler;
	timer_table[offset].data = data;
	increment_modulo (&offset, MAX_TIMERS);
	return TRUE;
}


#ifdef HARD_TIMER_TEST

U8 hard_timer_test_count;


void hard_timer_test_callback (U8 data)
{
	if (data != hard_timer_test_count)
		dbprintf ("bad callback data");

	hard_timer_test_count++;
	hard_timer_add (TIME_1S, hard_timer_test_callback, hard_timer_test_count);
}


void hard_timer_test (void)
{
	hard_timer_test_count = 0;
	hard_timer_add (TIME_1S, hard_timer_test_callback, 0);
	for (;;)
	{
		dbprintf ("%d\n", hard_timer_test_count);
		task_sleep (TIME_100MS);
	}
	task_exit ();
}

#endif


CALLSET_ENTRY (hard_timer, init)
{
	hard_timer_head = hard_timer_tail = 0;
	memset (timer_table, 0, sizeof (timer_table));
#ifdef HARD_TIMER_TEST
	task_create_gid (GID_HARD_TIMER_TEST, hard_timer_test);
#endif
}

