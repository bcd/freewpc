/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/* Magnet switch RTT runs every 4 ms */
#define MAG_SWITCH_RTT_FREQ 4
#define MAG_DRIVE_RTT_FREQ 32

#define MAG_POWER_TIME (100 / MAG_DRIVE_RTT_FREQ)
#define DEFAULT_MAG_HOLD_TIME (600 / MAG_DRIVE_RTT_FREQ)

__fastram__ enum magnet_state {
	MAG_DISABLED,
	MAG_ENABLED,
	MAG_ON_POWER,
	MAG_ON_HOLD,
} left_magnet_state, upper_right_magnet_state, lower_right_magnet_state;

__fastram__ U8 left_magnet_timer, upper_right_magnet_timer, lower_right_magnet_timer;
__fastram__ U8 left_magnet_hold_timer, upper_right_magnet_hold_timer, lower_right_magnet_hold_timer;

/** The magnet switch handler is a frequently called function
 * that polls the magnet switches to see if a ball is on
 * top of the magnet, and quickly turns on the magnet when
 * it senses a ball there if it has been enabled to do so. */
static inline void magnet_rtt_switch_handler (
	const U8 sw_magnet,
	const U8 sol_magnet,
	enum magnet_state *state,
	U8 *timer)
{
	/* rt_switch_poll is inverted because it is an opto */
	if ((*state == MAG_ENABLED) &&
		 (!rt_switch_poll (sw_magnet)))
	{
		/* Turn the magnet on 100% now */
		sol_enable (sol_magnet);
		/* switch to magnet on power */
		*timer = MAG_POWER_TIME;
		*state = MAG_ON_POWER;
	}
}

/** The magnet duty handler is a less-frequently called
 * function that turns on/off the magnet as necessary.
 * When a ball is being held, it uses duty cycling to avoid
 * burnout. */
static inline void magnet_rtt_duty_handler (
	const U8 sw_magnet,
	const U8 sol_magnet,
	enum magnet_state *state,
	U8 *timer, 
	U8 *hold_timer)
{
	switch (*state)
	{
		case MAG_DISABLED:
		case MAG_ENABLED:
			sol_disable (sol_magnet);
			break;

		case MAG_ON_POWER:
			/* keep magnet on with high power */
			/* switch to MAG_ON_HOLD fairly quickly though */
			/* switch should remain closed in this state */
			if (--*timer == 0)
			{
				/* switch to HOLD */
				//*timer = *hold_timer;
				*state = MAG_ON_HOLD;
			}
			else if (*timer == MAG_POWER_TIME)
			{
				/* magnet is on 100% */
				sol_enable (sol_magnet);
			}
			break;

		case MAG_ON_HOLD:
			if (--*hold_timer == 0)
			{	
				sol_disable (sol_magnet);
				/* switch to DISABLED */
				*hold_timer = 0;
				*state = MAG_DISABLED;
			}
			else
			{
				/* magnet is on 25% */
				if ((*hold_timer % 4) == 0)
				{
					sol_enable (sol_magnet);
				}
				else
				{
					sol_disable (sol_magnet);
				}
			}
			break;
	}
}

/* Realtime function to poll the magnet switches. */
void magnet_switch_rtt (void)
{
	magnet_rtt_switch_handler (SW_LEFT_MAGNET,
		SOL_LEFT_MAGNET, &left_magnet_state, &left_magnet_timer);
	
	magnet_rtt_switch_handler (SW_UPPER_RIGHT_MAGNET,
		SOL_UPPER_RIGHT_MAGNET, &upper_right_magnet_state, &upper_right_magnet_timer);
	
	magnet_rtt_switch_handler (SW_LOWER_RIGHT_MAGNET,
		SOL_RIGHT_MAGNET, &lower_right_magnet_state, &lower_right_magnet_timer);
}

/* Realtime function to duty cycle the magnet drives */
void magnet_duty_rtt (void)
{
	magnet_rtt_duty_handler (SW_LEFT_MAGNET, SOL_LEFT_MAGNET, 
		&left_magnet_state, &left_magnet_timer, &left_magnet_hold_timer);
	
	magnet_rtt_duty_handler (SW_UPPER_RIGHT_MAGNET, SOL_UPPER_RIGHT_MAGNET, 
		&upper_right_magnet_state, &upper_right_magnet_timer, &upper_right_magnet_hold_timer);
	
	magnet_rtt_duty_handler (SW_LOWER_RIGHT_MAGNET, SOL_RIGHT_MAGNET, 
		&lower_right_magnet_state, &lower_right_magnet_timer, &lower_right_magnet_hold_timer);
}


void magnet_enable_catch (U8 magnet)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	
	switch (magnet)
	{	
		case MAG_LEFT:
			left_magnet_hold_timer = DEFAULT_MAG_HOLD_TIME;
			break;
		case MAG_UPPER_RIGHT:
			upper_right_magnet_hold_timer = DEFAULT_MAG_HOLD_TIME;
			break;
		case MAG_RIGHT:
			lower_right_magnet_hold_timer = DEFAULT_MAG_HOLD_TIME;
			break;
	}
	magstates[magnet] = MAG_ENABLED;
}

void magnet_enable_catch_and_hold (U8 magnet, U8 holdtime_secs)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	
	/* Limit to 8 seconds */
	if (holdtime_secs > 8)
		holdtime_secs = 8;
	
	U8 holdtime = (holdtime_secs*31.25);
	switch (magnet)
	{	
		case MAG_LEFT:
			left_magnet_hold_timer = 254;
			break;
		case MAG_UPPER_RIGHT:
			upper_right_magnet_hold_timer = 254;
			break;
		case MAG_RIGHT:
			lower_right_magnet_hold_timer = 254;
			break;
	}
	magstates[magnet] = MAG_ENABLED;
}

void magnet_disable_catch (U8 magnet)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	magstates[magnet] = MAG_DISABLED;
}

void magnet_reset (void)
{
	left_magnet_state = upper_right_magnet_state = 
		lower_right_magnet_state = MAG_DISABLED;

	left_magnet_timer = upper_right_magnet_timer = 
		lower_right_magnet_timer = 0;

	left_magnet_hold_timer = upper_right_magnet_hold_timer =
		lower_right_magnet_hold_timer = 0;
}

CALLSET_ENTRY (magnet, idle_every_100ms)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	if (magstates[MAG_LEFT] == MAG_ON_HOLD 
		&& switch_poll_logical (SW_LEFT_MAGNET)
		&& !timer_find_gid (GID_LEFT_BALL_GRABBED))
	{
		callset_invoke (left_ball_grabbed);
		callset_invoke (ball_grabbed);
		timer_restart_free (GID_LEFT_BALL_GRABBED, (left_magnet_hold_timer * 2));
	}
		
	if (magstates[MAG_RIGHT] == MAG_ON_HOLD 
		&& switch_poll_logical (SW_LOWER_RIGHT_MAGNET)
		&& !timer_find_gid (GID_RIGHT_BALL_GRABBED))
	{
		callset_invoke (right_ball_grabbed);
		callset_invoke (ball_grabbed);
		timer_restart_free (GID_RIGHT_BALL_GRABBED, (lower_right_magnet_hold_timer * 2));
	}
}

//TODO MOVE THIS
/* Grab the ball following an auto fire */
CALLSET_ENTRY (magnet, sw_lower_right_magnet)
{
	if (in_live_game && task_find_gid (GID_BALL_LAUNCH))
	{
		magnet_enable_catch_and_hold (MAG_LEFT, 8);
		task_sleep_sec (1);
		magnet_disable_catch (MAG_LEFT);
	}
}

CALLSET_ENTRY (magnet, end_ball)
{
}

CALLSET_ENTRY (magnet, start_ball)
{
	magnet_reset ();
}

CALLSET_ENTRY (magnet, single_ball_play)
{
	magnet_reset ();
}

CALLSET_ENTRY (magnet, ball_search)
{
	magnet_enable_catch (MAG_LEFT);
	magnet_enable_catch (MAG_RIGHT);
	task_sleep_sec (1);
	magnet_reset ();
}

CALLSET_ENTRY (magnet, init)
{
	magnet_reset ();
}
