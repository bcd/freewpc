/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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


#if defined(MACHINE_LAUNCH_SWITCH) && \
	defined(MACHINE_LAUNCH_SOLENOID) && \
	defined(MACHINE_SHOOTER_SWITCH)
#define HAVE_AUTO_SERVE
#endif

#ifndef LAUNCH_DELAY
#define LAUNCH_DELAY TIME_3S
#endif


/**
 * Returns true if the machine supports autopluging balls.
 */
static inline bool have_auto_serve_p (void)
{
#ifdef HAVE_AUTO_SERVE
	return TRUE;
#endif
	return FALSE;
}


/**
 * Serve a new ball to the manual shooter lane.
 * This function is the preferred method to serve a ball to a manual
 * plunger at the beginning of a ball and after a ball lock.
 * It is not used for autoplunges.
 */
void serve_ball (void)
{
#ifdef DEVNO_TROUGH
	valid_playfield = FALSE;
	callset_invoke (serve_ball);
	effect_update_request ();
	device_request_kick (device_entry (DEVNO_TROUGH));
#ifdef HAVE_AUTO_SERVE
	if (config_timed_plunger == ON)
	{
		/* If timed plunger is enabled, then start a timer
		to autoplunge the ball regardless of button press */
		task_create_gid1 (GID_TIMED_PLUNGER_MONITOR, timed_plunger_monitor);
	}
#endif /* HAVE_AUTO_SERVE */
#ifdef MACHINE_LAUNCH_LAMP
	lamp_flash (MACHINE_LAUNCH_LAMP);
#endif
#endif /* DEVNO_TROUGH */
}


/**
 * Activate the autolaunch mechanism.  A ball must already have
 * been served from the trough ; this just fires the launch
 * solenoid.
 */
static void launch_ball_task (void)
{
#ifdef HAVE_AUTO_SERVE
	if (!switch_poll_logical (MACHINE_SHOOTER_SWITCH))
		task_sleep (TIME_500MS);

	do {
		sol_request_async (MACHINE_LAUNCH_SOLENOID);
		task_sleep (LAUNCH_DELAY);
	} while (switch_poll_logical (MACHINE_SHOOTER_SWITCH));
#endif
	task_exit ();
}

void launch_ball (void)
{
	/* If ball launch is already in progress, do not restart it. */
	task_create_gid1 (GID_LAUNCH_BALL, launch_ball_task);
}


/**
 * Autolaunch a new ball into play from the trough.
 */
void serve_ball_auto (void)
{
#ifdef DEVNO_TROUGH
	/* Fall back to manual ball serve if there is no autoplunger. */
	if (!have_auto_serve_p ())
	{
		serve_ball ();
	}
	else
	{
		set_valid_playfield ();
#if defined(MACHINE_TZ)
		autofire_add_ball ();
#else
		device_request_kick (device_entry (DEVNO_TROUGH));
#endif
	}
#endif /* DEVNO_TROUGH */
}


/**
 * Called to add another ball into play due to ball save.
 */
void save_ball (void)
{
	serve_ball_auto ();
}


/**
 * When valid playfield is already set, the player does not have
 * to press Launch to fire the ball; it is automatic.  Schedule
 * this launch as soon as we know a ball exited the trough (we
 * do not have to see the shooter).
 */
CALLSET_ENTRY (serve, dev_trough_kick_success)
{
	if (valid_playfield)
		launch_ball ();
}


/**
 * If we see the shooter at any other time than a trough kick,
 * we will autolaunch it but not if the door is open or we are
 * in tournament mode.
 */
CALLSET_ENTRY (serve, sw_shooter)
{
	if (valid_playfield
		&& !tournament_mode_enabled
		&& !global_flag_test (GLOBAL_FLAG_COIN_DOOR_OPENED))
	{
		launch_ball ();
	}
}


CALLSET_ENTRY (serve, valid_playfield)
{
#ifdef MACHINE_LAUNCH_LAMP
	lamp_flash_off (MACHINE_LAUNCH_LAMP);
#endif
}

