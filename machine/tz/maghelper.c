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

//extern U8 left_magnet_hold_timer, lower_right_magnet_hold_timer;

extern struct timed_mode_ops spiral_mode;
extern struct timed_mode_ops fastlock_mode;
extern U8 chaosmb_level;

/* Check to see if the magnet is OK to be enabled */
static bool magnet_not_busy (U8 magnet)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	if (feature_config.tz_mag_helpers == YES
		&& magstates[magnet] == MAG_DISABLED)
	{
		return TRUE;
	}
	else
		return FALSE;
}

/* Check to see if a magnet is enabled or busy */
bool magnet_enabled (U8 magnet)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	if (magstates[magnet] != MAG_DISABLED)
		return TRUE;
	else
		return FALSE;
}

CALLSET_ENTRY (maghelper, idle_every_100ms)
{
	//enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;

	/* Check to see if we have grabbed a ball
	 * This will only work as long as MAG_ON_HOLD
	 * is more than 100ms */
#if 0
	if (magstates[MAG_LEFT] == MAG_ON_HOLD
		&& switch_poll_logical (SW_LEFT_MAGNET)
		&& !timer_find_gid (GID_LEFT_BALL_GRABBED))
	{
		timer_restart_free (GID_LEFT_BALL_GRABBED, left_magnet_hold_timer);
		callset_invoke (left_ball_grabbed);
	}
	
	if (magstates[MAG_RIGHT] == MAG_ON_HOLD
		&& switch_poll_logical (SW_LOWER_RIGHT_MAGNET)
		&& !timer_find_gid (GID_RIGHT_BALL_GRABBED))
	{
		timer_restart_free (GID_RIGHT_BALL_GRABBED, lower_right_magnet_hold_timer);
		callset_invoke (right_ball_grabbed);
	}

#endif	
	/* Lower Right magnet grabs */
	/* Catch the ball for the camera shot, don't care about gumball */
	
	if (!switch_poll_logical (SW_LOWER_RIGHT_MAGNET) && magnet_not_busy (MAG_RIGHT))
	{	
		magnet_enable_catch_and_throw (MAG_RIGHT);
	}
	#if 0
	if (can_award_camera () 
		&& magnet_not_busy (MAG_RIGHT)
		&& !timer_find_gid (GID_SPIRALAWARD)
		&& !switch_poll_logical (SW_LOWER_RIGHT_MAGNET)
		&& !timer_find_gid (GID_LOCK_KICKED)
		&& !timer_find_gid (GID_BALL_LAUNCH))
	{	
		//magnet_enable_catch_and_hold (MAG_RIGHT, 1);
	}
	else if (timed_mode_running_p (&spiral_mode) 
		|| timed_mode_running_p (&fastlock_mode)
		|| flag_test (FLAG_CHAOSMB_RUNNING)
		|| flag_test (FLAG_SSSMB_RUNNING)
		|| timer_find_gid (GID_BALL_LAUNCH)
		|| timer_find_gid (GID_LOCK_KICKED))
	{
		//magnet_disable_catch (MAG_RIGHT);
	}
	#endif
	if (!switch_poll_logical (SW_LEFT_MAGNET) && magnet_not_busy (MAG_LEFT))
	{	
		magnet_enable_catch_and_throw (MAG_LEFT);
	}
	#if 0
	/* Left Magnet grabs */
	/* Enable catch from an autofire launch */
	if (task_find_gid (GID_DEATH_BALL_LAUNCH)
		&& magnet_not_busy (MAG_LEFT))
	{
		//magnet_enable_catch_and_hold (MAG_LEFT, 4);
	}
	/* Enable catch for Piano jackpot Shot */
	else if (flag_test (FLAG_MB_JACKPOT_LIT)
		&& magnet_not_busy (MAG_LEFT))
	{
		//magnet_enable_catch (MAG_LEFT);
	}
	else if (flag_test (FLAG_CHAOSMB_RUNNING)
		&& chaosmb_level == 2
		&& magnet_not_busy (MAG_LEFT))
	{
		//magnet_enable_catch (MAG_LEFT);
	}
	else
	{
		//magnet_disable_catch (MAG_LEFT);
	}
	#endif
}

#if 0
{
	if (task_kill_gid (GID_BALL_LAUNCH_DEATH))
	{
		timer_restart_free (GID_DEATH_BALL_LAUNCH, TIME_2S);
	}
}

#endif

CALLSET_ENTRY (maghelper, start_ball)
{
	magnet_reset ();
}
