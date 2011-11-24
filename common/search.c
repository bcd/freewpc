/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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
 */


/** The current value of the timer, which counts up from zero at the
last switch closure. */
U8 ball_search_timer;

/** The threshold for the timer at which ball search should begin */
U8 ball_search_timeout;

U8 ball_search_count;

/** The amount of time in seconds that this ball has lasted */
U16 ball_time;

/** The amount of time in seconds that this game has lasted.
This is a per-player variable */
__local__ U16 game_time;


/** Returns true if the chase ball feature is enabled.
 * When true, after 5 unsuccessful ball searches, all balls in play
 * are marked as missing, and endball is called.
 */
static bool chase_ball_enabled (void)
{
	return system_config.allow_chase_ball == YES;
}


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
#endif

#if defined(MACHINE_SOLENOID_P)
	/* If it's not a valid solenoid number, don't allow it.  This
	skips coils that are not installed. */
	if (!MACHINE_SOLENOID_P (sol))
		return FALSE;
#endif

	/* Skip known coils which should always be skipped. */
	if (MACHINE_SOL_FLASHERP(sol)
#ifdef MACHINE_BALL_SERVE_SOLENOID
		 || (sol == MACHINE_BALL_SERVE_SOLENOID)
#endif
#ifdef MACHINE_KNOCKER_SOLENOID
		 || (sol == MACHINE_KNOCKER_SOLENOID)
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
		{
			/* This coil controls a ball device. */

			/* If there are no balls detected here, pulse it */
			if (dev->actual_count == 0)
				return TRUE;

			/* If chase ball is turned off, then during the 5th ball search,
			pulse it */
			if (!chase_ball_enabled () && ball_search_count == 5)
				return (TRUE);

			/* Default is NOT to fire such a coil */
			return (FALSE);
		}
	}

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
	dbprintf ("Ball search %d\n", ball_search_count);

	/* Fire all solenoids.  Skip over solenoids known not to be
	pertinent to ball search.  Before starting, throw an event
	so machines can do special handling on their own. */
	callset_invoke (ball_search);
	task_sleep (TIME_200MS);

	for (sol = 0; sol < NUM_POWER_DRIVES; sol++)
	{
		if (ball_search_solenoid_ok (sol))
		{
			sol_request_async (sol);
			task_sleep (TIME_200MS);
		}

		/* If a switch triggered, stop the ball search immediately */
		if (ball_search_timer == 0)
			break;
	}
	callset_invoke (ball_search_end);
}


/** Runs a ball search in a separate task context. */
void ball_search_run_task (void)
{
	ball_search_run ();
	task_exit ();
}


/** Start a ball search in a separate task context. */
void ball_search_now (void)
{
	task_create_gid1 (GID_BALL_SEARCH_FORCE, ball_search_run_task);
}


/** Set the amount of time in seconds before the next ball search will
occur. */
void ball_search_timeout_set (U8 secs)
{
	ball_search_timeout = secs;
}


/** A monitor task that checks whether or not a ball search is
necessary.  This task periodically bumps a counter, which is
normally reset as scoring switches are triggered.  If the
counter reaches a threshold, and ball search is allowed to run,
then it is initiated.
	This task is also responsible for incrementing the ball time
statistic, when ball search is not necessary. */
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
		if (in_game && !in_test && !in_bonus && (live_balls || !valid_playfield)
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
					if ((ball_search_count >= 5) && chase_ball_enabled ())
					{
						/* If chase ball is enabled, after the 5th ball search
						we will force endball. */
						audit_increment (&system_audits.chase_balls);
						end_ball ();
						return;
					}
					else
					{
						/* Perform a ball search */
						ball_search_run ();
					}

					/* After the third ball search, cancel the tilt lamp
					effect, to help the player find the missing ball. */
					if (ball_search_count == 3)
						leff_stop (LEFF_TILT);


					if (ball_search_count < 10)
					{
						/* Delay a small amount for the first few ball searches */
						task_sleep_sec (12);
					}
					else
					{
						/* Delay longer after many ball searches */
						task_sleep_sec (20);
					}

					/* After a while, just give up -- but don't do that in tournament
					mode or on free play; this is just to keep a location game from cycling
					continuously. */
					if (ball_search_count >= 25 &&
							!price_config.free_play && !system_config.tournament_mode)
					{
						fatal (ERR_BALL_SEARCH_TIMEOUT);
					}
				}

				/* A ball was seen -- clear the counter and exit.  Also refresh devices
				right away */
				ball_search_count = 0;
				callset_invoke (device_update);
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

/*
 * Initialize the total game time statistic per player.
 */
CALLSET_ENTRY (ball_search, start_player)
{
	game_time = 0;
}

/*
 * At the beginning of a ball, initialize the ball time
 * statistic.  This variable will count up during the course of
 * the ball, in seconds.
 */
CALLSET_ENTRY (ball_search, start_ball)
{
	ball_time = 0;
}

/*
 * At the end of a ball, add the ball time to the
 * total time for the player.  Take care that it doesn't
 * overflow (unlikely, but still).
 */
CALLSET_ENTRY (ball_search, end_ball)
{
	game_time += ball_time;
	if (game_time < ball_time)
	{
		game_time = 0xFFFFUL;
	}
}

/*
 * At the end of the game for each player, log their
 * game time into the histograms.  A game is only considered
 * complete at the start of the last ball.
 */
CALLSET_ENTRY (ball_search, end_player)
{
	if (ball_up == system_config.balls_per_game)
		score_histogram_add (game_time);
}

