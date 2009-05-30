/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

enum ball_state {
	STOPPED, FORWARD, REVERSE
};

enum ball_state spinning_ball_state;


/**
 * Update the soccer ball spinning.
 */
void ball_spin_set (enum ball_state state)
{
	spinning_ball_state = state;

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

	/* Rewrite the motor outputs. */
	switch (state)
	{
		case STOPPED:
			sol_stop (SOL_BALL_FORWARD);
			sol_stop (SOL_BALL_REVERSE);
			break;
		case FORWARD:
			sol_start (SOL_BALL_FORWARD, SOL_DUTY_50, TIME_1S);
			sol_stop (SOL_BALL_REVERSE);
			break;
		case REVERSE:
			sol_start (SOL_BALL_REVERSE, SOL_DUTY_50, TIME_1S);
			sol_stop (SOL_BALL_FORWARD);
			break;
	}
}


void ball_spin_change (enum ball_state state)
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
void ball_spin_update (void)
{
	enum ball_state state = STOPPED;

	/* Update the motor depending on the new state. */
	ball_spin_change (state);
}

CALLSETX_ENTRY (ball_driver, init)
{
	ball_spin_set (STOPPED);
}

CALLSETX_ENTRY (ball_driver, end_game, tilt, stop_game)
{
	ball_spin_change (STOPPED);
}

