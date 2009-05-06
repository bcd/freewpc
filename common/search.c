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


/**
 * \file
 * \brief Ball search logic
 *
 * Ball search is triggered based on idle time during a game with no
 * playfield switch closures.  While a game is in progress, a monitor
 * task runs which decrements a timer.  Various conditions are checked
 * that pause the timer; e.g. when the ball is known to be at the
 * plunger.  The switch module resets the timer every time a playfield
 * switch closes.  If the timer hits zero, it triggers an actual ball
 * search, which pulses all of the playfield solenoids that might
 * legitimiately free the ball.  This logic avoids drives attached
 * to flashers or to any game-defined devices that should be avoided,
 * like the knocker or device kickout coils.
 *
 * Ball search is currently indefinite and will gladly run forever.
 * A real machine would give up after a while, or kick another ball
 * into play.
 */


/** The current value of the timer, which counts up from zero at the
last switch closure. */
U8 ball_search_timer;

/** The threshold for the timer at which ball search should begin */
U8 ball_search_timeout;

U8 ball_search_count;

/** The amount of time in seconds that this ball has lasted */
U16 ball_time;

/** The amount of time in seconds that this ball has lasted */
__local__ U16 game_time;

/** Returns true if the given solenoid is OK to fire during
 * a ball search.  The following should be avoided:
 * - kickout coils from ball devices
 * - the knocker
 * - flashers
 * - anything else the machine description says not to fire
 */
static bool ball_search_solenoid_ok (U8 sol)
{
	device_t *dev;

#if !defined(MACHINE_SOL_FLASHERP)
	/* If the machine description is not proper, then we can't know
	 * for sure which solenoids are OK, so don't fire _any_.  */
	return (FALSE);
#else
	if (MACHINE_SOL_FLASHERP(sol)
#ifdef MACHINE_BALL_SERVE_SOLENOID
		 || (sol == MACHINE_BALL_SERVE_SOLENOID)
#endif
#ifdef MACHINE_KNOCKER_SOLENOID
		 || (sol == MACHINE_KNOCKER_SOLENOID)
#endif
#ifdef MACHINE_LAUNCH_SOLENOID
		 || (sol == MACHINE_LAUNCH_SOLENOID)
#endif
#ifdef MACHINE_SOL_NOSEARCHP
		 || (MACHINE_SOL_NOSEARCHP(sol))
#endif
		 )
		return (FALSE);

	/* Also check for all ball device kick coils; skip them */
	for (dev=device_entry(0); dev < device_entry(NUM_DEVICES); dev++)
	{
		if (sol == dev->props->sol)
			return (FALSE);
	}
#endif

	/* OK, you can use it. */
	return (TRUE);
}


static inline void ball_search_timer_step (void)
{
	ball_search_timer++;
}


void ball_search_timer_reset (void)
{
	ball_search_timer = 0;
}


bool ball_search_timed_out (void)
{
	return (ball_search_timer >= ball_search_timeout);
}


/** Run through all solenoids to try to find a ball. */
void ball_search_run (void)
{
	U8 sol;

	ball_search_count++;

	/* Fire all solenoids.  Skip over solenoids known not to be
	pertinent to ball search.  Before starting, throw an event
	so machines can do special handling on their own. */
	callset_invoke (ball_search);

	for (sol = 0; sol < 8*4; sol++)
	{
		if (ball_search_solenoid_ok (sol))
		{
			sol_pulse (sol);
			task_sleep (TIME_200MS);
		}

		/* If a switch triggered, stop the ball search immediately */
		if (ball_search_timer == 0)
			break;
	}
}


void ball_search_run_task (void)
{
	ball_search_run ();
	task_exit ();
}


void ball_search_now (void)
{
	task_create_gid1 (GID_BALL_SEARCH_FORCE, ball_search_run_task);
}


void ball_search_timeout_set (U8 secs)
{
	ball_search_timeout = secs;
}


/** A monitor task that checks whether or not a ball search is
necessary.  This task periodically bumps a counter, which is
normally reset as scoring switches are triggered.  If the
counter reaches a threshold, and ball search is allowed to run,
then it is initiated. */
void ball_search_monitor_task (void)
{
	ball_search_timer_reset ();
	while (in_game)
	{
		task_sleep (TIME_1S);

		/* Step the ball search timer as long as a game
		 * is in progess.  But don't allow a ball search in
		 * some situations:
		 *
		 * - ball is on the shooter switch
		 * - either flipper button is held
		 */
		if (in_live_game && !in_bonus && live_balls
#ifdef MACHINE_SHOOTER_SWITCH
				&& !switch_poll_logical (MACHINE_SHOOTER_SWITCH)
#endif
				&& !switch_poll_logical (SW_LEFT_BUTTON)
				&& !switch_poll_logical (SW_RIGHT_BUTTON))
		{
			ball_time++;
			ball_search_timer_step ();
			if (ball_search_timed_out ())
			{
				ball_search_count = 0;
				while (ball_search_timer != 0)
				{
					ball_search_run ();

					/* TODO - after so many ball searches, try other
					things, like chase ball if it's enabled. */
					task_sleep_sec (15);
				}
				ball_search_count = 0;
			}
		}
	}
	task_exit ();
}


void ball_search_monitor_start (void)
{
	ball_search_timeout_set (BS_TIMEOUT_DEFAULT);
	task_create_gid1 (GID_BALL_SEARCH_MONITOR, ball_search_monitor_task);
}


void ball_search_monitor_stop (void)
{
	task_kill_gid (GID_BALL_SEARCH_MONITOR);
}


CALLSET_ENTRY (ball_search, init)
{
	ball_search_timeout_set (BS_TIMEOUT_DEFAULT);
}

CALLSET_ENTRY (ball_search, start_player)
{
	game_time = 0;
}

CALLSET_ENTRY (ball_search, start_ball)
{
	ball_time = 0;
}

CALLSET_ENTRY (ball_search, end_ball)
{
	game_time += ball_time;
	if (game_time < ball_time)
	{
		game_time = 0xFFFFUL;
	}
}


