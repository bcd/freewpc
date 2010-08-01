/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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
#define MAG_DRIVE_RTT_FREQ 4

#define MAG_POWER_TIME (200 / MAG_DRIVE_RTT_FREQ)
#define DEFAULT_MAG_HOLD_TIME (200 / MAG_DRIVE_RTT_FREQ)

#define DEFAULT_MAG_DROP_TIME ( 336 / MAG_DRIVE_RTT_FREQ)
#define DEFAULT_MAG_DROP_TIME_LEFT ( 376 / MAG_DRIVE_RTT_FREQ)
#define DEFAULT_MAG_DROP_TIME_RIGHT ( 328 / MAG_DRIVE_RTT_FREQ)
#define DEFAULT_MAG_THROW_TIME (24 / MAG_DRIVE_RTT_FREQ)

__fastram__ enum magnet_state {
	MAG_DISABLED,
	MAG_ENABLED,
	MAG_ON_POWER,
	MAG_ON_HOLD,
	MAG_THROW_DROP,
} left_magnet_state, upper_right_magnet_state, lower_right_magnet_state;

__fastram__ U8 left_magnet_timer, lower_right_magnet_timer;
__fastram__ U8 left_magnet_hold_timer, lower_right_magnet_hold_timer;
__fastram__ bool left_magnet_enabled_to_throw, lower_right_magnet_enabled_to_throw;

/** The magnet switch handler is a frequently called function
 * that polls the magnet switches to see if a ball is on
 * top of the magnet, and quickly turns on the magnet when
 * it senses a ball there if it has been enabled to do so. */
static inline void magnet_rtt_switch_handler (
	const U8 sw_magnet,
	const U8 sol_magnet,
	enum magnet_state *state,
	U8 *power_timer )
{
	/* rt_switch_poll is inverted because it is an opto */
	if ((*state == MAG_ENABLED) &&
		 (!rt_switch_poll (sw_magnet)))
	{
		sol_enable (sol_magnet);
		*state = MAG_ON_POWER;
		*power_timer = MAG_POWER_TIME;
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
	U8 *power_timer,
	U8 *hold_timer,
	bool *throw_enabled)
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
			/* But leave solenoid enabled so it doesn't suffer 
			 * any drop*/
			if (*power_timer == 0)
			{	
				/* switch to HOLD */
				*state = MAG_ON_HOLD;
			}
			--*power_timer;
			break;

		case MAG_ON_HOLD:
			/* keep magnet on with low power */
			/* switch should remain closed in this state */
			if (*hold_timer == 0)
			{
				if (*throw_enabled == TRUE)
				{
					*throw_enabled = FALSE;
					switch (sol_magnet)
					{
						case SOL_RIGHT_MAGNET:
							*hold_timer = DEFAULT_MAG_DROP_TIME_RIGHT;
							break;
						case SOL_LEFT_MAGNET:
							*hold_timer = DEFAULT_MAG_DROP_TIME_LEFT;
							break;
					}
					/* switch to THROW_DROP */
					sol_disable (sol_magnet);
					*state = MAG_THROW_DROP;
				}
				else
				{
					/* switch to DISABLED */
					sol_disable (sol_magnet);
					*state = MAG_DISABLED;
				}
			}
			/* Hold the ball at 50% duty or 100% if held
			 * for throw (to minimize rattle */
			else if ((*hold_timer % 2) != 0)
			{
				sol_enable (sol_magnet);
			}
			else if (*throw_enabled == TRUE)
			{
				sol_enable (sol_magnet);
			}
			else
			{
				sol_disable (sol_magnet);
			}
			/* This has to go here for some reason */
			--*hold_timer;
			break;
		
		case MAG_THROW_DROP:
			/* Short delay to let the ball roll
			 * down before applying a short pulse */
			if (*hold_timer == 0)
			{
				*power_timer = DEFAULT_MAG_THROW_TIME;
				sol_enable (sol_magnet);
				/* switch to ON_POWER but with no hold timer */
				*state = MAG_ON_POWER;
			}
			--*hold_timer;
			break;
	}
}


/* Realtime function to poll the magnet switches. */
void magnet_switch_rtt (void)
{
	magnet_rtt_switch_handler (SW_LEFT_MAGNET, SOL_LEFT_MAGNET,
		&left_magnet_state, &left_magnet_timer);
	
	magnet_rtt_switch_handler (SW_LOWER_RIGHT_MAGNET, SOL_RIGHT_MAGNET, 
		&lower_right_magnet_state, &lower_right_magnet_timer);
}


/* Realtime function to duty cycle the magnet drives */
void magnet_duty_rtt (void)
{
	magnet_rtt_duty_handler (SW_LEFT_MAGNET, SOL_LEFT_MAGNET, 
		&left_magnet_state, &left_magnet_timer, &left_magnet_hold_timer, &left_magnet_enabled_to_throw);
	
	magnet_rtt_duty_handler (SW_LOWER_RIGHT_MAGNET, SOL_RIGHT_MAGNET, 
		&lower_right_magnet_state, &lower_right_magnet_timer, &lower_right_magnet_hold_timer, &lower_right_magnet_enabled_to_throw);
}

static inline void set_mag_hold_time (U8 magnet, U8 holdtime)
{
	switch (magnet)
	{
		case MAG_LEFT:
			left_magnet_hold_timer = holdtime;
			break;
		case MAG_RIGHT:
			lower_right_magnet_hold_timer = holdtime;
			break;
	}
}

static inline void enable_magnet_throw (U8 magnet)
{
	switch (magnet)
	{
		case MAG_LEFT:
			left_magnet_enabled_to_throw = TRUE;
			break;
		case MAG_RIGHT:
			lower_right_magnet_enabled_to_throw = TRUE;
			break;
	}
}


void magnet_enable_catch (U8 magnet)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	set_mag_hold_time (magnet, DEFAULT_MAG_HOLD_TIME);	
	magstates[magnet] = MAG_ENABLED;
}

void magnet_enable_catch_and_hold (U8 magnet, U8 secs)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	
	/* Limit to 4 secs as we run at 16ms ticks in a U8 timer */
	if (secs > 2)
		secs = 2;
	set_mag_hold_time (magnet, (secs * (1000UL / MAG_DRIVE_RTT_FREQ)));
	magstates[magnet] = MAG_ENABLED;
}

void magnet_enable_catch_and_throw (U8 magnet)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	/* Hold the ball for 1 sec@100% before we throw to stabilise it*/
	set_mag_hold_time (magnet, (32  / MAG_DRIVE_RTT_FREQ));
	enable_magnet_throw (magnet);
	magstates[magnet] = MAG_ENABLED;
}

void magnet_disable_catch (U8 magnet)
{
	enum magnet_state *magstates = (enum magnet_state *)&left_magnet_state;
	/* Don't allow disable if the magnet is currently holding/grabbing 
	 * it will switch to disabled after it's done anyway */
	if (!magnet_busy (magnet))
		magstates[magnet] = MAG_DISABLED;
}

void magnet_reset (void)
{
	left_magnet_state = lower_right_magnet_state = MAG_DISABLED;
	left_magnet_timer = lower_right_magnet_timer = 0;
	left_magnet_hold_timer = lower_right_magnet_hold_timer = 0;
	left_magnet_enabled_to_throw = lower_right_magnet_enabled_to_throw = FALSE;
}

CALLSET_ENTRY (magnet, start_ball)
{
	magnet_reset ();
}

CALLSET_ENTRY (magnet, single_ball_play)
{
	magnet_reset ();
}

CALLSET_ENTRY (magnet, init)
{
	magnet_reset ();
}

