/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

bool gumball_load_enabled;

bool gumball_geneva_tripped;

U8 gumball_pending_releases;


/*************************************************************/
/* Gumball APIs                                              */
/*************************************************************/

bool gumball_load_is_enabled (void)
{
	return gumball_load_enabled;
}

void gumball_load_disable (void)
{
	gumball_load_enabled = FALSE;
}

void gumball_load_enable (void)
{
	gumball_load_enabled = TRUE;
}

void gumball_load_from_trough (void)
{
	extern void autofire_add_ball (void);

	dbprintf ("Gumball load requested\n");
	gumball_load_enable ();
	autofire_add_ball ();
}

void gumball_release_task (void)
{
	U8 timeout;

	while (gumball_pending_releases > 0)
	{
		gumball_geneva_tripped = FALSE;

		/* Timeout is 90x33ms = 3sec */
		timeout = 90;
		while ((gumball_geneva_tripped == FALSE) && (--timeout > 0))
		{
			sol_start (SOL_GUMBALL_RELEASE, SOL_DUTY_25, TIME_66MS);
			task_sleep (TIME_33MS);
		}

		sol_stop (SOL_GUMBALL_RELEASE);
		gumball_pending_releases--;
	}
	task_exit ();
}


void gumball_release (void)
{
	gumball_pending_releases++;
	task_create_gid1 (GID_GUMBALL_RELEASE, gumball_release_task);
}


/*************************************************************/
/* Switch Handlers                                           */
/*************************************************************/

CALLSET_ENTRY (gumball, sw_gumball_exit)
{
	if (event_did_follow (gumball_geneva, gumball_exit))
	{
		/* A ball successfully came out of the gumball machine. */
		/* When trying to release a ball, signal the release motor
		to stop */
	}

	event_should_follow (gumball_exit, camera, TIME_3S);
}

CALLSET_ENTRY (gumball, sw_gumball_geneva)
{
	dbprintf ("Geneva tripped.\n");
	gumball_geneva_tripped = TRUE;
	event_should_follow (gumball_geneva, gumball_exit, TIME_2S);
}


CALLSET_ENTRY (gumball, sw_gumball_enter)
{
	/* Ball has entered the gumball machine.
	 * Tell popper to quit retrying. */
	dbprintf ("Gumball entered.\n");
	task_kill_gid (GID_GUMBALL_POPPER_TASK);
}


void gumball_popper_task (void)
{
	task_sleep (TIME_500MS);
	for (;;)
	{
		if (switch_poll_logical (SW_GUMBALL_POPPER))
		{
			sol_start (SOL_POPPER, SOL_DUTY_100, TIME_200MS);
			task_sleep_sec (2);
		}
		else
		{
			break;
		}
	}
}


CALLSET_ENTRY (gumball, sw_gumball_popper)
{
	/* Wait for ball to settle, then pop ball into the gumball 
	machine.  We never keep a ball here. */
	task_create_gid1 (GID_GUMBALL_POPPER_TASK, gumball_popper_task);
}


void gumball_divertor_hold_task (void)
{
	U8 n;

	for (n=0 ; n < 12; n++)
	{
		sol_start (SOL_GUMBALL_DIV, SOL_DUTY_25, TIME_1S);
		task_sleep (TIME_500MS);
	}
	task_exit ();
}


void gumball_divertor_open (void)
{
	sol_start (SOL_GUMBALL_DIV, SOL_DUTY_100, TIME_100MS);
	task_recreate_gid_while (GID_GUMBALL_DIVERTOR_HOLD, gumball_divertor_hold_task, TASK_DURATION_INF);
}


void gumball_divertor_close (void)
{
	task_kill_gid (GID_GUMBALL_DIVERTOR_HOLD);
	sol_stop (SOL_GUMBALL_DIV);
}


void sw_gumball_right_loop_entered (void)
{
	if (gumball_load_is_enabled ())
	{
		gumball_divertor_open ();
	}
}


CALLSET_ENTRY (gumball, sw_gumball_lane)
{
	/* Ball is approaching popper.
	 * Gumball diverter can be closed now. */
	gumball_divertor_close ();
	gumball_load_disable ();
}


/* Called whenever the far left trough switch is tripped.
The sole purpose of this to determine when there are too 
many balls in the trough, and one needs to be fired into the
gumball out of the way.  If a ball remains on this switch for
3 seconds, then it is assumed there are 4 balls in the trough
and one must be loaded. */
void sw_far_left_trough_monitor (void)
{
	U8 timeout = TIME_3S / TIME_200MS;
	device_t *dev = device_entry (DEVNO_TROUGH);

 	while (task_find_gid (GID_DEVICE_PROBE))
		task_sleep_sec (1);

	dbprintf ("Far left trough check\n");
	/* Poll the switch for up to 3 seconds.  If it ever opens,
	then abort.  It must stay closed and the trough must
	remain full in order for us to continue. */
	while (timeout > 0)
	{
		task_sleep (TIME_200MS);
		if ((!switch_poll_logical (SW_FAR_LEFT_TROUGH))
			|| (dev->actual_count != dev->size))
		{
			dbprintf ("Far left trough not stable\n");
			task_exit ();
		}
		timeout--;
	}

	/* If a ball is known to be in play, then delay the
	load */
	while (valid_playfield)
	{
		dbprintf ("Far left trough : waiting to load\n");
		task_sleep_sec (1);
	}

	/* Start the load */
	dbprintf ("Far left trough stable : loading gumball\n");
	gumball_load_from_trough ();
	task_exit ();
}


CALLSET_ENTRY (gumball, sw_far_left_trough)
{
	if (!in_test)
		task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}


CALLSET_ENTRY (gumball, start_ball)
{
	task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}


CALLSET_ENTRY (gumball, amode_start)
{
	task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}


CALLSET_ENTRY (gumball, empty_balls_test)
{
	U8 count;
	for (count = 3; count > 0; --count)
		gumball_release ();
}


CALLSET_ENTRY (gumball, ball_search)
{
	/* TODO : when ball searching at game start, see if the
	extra balls are in the gumball and try to release 1. */
}


CALLSET_ENTRY (gumball, init)
{
	gumball_load_disable ();
}


