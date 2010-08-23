/*
 * Copyright 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/* Include autoplunger logic only when all of these machine
 * characteristics are known */
#if defined(MACHINE_LAUNCH_SWITCH) && defined(MACHINE_LAUNCH_SOLENOID) && defined(MACHINE_SHOOTER_SWITCH)
#define INCLUDE_AUTOPLUNGER
#endif

/**
 * \file
 * \brief Common plunger routines.
 */

#ifdef INCLUDE_AUTOPLUNGER
void timed_plunger_monitor (void)
{
	task_sleep_sec (7);
	launch_button_pressed ();
	task_exit ();
}
#endif


void shooter_clear_monitor (void)
{
	task_add_duration (TASK_DURATION_INF);
	task_sleep_sec (4); /* this could be machine-specific */
	global_flag_off (GLOBAL_FLAG_BALL_AT_PLUNGER);
	task_exit ();
}


void shooter_update (void)
{
#ifdef MACHINE_SHOOTER_SWITCH
	/* A ball seen on the shooter switch means it is definitely there.
	 * If not seen, it might be out of the plunger lane, or it may just be
	 * "on its way up".  During that interval, still consider the ball
	 * at the plunger, so delay clearing the flag.
	 */
	if (switch_poll_logical (MACHINE_SHOOTER_SWITCH))
	{
		task_kill_gid (GID_SHOOTER_CLEAR);
		global_flag_on (GLOBAL_FLAG_BALL_AT_PLUNGER);
	}
	else
	{
		task_create_gid1 (GID_SHOOTER_CLEAR, shooter_clear_monitor);
	}
#endif
}


CALLSET_ENTRY (plunger, valid_playfield)
{
#ifdef INCLUDE_AUTOPLUNGER
	task_kill_gid (GID_TIMED_PLUNGER_MONITOR);
#endif
	task_kill_gid (GID_SHOOTER_CLEAR);
	global_flag_off (GLOBAL_FLAG_BALL_AT_PLUNGER);

}


/**
 * Update the status of BALL_AT_PLUNGER whenever the switch toggles, or at
 * other key points in time.
 */
CALLSET_ENTRY (plunger, amode_start, start_ball, sw_shooter)
{
	shooter_update ();
}


/**
 * Called when the player presses a button intending to launch a ball into
 * play.
 */
void launch_button_pressed (void)
{
	/* Do not attempt to fire if a ball is in play and no ball is seen
	on the shooter */
	if (!valid_playfield || switch_poll_logical (MACHINE_SHOOTER_SWITCH))
		launch_ball ();
}


CALLSET_ENTRY (plunger, sw_launch_button)
{
#ifdef INCLUDE_AUTOPLUNGER
	launch_button_pressed ();
#endif
}


CALLSET_ENTRY (plunger, sw_left_button, sw_right_button)
{
#ifdef INCLUDE_AUTOPLUNGER
	if (system_config.flipper_plunger == ON)
		launch_button_pressed ();
#endif
}

