/*
 * Copyright 2008-2010 by Brian Dominy <brian@oddchange.com>
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
#include "soccer_ball.h"

enum ball_state {
	STOPPED, FORWARD, REVERSE
};

enum ball_state spinning_ball_state;


/**
 * Check the request before writing to the device driver, and tweak it
 * slightly if needed.
 *
 * Keep the device in stopped state if the ball has been disabled.
 * Reverse the polarity if that feature is enabled.
 */
void ball_spin_set (enum ball_state state)
{
	/* If the ball has been disabled, state remains
	STOPPED. */
	if (feature_config.disable_ball == YES)
		state = STOPPED;

	/* If the reverse option is set, then
	invert the meaning of forward/backward. */
	else if (feature_config.reverse_spin == YES)
	{
		if (state == FORWARD)
			state = REVERSE;
		else if (state == REVERSE)
			state = FORWARD;
	}

	dbprintf ("ball_spin = %d\n", state);

	/* Rewrite the motor outputs. */
	spinning_ball_state = state;
	switch (state)
	{
		case STOPPED:
			soccer_ball_stop ();
			break;
		case FORWARD:
			soccer_ball_start_forward ();
			break;
		case REVERSE:
			soccer_ball_start_reverse ();
			break;
	}
}


/**
 * Update the device driver if there is a new state request.
 */
static void ball_spin_change (enum ball_state state)
{
	if (state != spinning_ball_state)
	{
		ball_spin_set (state);
	}
}

/**
 * Recalculate what the spinning ball should be doing.
 * This function should be invoked periodically to keep
 * the solenoids updated.
 */
CALLSET_ENTRY (ball_spin, device_update)
{
	enum ball_state state;
	
	if (!valid_playfield)
	{
		state = STOPPED;
	}
	else if (ball_up == 1)
	{
		state = FORWARD;
	}
	else
	{
		state = REVERSE;
	}

	/* Update the motor depending on the desired state. */
	ball_spin_change (state);
}

