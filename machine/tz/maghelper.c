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
} left_magnet_state, upper_right_magnet_state, lower_right_magnet_state;


extern struct timed_mode_ops spiral_mode;
extern struct timed_mode_ops fastlock_mode;

CALLSET_ENTRY (maghelper, idle_every_100ms)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	
	/* Grab the ball for the camera shot, don't care about gumball */
	if (can_award_camera () && !timer_find_gid (GID_SPIRALAWARD)
		&& switch_poll (SW_LOWER_RIGHT_MAGNET)
		&& !timer_find_gid (GID_LOCK_KICKED)
		&& !timed_mode_running_p (&spiral_mode)
		&& !timed_mode_running_p (&fastlock_mode)
		&& !timer_find_gid (GID_BALL_LAUNCH)
		&& feature_config.tz_mag_helpers == YES
		&& magstates[MAG_RIGHT] != MAG_ON_HOLD
		&& magstates[MAG_RIGHT] != MAG_ON_POWER)
	{	
		magnet_enable_catch (MAG_RIGHT);
	}
	else if (timed_mode_running_p (&spiral_mode) 
		|| timed_mode_running_p (&fastlock_mode)
		|| flag_test (FLAG_CHAOSMB_RUNNING)
		|| flag_test (FLAG_SSSMB_RUNNING)
		|| timer_find_gid (GID_BALL_LAUNCH)
		|| timer_find_gid (GID_LOCK_KICKED))
	{
		magnet_disable_catch (MAG_RIGHT);
	}	
}

CALLSET_ENTRY (maghelper, start_ball)
{
	magnet_reset ();
}
