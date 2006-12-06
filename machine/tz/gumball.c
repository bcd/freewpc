/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

/*************************************************************/
/* Gumball APIs                                              */
/*************************************************************/

void gumball_mech_activate (void)
{
}

void gumball_mech_deactivate (void)
{
}

void gumball_diverter_open (void)
{
}

void gumball_diverter_close (void)
{
}

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

void gumball_release (void)
{
	U8 timeout;

	/* TODO : multiple release requests from multiple threads are
	 * not handled */

	dbprintf ("Gumball release requested\n");
	gumball_geneva_tripped = FALSE;
	sol_on (SOL_GUMBALL_RELEASE);

	timeout = 64;
	while ((gumball_geneva_tripped == FALSE) && (--timeout > 0))
		task_sleep (TIME_33MS);

	sol_off (SOL_GUMBALL_RELEASE);
}

/*************************************************************/
/* Switch Handlers                                           */
/*************************************************************/

CALLSET_ENTRY (gumball, sw_gumball_exit)
{
	if (event_did_follow (gumball_geneva, gumball_exit))
	{
		/* A ball successfully came out of the gumball machine. */
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
	 * Increment virtual count of balls inside.
	 * Tell popper to quit retrying. */
	dbprintf ("Gumball entered.\n");
}

CALLSET_ENTRY (gumball, sw_gumball_popper)
{
	/* Wait for ball to settle, then pop
	 * ball into the gumball machine. */
	task_sleep (TIME_100MS * 5);

	dbprintf ("Pulsing popper\n");
	sol_on (SOL_POPPER);
	task_sleep (TIME_100MS);
	sol_off (SOL_POPPER);
}

void sw_gumball_right_loop_entered (void)
{
	if (gumball_load_is_enabled ())
	{
		dbprintf ("Gumball load enabled; diverter on\n");
		sol_on (SOL_GUMBALL_DIV);
	}
	else
	{
		dbprintf ("Gumball load not enabled.\n");
	}
}

CALLSET_ENTRY (gumball, sw_gumball_lane)
{
	/* Ball is approaching popper.
	 * Gumball diverter can be closed now. */
	dbprintf ("Gumball lane reached; diverter off\n");
	sol_off (SOL_GUMBALL_DIV);
	gumball_load_disable ();
}

void sw_far_left_trough_monitor (void)
{
	U8 timeout = TIME_3S / TIME_200MS;
	device_t *dev = device_entry (DEVNO_TROUGH);

	while (timeout > 0)
	{
		task_sleep (TIME_200MS);
		if ((!switch_poll_logical (SW_FAR_LEFT_TROUGH))
			|| (dev->actual_count != dev->size))
		{
			task_exit ();
		}
		timeout--;
	}

	while (ball_in_play)
		task_sleep_sec (1);

	gumball_load_from_trough ();
	task_exit ();
}


CALLSET_ENTRY (gumball, sw_far_left_trough)
{
	if (!in_test)
		task_recreate_gid (GID_FAR_LEFT_TROUGH_MONITOR, sw_far_left_trough_monitor);
}


/*************************************************************/
/* Callbacks                                                 */
/*************************************************************/

CALLSET_ENTRY (gumball, init)
{
	gumball_load_disable ();
}

