/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
#include <search.h>

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

#ifdef MACHINE_TZ
extern bool mpf_active;
#endif
/*
 * Check if timers should be paused.
 *
 * Returns TRUE if timers should not run for some reason.
 * Returns FALSE if timers should continue to run.
 */
bool system_timer_pause (void)
{
	if (!in_game || in_bonus || !valid_playfield)
		return TRUE;

	if (global_flag_test (GLOBAL_FLAG_BALL_AT_PLUNGER) && single_ball_play ())
		return TRUE;

	if (global_flag_test (GLOBAL_FLAG_COIN_DOOR_OPENED))
		return TRUE;

	if (config_timed_game)
	{
		extern U8 timed_game_suspend_count;

		if (timer_find_gid (GID_TIMED_GAME_PAUSED))
			return TRUE;

		if (timed_game_suspend_count)
			return TRUE;
	}

	if (device_holdup_count ())
		return TRUE;

	if (ball_search_timed_out ())
		return TRUE;

	if (timer_find_gid (GID_BALLSAVE_EXTENDED))
		return TRUE;
	#ifdef MACHINE_TZ
	if (mpf_active)
		return TRUE;
	#endif
	return FALSE;
}


void timer_pause_second (void)
{
	do {
		task_sleep (TIME_400MS);
	} while (system_timer_pause ());
	do {
		task_sleep (TIME_400MS);
	} while (system_timer_pause ());
	do {
		task_sleep (TIME_400MS);
	} while (system_timer_pause ());
}


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

