
/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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


U8 ball_search_timer;
U8 ball_search_timeout;


/** Returns true if the given solenoid is OK to fire during
 * a ball search.  The following should be avoided:
 * - kickout coils from ball devices
 * - the knocker
 * - flashers
 */
static bool ball_search_solenoid_ok (U8 sol)
{
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
	/* TODO - add check for all ball device kick coils */
#endif

	/* OK, you can use it. */
	return (TRUE);
}


static void ball_search_timer_step (void)
{
	ball_search_timer++;
}


void ball_search_timer_reset (void)
{
	ball_search_timer = 0;
}


static bool ball_search_timed_out (void)
{
	return (ball_search_timer >= ball_search_timeout);
}


static void ball_search_run (void)
{
	U8 sol;

	dbprintf ("Ball search triggered\n");

	/* Fire all solenoids */
	/* Skip over solenoids known not to be pertinent to ball
	 * search, and others defined by the machine description */
	for (sol = 0; sol < 8*4; sol++)
	{
		if (ball_search_solenoid_ok (sol))
		{
			sol_pulse (sol);
			task_sleep (TIME_100MS * 1);
		}

		/* If a switch triggered, stop the ball
		 * search immediately */
		if (ball_search_timer == 0)
			break;
	}
}


void ball_search_timeout_set (U8 secs)
{
	ball_search_timeout = secs;
}


void ball_search_monitor_task (void)
{
	extern U8 live_balls;

	for (;;)
	{
		task_sleep_sec (1);

		/* Step the ball search timer as long as a game
		 * is in progess. */
		if (in_live_game 
				&& !in_bonus 
				&& live_balls 
				&& !switch_poll_logical (MACHINE_SHOOTER_SWITCH)
				&& !switch_poll_logical (SW_LL_FLIP_SW) 
				&& !switch_poll_logical (SW_LR_FLIP_SW))
		{
			ball_search_timer_step ();
			if (ball_search_timed_out ())
			{
				while (ball_search_timer != 0)
				{
					ball_search_run ();
					task_sleep_sec (15);
				}
			}
		}
	}
}


void ball_search_monitor_start (void)
{
	task_create_gid1 (GID_BALL_SEARCH_MONITOR, ball_search_monitor_task);
}


void ball_search_monitor_stop (void)
{
	task_kill_gid (GID_BALL_SEARCH_MONITOR);
}


void ball_search_init (void)
{
	ball_search_timeout_set (BS_TIMEOUT_DEFAULT);
}

