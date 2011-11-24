/*
 * Copyright 2007-2011 by Brian Dominy <brian@oddchange.com>
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
 * \brief Common logic for dealing with the shooter switch and the
 * launch button.
 *
 * For the shooter, we are interested in knowing when
 * a ball is on the main shooter, and when it is not.  This is tracked
 * in the global flag BALL_AT_PLUNGER.  For the launch button, we
 * recognize it only under certain conditions, and then forward the
 * launch request via a call to launch_ball().
 */

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


CALLSET_ENTRY (shooter_switch, valid_playfield)
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
CALLSET_ENTRY (shooter_switch, amode_start, start_ball, sw_shooter)
{
	shooter_update ();
}


/**
 * Called when the player presses a button intending to launch a ball into
 * play.
 */
#ifdef INCLUDE_AUTOPLUNGER
static void launch_button_pressed (void)
{
	if (in_live_game && global_flag_test (GLOBAL_FLAG_BALL_AT_PLUNGER))
		launch_ball ();
}
#endif

#ifdef INCLUDE_AUTOPLUNGER
static void timed_plunger_monitor (void)
{
	task_sleep_sec (7);
	launch_button_pressed ();
	task_exit ();
}
#endif


CALLSET_ENTRY (launch_button, serve_ball)
{
#ifdef INCLUDE_AUTOPLUNGER
	if (config_timed_plunger == ON)
	{
		/* If timed plunger is enabled, then start a timer
		to autoplunge the ball regardless of button press */
		task_create_gid1 (GID_TIMED_PLUNGER_MONITOR, timed_plunger_monitor);
	}
#ifdef MACHINE_LAUNCH_LAMP
	lamp_flash (MACHINE_LAUNCH_LAMP);
#endif
#endif
}


CALLSET_ENTRY (launch_button, sw_launch_button)
{
#ifdef INCLUDE_AUTOPLUNGER
	launch_button_pressed ();
#endif
}


CALLSET_ENTRY (launch_button, sw_left_button, sw_right_button)
{
#ifdef INCLUDE_AUTOPLUNGER
	if (system_config.flipper_plunger == ON)
		launch_button_pressed ();
#endif
}

