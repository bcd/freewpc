/*
 * Copyright 2006-2010 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* TODO 
 * Detect a succesful ball throw:
 * If we try to throw, but the same switch triggers in under 200ms, grab and try again
 */

#include <freewpc.h>
extern __fastram__ enum magnet_state {
	MAG_DISABLED,
	MAG_ENABLED,
	MAG_THROW_POWER,
	MAG_ON_POWER,
	MAG_ON_HOLD,
	MAG_THROW_DROP,
} left_magnet_state, upper_right_magnet_state, lower_right_magnet_state;

extern U8 left_magnet_hold_timer, lower_right_magnet_hold_timer;
extern bool lower_right_magnet_enabled_to_throw;

extern struct timed_mode_ops spiral_mode;
extern struct timed_mode_ops fastlock_mode;
extern struct timed_mode_ops hitch_mode;

extern __fastram__ bool left_magnet_enabled_to_throw, lower_right_magnet_enabled_to_throw;
extern U8 chaosmb_level;
extern U8 chaosmb_hits_to_relight;
extern U8 gumball_enable_count;

/* Whether the ball should be passed back and forth with
 * the loop magnets */
bool juggle_ball;
__fastram__ bool magnets_enabled;

/* Check to see if a magnet is enabled.
 * Currently only used by the gumball divertor code 
 * to stop it opening on a ball grab */
bool magnet_enabled (U8 magnet)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	if (magstates[magnet] != MAG_DISABLED)
		return TRUE;
	else
		return FALSE;
}

/* Check whether the magnet is busy holding/grabbing/throwing 
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

/* Check to see if we have successfully grabbed a ball */

static void monitor_left_throw_task (void)
{
	U8 i = 0;
	//callset_invoke (left_ball_throw_attempt);
	while (switch_poll_logical (SW_LEFT_MAGNET))
	{	
		i++;
		if (i > 4)
			callset_invoke (magnet_throw_left_grab_failure);
		task_sleep (TIME_66MS);
	}
	/* Give the ball a generous 2.5 seconds to make it round the loop */
	task_sleep_sec (2);
	task_sleep (TIME_500MS);
	callset_invoke (magnet_throw_left_failure); 
	task_exit ();
}


static void monitor_left_grab_task (void)
{
	callset_invoke (left_ball_grabbed);
	
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
	callset_invoke (right_ball_grabbed);
	while (switch_poll_logical (SW_LOWER_RIGHT_MAGNET))
	{
		task_sleep (TIME_66MS);
	}
	task_sleep (TIME_500MS);
	task_exit ();
}

void magnet_ball_grab_monitor_rtt (void)
{
	if (magnets_enabled)
	{
		enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
		
		if ( magstates[MAG_LEFT] == MAG_ON_HOLD
			&& switch_poll_logical (SW_LEFT_MAGNET)
			&& !task_find_gid (GID_LEFT_BALL_GRABBED))
		{
			if (left_magnet_enabled_to_throw && !task_find_gid (GID_LEFT_TO_RIGHT_THROW))
				task_recreate_gid (GID_LEFT_TO_RIGHT_THROW, monitor_left_throw_task);
			task_recreate_gid (GID_LEFT_BALL_GRABBED, monitor_left_grab_task);
		}
		
		if ( magstates[MAG_RIGHT] == MAG_ON_HOLD
			&& switch_poll_logical (SW_LOWER_RIGHT_MAGNET)
			&& !task_find_gid (GID_RIGHT_BALL_GRABBED))
		{
			task_recreate_gid (GID_RIGHT_BALL_GRABBED, monitor_right_grab_task);
		}
	}
}

/* Task to handle enabling the magnets during certain game situations */
void magnet_enable_monitor_task (void)
{
	while (magnets_enabled)
	{
		/* Lower Right magnet grabs */
		/* Catch the ball for the camera and hitch shot, don't care about gumball */
		if ((can_award_camera () 
			|| timed_mode_running_p (&hitch_mode) 
			|| pb_maybe_in_play ())
			&& (!timer_find_gid (GID_SPIRALAWARD)
			&& !timer_find_gid (GID_LOCK_KICKED)
			&& !timer_find_gid (GID_BALL_LAUNCH)
			&& !timer_find_gid (GID_LOAD_ATTEMPT)
			&& !timed_mode_running_p (&spiral_mode) 
			&& !timed_mode_running_p (&fastlock_mode)
			&& !magnet_busy (MAG_RIGHT)
			&& !task_find_gid (GID_RIGHT_BALL_GRABBED)))
		{	
			magnet_enable_catch_and_hold (MAG_RIGHT, 2);
		}
		/* Enable catch for misdetected powerball */
		if (timer_find_gid (GID_SPIRALAWARD) && global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY))
			magnet_enable_catch_and_hold (MAG_RIGHT, 2);

		/* disable catch during certain game situations */
		if ((timer_find_gid (GID_SPIRALAWARD)
			|| timer_find_gid (GID_LOCK_KICKED)
			|| timer_find_gid (GID_BALL_LAUNCH)
			|| timer_find_gid (GID_LOAD_ATTEMPT)
			|| timed_mode_running_p (&spiral_mode) 
			|| timed_mode_running_p (&fastlock_mode)
			/* Don't grab the ball if the gumball is lit and configured to do so */
			|| (feature_config.no_grab_gumball == YES && gumball_enable_count))
			/* Don't turn off the magnet if it's busy doing something else */
			&& (!magnet_busy (MAG_RIGHT) && !task_find_gid (GID_RIGHT_BALL_GRABBED)))
		{	
			magnet_disable_catch (MAG_RIGHT);
		}
			
		/* Left Magnet grabs */
		if (magnet_busy (MAG_LEFT) || task_find_gid (GID_LEFT_BALL_GRABBED)
			|| task_find_gid (GID_LEFT_TO_RIGHT_THROW))
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
			&& chaosmb_level == 2
			&& chaosmb_hits_to_relight == 0)
		{
			magnet_enable_catch (MAG_LEFT);
		}
		/* Enable catch for Spiralaward */
		else if (timer_find_gid (GID_SPIRALAWARD_APPROACHING))
		{
			magnet_enable_catch_and_throw (MAG_LEFT);
		}
		/* Catch for the shot at the power payoff */
		else if (hurryup_active ())
		{
			magnet_enable_catch (MAG_LEFT);
		}
		/* If in maybe state, turn on the magnets to help detection */
		else if (pb_maybe_in_play ())
		{
			magnet_enable_catch (MAG_LEFT);
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
		task_sleep (TIME_100MS);
	}
	task_exit ();
}

CALLSET_ENTRY (maghelper, sw_lower_right_magnet)
{
	/* Start another timer so we only enable the magnet
	 * for the spiralaward catch if it has gone round the
	 * loop the right way */
	if (timer_find_gid (GID_SPIRALAWARD))
	{
		magnet_enable_catch_and_throw (MAG_LEFT);
		timer_restart_free (GID_SPIRALAWARD_APPROACHING, TIME_2S);
	}
	if (task_kill_gid (GID_LEFT_TO_RIGHT_THROW))
	{
		/* Ball has successfully been thrown */
		sound_send (SND_ADDAMS_FASTLOCK_STARTED);
		callset_invoke (magnet_throw_left_success);
	}
	/* Check to see if powerball slips out undetected 
	 * We are expecting to grab the ball, if this doesn't
	 * happen after a certain amount of time we can say
	 * that the powerball has slipped through 
	 * Code is in powerball.c */
	if (!global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY)
		&& magnet_busy (MAG_RIGHT)
		&& single_ball_play ()
		&& !lower_right_magnet_enabled_to_throw)
		callset_invoke (check_magnet_grab);
}

CALLSET_ENTRY (maghelper, sw_left_magnet)
{
	/* Buggy, seems to conflict with the throw and misdetect a PB */
	/*
	if (!global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY)
		&& magnet_enabled (MAG_LEFT)
		&& single_ball_play ())
		callset_invoke (check_magnet_grab);
	timer_kill_gid (GID_SPIRALAWARD);
	*/
}

CALLSET_ENTRY (maghelper, start_ball)
{
	magnet_reset ();
	juggle_ball = FALSE;
	if (feature_config.tz_mag_helpers == YES)
	{
		magnets_enabled = TRUE;
		task_recreate_gid (GID_MAGNET_ENABLE_MONITOR, magnet_enable_monitor_task);
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
	magnets_enabled = FALSE;
}
