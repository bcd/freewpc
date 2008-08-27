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
 * \brief Timer APIs
 *
 * Timers are special cases of tasks; they just all run the same
 * function, counting down the time, then exiting.  Actually, there
 * are a few different predefined timer functions, which offer
 * slightly different behavior.
 *
 * The timer APIs are very similar to the task APIs, just with the
 * addition of a timeout value in most places.
 */


#define TIMER_FREERUNNING_GRAN	TIME_100MS
#define TIMER_PAUSABLE_GRAN		TIME_100MS


U8 pausable_timer_locks;


void freerunning_timer_function (void)
{
	U16 ticks = task_get_arg ();
	while (ticks > 0)
	{
		task_sleep (TIMER_FREERUNNING_GRAN);
		ticks -= TIMER_FREERUNNING_GRAN;
	}
	task_exit ();
}


void live_ball_timer_function (void)
{
	U16 ticks = task_get_arg ();
	while (ticks > 0)
	{
		task_sleep (TIMER_PAUSABLE_GRAN);
		if (0) /* TODO : add condition that ball is live */
			continue;
		ticks -= TIMER_PAUSABLE_GRAN;
	}
	task_exit ();
}


task_pid_t timer_restart (task_gid_t gid, U16 ticks, task_function_t fn)
{
	task_pid_t tp = task_recreate_gid (gid, fn);
	task_set_arg (tp, ticks);
	return (tp);
}


task_pid_t timer_start1 (task_gid_t gid, U16 ticks, task_function_t fn)
{
	task_pid_t tp = task_create_gid1 (gid, fn);
	task_set_arg (tp, ticks);
	return (tp);
}


task_pid_t timer_start (task_gid_t gid, U16 ticks, task_function_t fn)
{
	task_pid_t tp = task_create_gid (gid, fn);
	task_set_arg (tp, ticks);
	return (tp);
}


CALLSET_ENTRY (timer, init)
{
	pausable_timer_locks = 0;
}

