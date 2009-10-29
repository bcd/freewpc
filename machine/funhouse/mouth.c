/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

const U8 *mouth_cmd;

U8 mouth_state;

U8 mouth_timer;

#define MC_IDLE 0
#define MC_UP 1
#define MC_DOWN 2
#define MC_PAUSE 3
#define MC_START 4

const U8 mouth_idle[] = { MC_IDLE };
const U8 mouth_step_down[] = { MC_DOWN, 1, MC_IDLE };
const U8 mouth_step_up[] = { MC_UP, 1, MC_IDLE };

void rudy_mouth_rtt (void)
{
	if (mouth_state == MC_IDLE)
		return;

	if (--mouth_timer == 0)
	{
		mouth_state = *mouth_cmd++;
		mouth_timer = *mouth_cmd++;
		switch (mouth_state)
		{
			case MC_PAUSE:
			case MC_IDLE:
				sol_disable (SOL_MOTOR_DIRECTION);
				sol_disable (SOL_MOUTH_MOTOR);
				break;
			case MC_UP:
				sol_disable (SOL_MOTOR_DIRECTION);
				sol_enable (SOL_MOUTH_MOTOR);
				break;
			case MC_DOWN:
				sol_enable (SOL_MOTOR_DIRECTION);
				sol_enable (SOL_MOUTH_MOTOR);
				break;
		}
	}
}


void rudy_mouth_command (const U8 *cmd)
{
	mouth_cmd = cmd;
	mouth_state = MC_START;
	mouth_timer = 1;
}


#if 0
CALLSETX_ENTRY (mouth, sw_left_button)
{
	if (!in_game && !in_test)
	{
		rudy_mouth_command (mouth_step_down);
	}
}

CALLSETX_ENTRY (mouth, sw_right_button)
{
	if (!in_game && !in_test)
	{
		rudy_mouth_command (mouth_step_up);
	}
}
#endif


CALLSET_ENTRY (mouth, dev_rudy_enter)
{
}


CALLSET_ENTRY (mouth, init)
{
	mouth_cmd = mouth_idle;
	mouth_state = MC_IDLE;
}

