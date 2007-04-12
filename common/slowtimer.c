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


/**
 * \file
 * \brief The slow timer APIs, which operate at 1s granularity and add support for 
 * exit routines, grace periods, etc.
 */



/** The table of slow timer objects */
slow_timer_t slow_timers[MAX_SLOW_TIMERS];



/**
 * Dump the slowtimer table to the debugger port.
 */
void slow_timer_dump (void)
{
	U8 i;

	dbprintf ("\n");
	for (i=0 ; i < MAX_SLOW_TIMERS; i++)
	{
		slow_timer_t *timer = &slow_timers[i];
		dbprintf ("T%p: %02X %02X ", timer, timer->flags, timer->count);
		if (timer->config)
		{
			dbprintf ("   %p   %02X %02X", 
				timer->config, 
				far_read8 (&timer->config->duration, timer->config_page),
				far_read8 (&timer->config->warning, timer->config_page));
		}
		dbprintf ("\n");
	}
}


/**
 * Returns true if all slow timers should be paused.
 * The defualt conditions are when devices are held up in a multiball device
 * temporarily, or during ball search.  (The latter condition is not quite
 * right -- TODO -- but is good enough for now.)
 */
static bool slow_timer_default_pause (void)
{
	return ((device_holdup_count() > 0) || ball_search_timed_out ());
}


/**
 * The slow timer thread is a single task that handles the updates of all the
 * timer entries in the table.
 */
static void slow_timer_thread (void)
{
	U8 i;

	/* The slow timer thread should never be stopped, so protect it. */
	task_set_flags (TASK_PROTECTED);

	for (;;)
	{
		if (!slow_timer_default_pause ()) /* generic check says ok to update timers */
		{
			for (i=0 ; i < MAX_SLOW_TIMERS; i++)
			{
				slow_timer_t *timer = &slow_timers[i];

				/* Is the timer running and nonzero?
				 * If the timer config specified additional rules for when it should be
				 * paused, is it still OK to advance the timer count? */
				if ((timer->count != 0) 
					&& (timer->flags & SLOW_TIMER_RUNNING)
					&& (!timer->config->pause || !timer->config->pause ()))
				{
					/* Yes, advance the timer. */
					timer->count--;
					if (timer->count == 0)
					{
						/* Timer expired */
						if (timer->config->expire_func)
							timer->config->expire_func (timer);
						timer->flags &= ~SLOW_TIMER_RUNNING;
		
						if (timer->flags & SLOW_TIMER_PERIODIC)
						{
						}
						else if (timer->flags & SLOW_TIMER_AUTOFREE)
						{
							slow_timer_free (timer);
						}
					}
					else if (timer->count == 
						SLOW_TIMER_UPDATE_COUNT * timer->config->warning)
					{
						/* Timer reaches the warning mark. */
						if (timer->config->warn_func)
							timer->config->warn_func (timer);
					}
				}
			}
		}

		/* OK, serviced all timers this period.  Wait then do it all over again. */
		task_sleep (SLOW_TIMER_UPDATE_FREQ);
	}
}


void slow_timer_free (slow_timer_t *timer)
{
	timer->count = 0;
	timer->config = NULL;
}


/** Return the current count of a running timer.
 * If the timer is in its grace period, it will return 0. */
U8 slow_timer_get_count (slow_timer_t *timer)
{
	U8 secs = timer->count / SLOW_TIMER_UPDATE_COUNT;
	if (secs <= timer->config->grace)
		return 0;
	else
		return secs - timer->config->grace;
}


/** Resets the count of a running timer. */
void slow_timer_set_count (slow_timer_t *timer, U8 count)
{
#ifdef GCC4 /* TODO */
#else
	timer->count = (count + timer->config->grace) * SLOW_TIMER_UPDATE_COUNT;
#endif
}


/** Allocate a slow timer and initialize it from the given configuration. */
slow_timer_t *slow_timer_alloc (slow_timer_config_t *cfg)
{
	U8 i;
	for (i=0 ; i < MAX_SLOW_TIMERS; i++)
		if (slow_timers[i].config == NULL)
		{
			slow_timers[i].config = cfg;
			slow_timers[i].flags = cfg->flags;
			slow_timers[i].count = 0;
			return &slow_timers[i];
		}
	return NULL;
			
}


/** Start the timer. */
void slow_timer_start (slow_timer_t *timer)
{
#ifdef GCC4 /* TODO */
#else
	timer->count = (timer->config->duration + timer->config->grace)
		* SLOW_TIMER_UPDATE_COUNT;
#endif
	timer->flags |= SLOW_TIMER_RUNNING;
}


/** Stop the timer. */
void slow_timer_stop (slow_timer_t *timer)
{
	timer->flags &= ~SLOW_TIMER_RUNNING;
}


/**
 * The normal way to create and start a timer in one shot.
 * A table entry is allocated for it, it is initialized from
 * its configuration, and it is started.  Upon completion,
 * it is also freed automatically.
 */
void slow_timer_create (slow_timer_config_t *cfg)
{
	slow_timer_t *timer = slow_timer_alloc (cfg);
	if (timer)
	{
		slow_timer_start (timer);
		timer->flags |= SLOW_TIMER_AUTOFREE;
	}
}


/** Find the timer for a given configuration.
 * Only the first such timer will be returned, in the event that
 * multiple timers were started from the same config. */
slow_timer_t *slow_timer_find (slow_timer_config_t *cfg)
{
	U8 i;
	for (i=0 ; i < MAX_SLOW_TIMERS; i++)
	{
		slow_timer_t *timer = &slow_timers[i];
		if (timer->config == cfg)
			return timer;
	}
	return NULL;
}


/** Stop and free all of the timers of a certain class. */
void slow_timer_kill_subset (U8 flags)
{
	U8 i;
	for (i=0 ; i < MAX_SLOW_TIMERS; i++)
	{
		slow_timer_t *timer = &slow_timers[i];
		if (timer->config && (timer->flags & flags))
		{
			slow_timer_stop (timer);
			slow_timer_free (timer);
		}
	}
}


CALLSET_ENTRY (slow_timer, end_ball)
{
	slow_timer_kill_subset (SLOW_TIMER_KILL_END_BALL);
}


CALLSET_ENTRY (slow_timer, init)
{
#if 0
	/* Initialize the slow timer subsystem.
	 * A dedicated thread runs continuously to update all of the timers. */
	memset (slow_timers, 0, sizeof (slow_timers));
	task_create_gid (GID_TIMER_THREAD, slow_timer_thread);
#endif
}

