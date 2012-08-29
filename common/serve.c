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
#include <search.h>

/**
 * \file
 * \brief Logic for serving balls from the trough.
 *
 * This module implements ball serving, both for manual and auto plunges.
 * The two main APIs exposed here are serve_ball() and serve_ball_auto().
 * These add a single ball to play, either at the manual or auto plunger.
 * There is also set_ball_count() which attempts to set the global ball
 * count to an arbitrary number.
 */


/* Use HAVE_AUTO_SERVE around code which deals with autoplunger hardware.
Such code will not compile on machines where there is no such thing. */
#if defined(MACHINE_LAUNCH_SWITCH) && \
	defined(MACHINE_LAUNCH_SOLENOID) && \
	defined(MACHINE_SHOOTER_SWITCH)

#define HAVE_AUTO_SERVE

/* When using an autoplunger, LAUNCH_DELAY says how much time to wait
after launching a ball, before trying to launch another one (could be
another ball served, or the same ball which failed to launch OK). */
#ifndef LAUNCH_DELAY
#define LAUNCH_DELAY TIME_3S
#endif

#endif

/* Indicates how many balls we want to be in play.  Eventually,
live_balls == live_balls_wanted if everything works OK. */
U8 live_balls_wanted;


/**
 * Returns true if the machine supports autoplunging balls.
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
 * Autolaunch a new ball into play from the trough.  This is the
 * preferred API to use by ballsavers.
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
		/* TZ's autoplunger is a little different, so it is handled
		specially. */
#if defined(MACHINE_TZ)
		autofire_add_ball ();
#else
		device_request_kick (device_entry (DEVNO_TROUGH));
#endif
	}
#endif /* DEVNO_TROUGH */
}


/**
 * A background task that attempts to set the number of balls
 * in play to 'live_balls_wanted'.
 */
#ifdef DEVNO_TROUGH
static void set_ball_count_task (void)
{
	device_t *dev = device_entry (DEVNO_TROUGH);
	U8 max_live_balls;
	U8 retries;

	/* While we are launching balls, we monitor 'live_balls' to
	check that it is going up. */
	max_live_balls = live_balls;

	/* Set the number of times we will attempt to kick a ball.
	This is the number of balls that need to be added to play,
	plus 2 to handle errors.  After this, we give up. */
	retries = live_balls_wanted - max_live_balls + 2;

	while (retries && max_live_balls < live_balls_wanted)
	{
		retries--;

		/* Are there enough balls in the trough to satisfy another
		kick request?  If not, then we need to add the balls from
		somewhere else.  This is machine-specific. */
		if (dev->actual_count < dev->kicks_needed)
		{
			callset_invoke (trough_rescue);
		}
		else
		{
			serve_ball_auto ();
		}

		/* Wait a bit for the ball to make it to the shooter lane. */
		task_sleep (TIME_2S + TIME_500MS);

		/* As long as there is a ball on the shooter, wait before trying
		to continue.  This flag will clear once the shooter switch
		has cleared for a few seconds. */
		while (global_flag_test (GLOBAL_FLAG_BALL_AT_PLUNGER))
			task_sleep (TIME_133MS);

		/* See if the ball count went up, indicating success */
		if (live_balls > max_live_balls)
			max_live_balls = live_balls;
	}
	task_exit ();
}


/**
 * Set the total number of balls in play to COUNT.
 */
void set_ball_count (U8 count)
{
	if (count <= live_balls)
		return;
	live_balls_wanted = count;
	task_recreate_gid (GID_SET_BALL_COUNT, set_ball_count_task);
}


/**
 * Add COUNT balls into play from the trough.
 */
void add_ball_count (U8 count)
{
	set_ball_count (live_balls + count);
}
#endif


/**
 * If a ball is already on the shooter, or a previous autolaunch
 * request is still in progress, then delay kicking further balls.
 */
CALLSET_BOOL_ENTRY (serve, dev_trough_kick_request)
{
	/* TODO - ball at plunger should block ALL kick requests? */
	if (global_flag_test (GLOBAL_FLAG_BALL_AT_PLUNGER))
		return FALSE;
	else if (task_find_gid (GID_LAUNCH_BALL))
		return FALSE;
	else
		return TRUE;
}


/**
 * When valid playfield is already set, the player does not have
 * to press Launch to fire the ball; it is automatic.  Schedule
 * this launch as soon as we know a ball exited the trough (we
 * do not have to see the shooter).
 */
CALLSET_ENTRY (serve, dev_trough_kick_success)
{
#ifdef HAVE_AUTO_SERVE
	if (valid_playfield)
		launch_ball ();
#endif
}


/**
 * If we see the shooter at any other time than a trough kick,
 * we will autolaunch it but not if the door is open or we are
 * in tournament mode.
 */
CALLSET_ENTRY (serve, sw_shooter)
{
#ifdef MACHINE_SHOOTER_SWITCH
	if (!switch_poll_logical (MACHINE_SHOOTER_SWITCH))
		return;
	ball_search_timer_reset ();
	if (valid_playfield
		&& !tournament_mode_enabled
		&& !global_flag_test (GLOBAL_FLAG_COIN_DOOR_OPENED))
	{
		/* TODO - this might be game specific.  For example, Simpsons Pinball
		Party would give you a manual skill shot here except during
		multiball. */
		launch_ball ();
	}
#endif
}


CALLSET_ENTRY (serve, valid_playfield)
{
#ifdef MACHINE_LAUNCH_LAMP
	/* TODO - where is the lamp turned ON? */
	lamp_flash_off (MACHINE_LAUNCH_LAMP);
#endif
}

