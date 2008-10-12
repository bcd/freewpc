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

/**
 * \file
 * \brief AC/zerocross circuit handling
 *
 * This module is not yet complete.
 */

#define AC_DOMESTIC_CYCLE 17
#define AC_EXPORT_CYCLE 20

/**
 * The different states of the zerocross circuit.
 */
typedef enum {
	/** At initialization, it is unknown if the zerocross circuit is
	 * functional or not, or whether we are running on 50Hz or 60Hz
	 * AC.  In this state, do not allow any solenoids/GI to be
	 * controlled.
	 */
	ZC_INITIALIZING,

	/** Set when the zerocross circuit is known to be working OK.
	 * It can be used to perform lamp dimming and precise solenoid
	 * timing.
	 */
	ZC_WORKING,

	/** Set when the zerocross circuit is known to be broken.
	 * It will not be used anymore.
	 */
	ZC_BROKEN,
} zc_status_t;


/**
 * The time since the last zerocross point, in IRQs/milliseconds.
 * This value is cleared whenever zerocross occurs and increments
 * during other polling intervals.
 */
__fastram__ U8 zc_timer;

/**
 * The current status of the zerocross circuit.
 */
zc_status_t zc_status;


/**
 * The expected length of the AC cycle.
 *
 * Cycle length is the sum of two consecutive intervals between
 * zerocross points.
 *
 * In the US, at 60Hz AC, this should be 17.
 * Elsewhere at 50Hz AC, this should be 20.
 *
 * This value is calculated during the INITIALIZING phase
 * and is guaranteed to be nonzero once in the WORKING state.
 */
U8 ac_expected_cycle_len;

U8 ac_zerocross_errors;

#ifdef DEBUG
U8 ac_zerocross_histogram[5];
#endif


/**
 * Real-time function that checks to see if we are currently at a
 * zero crossing point. */
void ac_rtt (void)
{
	if (unlikely (zc_status == ZC_INITIALIZING))
	{
	}

	else if (unlikely (zc_status == ZC_BROKEN))
	{
	}

	/* Otherwise, we must be in the WORKING state */
	else if (unlikely (wpc_read_ac_zerocross ()))
	{
		/* We are currently at a zero crossing. */
#ifdef DEBUG
		/* Audit the time since the last zerocrossing. */
		if (zc_timer < 7)
		{
			ac_zerocross_histogram[0]++;
		}
		else if (zc_timer > 9)
		{
			ac_zerocross_histogram[4]++;
		}
		else
		{
			ac_zerocross_histogram[zc_timer-6]++;
		}
#endif

handle_zero_crossing:;
		/* Reset the timer */
		zc_timer = 0;
	}
	else
	{
		/* We are not at a zero crossing. */

		/* Increment the crossing counter. */
		zc_timer++;
		if (unlikely (zc_timer > 11))
		{
			/* We should have gotten a zero crossing by now, but
			we didn't.  We'll just pretend we got one anyway. */
			ac_zerocross_errors++;
			if (ac_zerocross_errors < 5)
			{
				goto handle_zero_crossing;
			}
			else
			{
				/* Nope, it's really broken.  Forget about it. */
				zc_status = ZC_BROKEN;
			}
		}

		/* When zerocross is not active, there is AC power
		 * and GI dimming can be done.  It makes no sense to do
		 * this at the instant the zerocross is detected.
		 *
		 * As the GI lamps only need to be pulsed roughly
		 * 1/8 of the time anyway, like the controlled lamps,
		 * we just use the zc_timer to determine at what point
		 * since zerocross to turn on the circuit.  During that
		 * 1ms interval only, enable the string; during all
		 * others, disable the string.
		 */
	}
}


void ac_init (void)
{
	zc_timer = 0;
	ac_zerocross_errors = 0;

	/* Assume working AC/zerocross for now - TODO */
	zc_status = ZC_WORKING;
	ac_expected_cycle_len = AC_DOMESTIC_CYCLE;
}

