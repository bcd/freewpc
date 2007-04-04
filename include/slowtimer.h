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

/**
 * \file slowtimer.h
 * Slowtimers are those that run in multiples of 1 second.  They are
 * typically used to implement game features like modes and hurry-ups.
 * A single slow timer task manages all of the timers.
 */

#ifndef _SLOWTIMER_H
#define _SLOWTIMER_H


/** The maximum number of slow timers that the system supports. */
#define MAX_SLOW_TIMERS 16

/** The update frequency of the timers.  Although slowtimers have
 * 1s granularity, they can be updated more frequently.  This helps
 * out when timers are paused and then restarted. */
#define SLOW_TIMER_UPDATE_FREQ (TIME_200MS + TIME_50MS)

/** This is related to SLOW_TIMER_UPDATE_FREQ, and indicates the
 * number of iterations before 1s has expired. */
#define SLOW_TIMER_UPDATE_COUNT 4

/** Set on a timer object when the timer is in the running state */
#define SLOW_TIMER_RUNNING 0x1

/** Set on a timer object when it has been declared periodic, meaning
 * that it will be restarted automatically upon expiry. */
#define SLOW_TIMER_PERIODIC 0x2

#define SLOW_TIMER_AUTOFREE 0x4

/** Set on a timer that should be stop automatically at end ball */
#define SLOW_TIMER_KILL_END_BALL 0x8

/* Forward declaration */
struct slow_timer;


/** Defines the configuration of a slow timer.  This is a read-only
 * object. */
typedef struct
{
	/** The number of seconds for which the timer should run */
	U8 duration;

	/** The timer value at which a warning is emitted */
	U8 warning;

	/** The number of grace period seconds alloted.  During this time,
	 * the timer will read "0" but technically it is still running.
	 * The expire function is not called until after the grace period. */
	U8 grace;

	/** The default flags for the timer */
	U8 flags;

	/** A function that determines when this timer should be paused.
	 * This is checked on every iteration of the timer thread before
	 * decrementing the count. */
	bool (*pause) (void);

	/** A function to call when the timer expires */
	void (*expire_func) (struct slow_timer *timer);

	/** A function to call when the timer reaches its warning count. */
	void (*warn_func) (struct slow_timer *timer);
} slow_timer_config_t;


/** Defines the state of an allocated timer.  This is a read-write object. */
typedef struct slow_timer
{
	/** Pointer to the configuration for this timer.
	TODO : no bank switching is performed when reading this, so modules
	using slowtimers have to be in the fixed area. */
	slow_timer_config_t *config;

	/** The current count */
	U8 count;

	/** The timer flags */
	U8 flags;
} slow_timer_t;


/* Function prototypes */

__common__ slow_timer_t *slow_timer_alloc (slow_timer_config_t *cfg);
__common__ U8 slow_timer_get_count (slow_timer_t *timer);
__common__ void slow_timer_set_count (slow_timer_t *timer, U8 count);
__common__ void slow_timer_start (slow_timer_t *timer);
__common__ void slow_timer_free (slow_timer_t *timer);
__common__ void slow_timer_stop (slow_timer_t *timer);
__common__ void slow_timer_create (slow_timer_config_t *cfg);
__common__ void slow_timer_kill_subset (U8 flags);
__common__ slow_timer_t *slow_timer_find (slow_timer_config_t *cfg);
__common__ void slow_timer_dump (void);

#endif /* _SLOWTIMER_H */

