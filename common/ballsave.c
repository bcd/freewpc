/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Implement the standard ballsaver.
 */

#include <freewpc.h>

/* The default ball save time is 7 seconds.  Machines can override this. */
#ifndef MACHINE_BALL_SAVE_TIME
#define MACHINE_BALL_SAVE_TIME 7
#endif

U8 ball_save_timer;

/*
 * Describe the ball save mode
 */
struct timed_mode_ops ball_save_mode = {
	DEFAULT_MODE,
	.gid = GID_BALLSAVER_TIMER,
	.init_timer = 3,
	.timer = &ball_save_timer,
	.grace_timer = 3,
	.pause = system_timer_pause,
};


/**
 * Ball save lamp effect handler
 */
void ball_save_leff (void)
{
	for (;;)
	{
#ifdef MACHINE_BALL_SAVE_LAMP
		leff_toggle (MACHINE_BALL_SAVE_LAMP);
#endif
		task_sleep (TIME_100MS);
	}
}


/**
 * Start/extend the ballsaver.
 */
void ballsave_add_time (U8 secs)
{
	if (in_tilt)
		return;
	timed_mode_add (&ball_save_mode, secs);
}


/**
 * Disable the ballsaver.
 */
void ballsave_disable (void)
{
	timed_mode_end (&ball_save_mode);
}


/**
 * Return true if the ballsaver is active.
 */
bool ballsave_test_active (void)
{
	return timed_mode_running_p (&ball_save_mode);
}


/**
 * Return a ball back into play due to ballsave.
 */
void ballsave_launch (void)
{
#if defined(MACHINE_TZ)
	autofire_add_ball ();
#elif defined (DEVNO_TROUGH)
	device_request_kick (device_entry (DEVNO_TROUGH));
#endif
	deff_start (DEFF_BALL_SAVE);
}


/*
 * On any drain switch, extend the ball save timer so that
 * ball save doesn't timeout before endball is called.
 */

CALLSET_ENTRY (ballsave, sw_left_outlane, sw_right_outlane, sw_outhole)
{
	if (single_ball_play () && ballsave_test_active ())
	{
		/* Here, we start a separate hidden timer to indicate
		that ballsave should occur once the ball reaches the
		trough.  It should have a long expiry time just in case
		something goes wrong.  But the ballsave lamp should not
		be forced on during this period. */
		timer_restart_free (GID_BALLSAVE_EXTENDED, TIME_7S);
	}
}


/*
 * Default ballsaver is turned on as soon as valid
 * playfield is asserted.
 */

CALLSET_ENTRY (ballsave, valid_playfield)
{
#if MACHINE_BALL_SAVE_TIME > 0
	#ifdef MACHINE_TZ
	extern U8 balls_served;
	/* Don't turn on the ball saver after the first ball */
	if (!config_timed_game && balls_served < 2)
	#else
	if (!config_timed_game)
	#endif
	{
		timed_mode_begin (&ball_save_mode);
		timed_mode_reset (&ball_save_mode, MACHINE_BALL_SAVE_TIME);
	}
#endif
}

/*
 * Ball save is turned off when any multiball ends.
 */
CALLSET_ENTRY (ballsave, single_ball_play)
{
	ballsave_disable ();
}

/*
 * Ball save is activated at ball drain if it is active, or in timed
 * game when there are no balls in play.
 */
CALLSET_BOOL_ENTRY (ballsave, ball_drain)
{
	if (config_timed_game && !in_tilt && (timed_game_timer > 0) && (live_balls == 0))
	{
		ballsave_launch ();
		callset_invoke (timed_drain_penalty);
		return FALSE;
	}
	else if (timer_test_and_kill_gid (GID_BALLSAVE_EXTENDED)
		|| ballsave_test_active ())
	{
		ballsave_launch ();
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


/**
 * Update the ballsave lamp.
 */
CALLSET_ENTRY (ballsave, lamp_update)
{
#ifdef MACHINE_BALL_SAVE_LAMP
	if (timed_mode_effect_running_p (&ball_save_mode))
	{
		leff_start (LEFF_BALL_SAVE);
	}
	else
	{
		leff_stop (LEFF_BALL_SAVE);
	}
#endif
}

/* TODO - handle early ball save due to an outlane drain */

