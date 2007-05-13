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

#ifndef _RTSOL_H
#define _RTSOL_H

/* Code for doing real-time solenoids.
 * These are handled directly by polling the switches and
 * then driving the appropriate solenoids.
 */

#define RTSOL_ACTIVE_HIGH	0xff
#define RTSOL_ACTIVE_LOW	0x0

#define RTSW_ACTIVE_HIGH	0xff
#define RTSW_ACTIVE_LOW		0x0


extern inline void rt_sol_off (U8 *sol_cache, U8 bitmask, U8 active_high)
{
	if (active_high == RTSOL_ACTIVE_HIGH)
	{
		*sol_cache &= ~bitmask;
	}
	else
	{
		*sol_cache |= bitmask;
	}
}

extern inline void rt_sol_on (U8 *sol_cache, U8 bitmask, U8 active_high)
{
	if (active_high == RTSOL_ACTIVE_HIGH)
	{
		*sol_cache |= bitmask;
	}
	else
	{
		*sol_cache &= ~bitmask;
	}
}

extern inline U8 rt_sol_active (U8 *sol_cache, U8 bitmask, U8 active_high)
{
	return (*sol_cache & bitmask);
}


extern inline void rt_sol_enable (U8 sol)
{
	rt_sol_on (sol_rt_state + (sol / 8), 1 << (sol % 8), RTSOL_ACTIVE_HIGH);
}


extern inline void rt_sol_disable (U8 sol)
{
	rt_sol_off (sol_rt_state + (sol / 8), 1 << (sol % 8), RTSOL_ACTIVE_HIGH);
}


/* Each rt solenoid is governed by a rt_sol_state object,
 * which tracks the current status of that solenoid.
 * The state is an 8-bit value divided into two parts:
 * 1-bit for whether the solenoid is in its on/off phase,
 * and 7-bits for the timer coutdown/countup.
 */
extern inline void rt_solenoid_update1 (
	U8 *sol_cache,
	const U8 sol_bitmask,
	const U8 sol_active_high,
	U8 *sw_cache,
	const U8 sw_bitmask,
	const U8 sw_active_high,
	S8 *rt_sol_state,
	const U8 sol_on_irqs,
	const U8 sol_off_irqs )
{
	/* Declaring state as a local register variable improves the performance.
	 * 'a' can be used here safely since 'D' is not. */
	register S8 state __areg__;

	state = *rt_sol_state;
	
	if (!in_live_game)
	{
		/* Solenoid is never pulsed except during a game */
		rt_sol_off (sol_cache, sol_bitmask, sol_active_high);
	}
	else if (state == 0)
	{
		/* Solenoid is idle - normal case */
		/* Only here are allowed to poll the switch */
		if (rt_sol_active (sw_cache, sw_bitmask, sw_active_high))
		{
			/* Yes, the switch is active, so the solenoid can
			 * be scheduled to activate now */
			*rt_sol_state = sol_on_irqs;
		}
	}
	else if (state < 0)
	{
		/* Solenoid is in its off-phase */
		rt_sol_off (sol_cache, sol_bitmask, sol_active_high);
		(*rt_sol_state)++;
	}
	else if (state > 0)
	{
		/* Solenoid is in its on-phase */
		rt_sol_on (sol_cache, sol_bitmask, sol_active_high);
		(*rt_sol_state)--;
		if (*rt_sol_state == 0)
		{
			*rt_sol_state = -sol_off_irqs;
		}
	}
}


/**
 * rt_solenoid_update is the main interface for realtime drivers like
 * slingshots and jet bumpers.  It will poll the switch of the device
 * to see if it's being actuated, and then begin a short phase pulse
 * to the solenoid, followed by a minimum off-phase.  It then goes back
 * to polling the switch.
 *
 * The switch & solenoid are parameterized so that this macro may be
 * used in multiple places (at the cost of duplicating the code).
 * The on/off phase time and the switch/solenoid polarity can also be
 * given (in case the switch is an opto, or the drive goes from on to off?)
 */
extern inline void rt_solenoid_update (
	S8 *rt_sol_state,
	const U8 sol_num,
	const U8 sol_active_high,
	const U8 sw_num,
	const U8 sw_active_high,
	const U8 sol_on_irqs,
	const U8 sol_off_irqs )
{
	rt_solenoid_update1 (
		(U8 *)&sol_rt_state + (sol_num / 8), 1 << (sol_num % 8),
		sol_active_high,
		&switch_bits[0][0] + (sw_num / 8), 1 << (sw_num % 8),
		sw_active_high, 
		rt_sol_state,
		sol_on_irqs, sol_off_irqs );
}

#endif /* _RTSOL_H */

/* vim: set ts=3: */
