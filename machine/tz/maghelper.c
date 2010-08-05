/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (maghelper, __machine3__) */

#include <freewpc.h>
extern __fastram__ enum magnet_state {
	MAG_DISABLED,
	MAG_ENABLED,
	MAG_ON_POWER,
	MAG_ON_HOLD,
	MAG_THROW_DROP,
} left_magnet_state, upper_right_magnet_state, lower_right_magnet_state;

extern U8 left_magnet_hold_timer, lower_right_magnet_hold_timer;

extern struct timed_mode_ops spiral_mode;
extern struct timed_mode_ops fastlock_mode;
extern U8 chaosmb_level;

/* Whether the ball should be passed back and forth with
 * the loop magnets */
bool juggle_ball;

/* Check to see if a magnet is enabled.
 * Currently only used by the gumball divertor code 
 * to stop it opening on a ball grab */
bool magnet_enabled (U8 magnet)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	if (magstates[magnet] == MAG_ENABLED)
		return TRUE;
	else
		return FALSE;
}

/* Ceck whether the magnet is busy holding/grabbing/throwing 
 * but NOT enabled */
bool magnet_busy (U8 magnet)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	if (magstates[magnet] == MAG_DISABLED 
		|| magstates[magnet] == MAG_ENABLED)
		return FALSE;
	else
		return TRUE;
}

static void magnet_enable_monitor_task (void)
{
	for (;;)
	{
		/* Lower Right magnet grabs */
		/* Catch the ball for the camera shot, don't care about gumball */
				
		if (timer_find_gid (GID_SPIRALAWARD)
			|| timer_find_gid (GID_LOCK_KICKED)
			|| timer_find_gid (GID_BALL_LAUNCH)
			|| timer_find_gid (GID_LOAD_ATTEMPT)
			|| timed_mode_running_p (&spiral_mode) 
			|| timed_mode_running_p (&fastlock_mode))
		{
			magnet_disable_catch (MAG_RIGHT);
		}
		else if (can_award_camera () 
			&& !magnet_busy (MAG_RIGHT)
			&& !task_find_gid (GID_RIGHT_BALL_GRABBED))
		{	
			magnet_enable_catch_and_hold (MAG_RIGHT, 2);
		}
	
		/* Left Magnet grabs */
		if (magnet_busy (MAG_LEFT)
			|| task_find_gid (GID_LEFT_BALL_GRABBED))
		{
			/* Do nothing, magnet is busy */
		}
		/* Enable catch from an ballsave death */
		else if (task_find_gid (GID_BALL_LAUNCH_DEATH))
		{
			magnet_enable_catch_and_throw (MAG_LEFT);
		}
		/* Enable catch for Piano jackpot Shot */
		else if (global_flag_test (GLOBAL_FLAG_MB_JACKPOT_LIT))
		{
			magnet_enable_catch (MAG_LEFT);
		}
		/* Enable catch for Chaos MB when piano jackpot is lit */
		else if (global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING)
			&& chaosmb_level == 2)
		{
			magnet_enable_catch (MAG_LEFT);
		}
		/* Enable catch for Spiralaward */
		else if (timer_find_gid (GID_SPIRALAWARD_APPROACHING))
		{
			magnet_enable_catch_and_throw (MAG_LEFT);
		}
		
		/* If in maybe state, turn on the magnets to help detection */
		if (pb_maybe_in_play ())
		{
			magnet_enable_catch (MAG_LEFT);
			magnet_enable_catch (MAG_RIGHT);
		}
	
		if (juggle_ball)
		{
			if (!switch_poll_logical (SW_LOWER_RIGHT_MAGNET)
				&& !magnet_busy (MAG_RIGHT))
			{	
				magnet_enable_catch_and_throw (MAG_RIGHT);
			}
			
			if (!switch_poll_logical (SW_LEFT_MAGNET)
				&& !magnet_busy (MAG_LEFT))
			{	
				magnet_enable_catch_and_throw (MAG_LEFT);
			}
		}
		task_sleep (TIME_200MS);
	}
	task_exit ();
}

/* Check to see if the ball is still being held */
static void monitor_left_grab_task (void)
{
	while (switch_poll_logical (SW_LEFT_MAGNET))
	{
		task_sleep (TIME_66MS);
	}
	/* Sleep again so we don't get retriggered before the
	 * ball has had a chance to roll away */
	task_sleep (TIME_500MS);
	task_exit ();
}

static void monitor_right_grab_task (void)
{
	while (switch_poll_logical (SW_LOWER_RIGHT_MAGNET))
	{
		task_sleep (TIME_66MS);
	}
	task_sleep (TIME_500MS);
	task_exit ();
}

/* Check to see if we have successfully grabbed a ball */
static void magnet_ball_grab_monitor_task (void)
{
	for (;;)
	{
		enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
		if ((magstates[MAG_LEFT] == MAG_ON_HOLD)
			&& switch_poll_logical (SW_LEFT_MAGNET)
			&& !task_find_gid (GID_LEFT_BALL_GRABBED))
		{
			task_recreate_gid (GID_LEFT_BALL_GRABBED, monitor_left_grab_task);
			callset_invoke (left_ball_grabbed);
		}
		
		if ((magstates[MAG_RIGHT] == MAG_ON_HOLD)
			&& switch_poll_logical (SW_LOWER_RIGHT_MAGNET)
			&& !task_find_gid (GID_RIGHT_BALL_GRABBED))
		{
			task_recreate_gid (GID_RIGHT_BALL_GRABBED, monitor_right_grab_task);
			callset_invoke (right_ball_grabbed);
		}
		/* This sleep has to be smaller than MAG_ON_HOLD + MAG_ON_POWER time */
		task_sleep (TIME_100MS);
	}
	task_exit ();
}

/* Start another timer so we only enable the magnet
 * for the spiralaward catch if it has gone round the
 * loop the right way */
CALLSET_ENTRY (maghelper, sw_lower_right_magnet)
{
	if (timer_find_gid (GID_SPIRALAWARD))
		timer_restart_free (GID_SPIRALAWARD_APPROACHING, TIME_2S);
}

CALLSET_ENTRY (maghelper, start_ball)
{
	magnet_reset ();
	juggle_ball = FALSE;
	if (feature_config.tz_mag_helpers == YES)
	{	
		task_recreate_gid (GID_MAGNET_ENABLE_MONITOR, magnet_enable_monitor_task);
		task_recreate_gid (GID_MAGNET_BALL_GRAB_MONITOR, magnet_ball_grab_monitor_task);
	}
}

CALLSET_ENTRY (maghelper, ball_search)
{
	if (switch_poll_logical (SW_LOWER_RIGHT_MAGNET))
		magnet_enable_catch_and_throw (MAG_RIGHT);	
	if (switch_poll_logical (SW_LEFT_MAGNET))
		magnet_enable_catch_and_throw (MAG_LEFT);	
}

CALLSET_ENTRY (maghelper, end_ball)
{
	magnet_reset ();
	task_kill_gid (GID_MAGNET_ENABLE_MONITOR);
	task_kill_gid (GID_MAGNET_BALL_GRAB_MONITOR);
}
