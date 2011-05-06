/*
 * Copyright 2010 by Ewan Meadows <sonny_jim@hotmail.com>
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


/* CALLSET_SECTION (outhole, __machine2__) */

#include <freewpc.h>
/* How many balls have drained in three seconds */
U8 multidrain_count;
bool powerball_death;

CALLSET_ENTRY (outhole, ball_search)
{
	if (device_recount (device_entry (DEVNO_LOCK)) + device_recount (device_entry (DEVNO_TROUGH)) == 3)
		end_ball ();
	while (switch_poll_logical (SW_OUTHOLE))
	{
		sol_request (SOL_OUTHOLE);
		task_sleep_sec (1);
	}
}

CALLSET_ENTRY (outhole, mball_start)
{
	multidrain_count = 0;
}

static void multidrain_check (void)
{
		/* Start a timer to check if 3 balls drain quickly */
		if (!timer_find_gid (GID_MULTIDRAIN) 
			&& multi_ball_play ()
			&& multidrain_count == 0)
		{
			timer_restart_free (GID_MULTIDRAIN, TIME_9S);
		}
		
		if (timer_find_gid (GID_MULTIDRAIN))
			/* There are 6 balls installed normally */
			bounded_increment (multidrain_count, feature_config.installed_balls);
}

CALLSET_ENTRY (outhole, sw_outhole)
{	
	if (in_live_game && !timer_find_gid (GID_OUTHOLE_DEBOUNCE))
	{
		timer_start_free (GID_OUTHOLE_DEBOUNCE, TIME_500MS);
		/* Whoops, lost the powerball before getting it in the gumball */
		if (!multi_ball_play () && global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY) && !ballsave_test_active ())
			powerball_death = TRUE;
		deff_start (DEFF_BALL_EXPLODE);
		leff_start (LEFF_STROBE_UP);
	}
}

CALLSET_ENTRY (outhole, dev_trough_enter)
{
	if (in_live_game)
	{
		if (ballsave_test_active ())
		{
			/* Start a timer so we grab the ball after its
			 * fired to help the player */
			timer_restart_free (GID_BALL_LAUNCH_DEATH, TIME_6S);
		}
		else
			multidrain_check ();
	}
	
}

CALLSET_ENTRY (outhole, valid_playfield)
{
	powerball_death = FALSE;
}

CALLSET_ENTRY (outhole, serve_ball)
{
	powerball_death = FALSE;
	multidrain_count = 0;
	timer_kill_gid (GID_MULTIDRAIN);
}
