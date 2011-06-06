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

/**
 * \file
 * \brief AC/zerocross circuit handling
 *
 * The zerocross circuit allows software to synchronize with the
 * AC cycle.  Alternate current produces a constantly changing
 * instantaneous voltage.  For long bursts of power, this is
 * negligible, but for short bursts, adjusting the pulse near the
 * peak of the AC cycle can produce a stronger pulse.
 *
 * This module exports a value, 'zc_timer', which is zero right
 * at the zerocross point, and increments every millisecond after.
 */

/**
 * The time since the last zerocross point, in IRQs/milliseconds.
 * This value is cleared whenever zerocross occurs and increments
 * during other polling intervals.
 */
__fastram__ U8 zc_timer;
__fastram__ U8 zc_timer_stored;

/**
 * The current status of the zerocross circuit.
 */
#ifdef CONFIG_NO_ZEROCROSS
#define zc_status ZC_BROKEN
#define zc_set_status(x)
#define ZC_MAX_PERIOD 8
#else
zc_status_t zc_status;
#define zc_set_status(x) zc_status = x
#define ZC_MAX_PERIOD 11
#endif


U8 ac_zerocross_errors;


/**
 * Real-time function that checks to see if we are currently at a
 * zero crossing point. */
void ac_rtt (void)
{
	if (unlikely (zc_status == ZC_INITIALIZING))
	{
		/* TODO - here, verify that zerocross is OK by
		 * counting the frequency of occurrences and make sure
		 * they are within range.  When good, switch to
		 * ZC_WORKING; if bad, switch to ZC_BROKEN. */
	}

	else if (likely (zc_status != ZC_BROKEN)
		&& unlikely (pinio_read_ac_zerocross ()))
	{
		/* Read the zerocross register, unless broken.
		 * If we are currently at a zero crossing,
		 * reset the timer. */
		zc_timer_stored = zc_timer;
		zc_timer = 0;
	}
	else
	{
		/* We are not at a zero crossing, or the hardware
		 * is busted. */

		/* Increment the crossing counter. */
		zc_timer++;
		if (unlikely (zc_timer > ZC_MAX_PERIOD))
		{
			/* We should have gotten a zero crossing by now, but
			we didn't.  We'll just pretend we got one anyway. */
			interrupt_dbprintf ("ZC failure?\n");
			zc_timer = 0;

			ac_zerocross_errors++;
#if 0
			if (ac_zerocross_errors < 5)
			{
				goto handle_zero_crossing;
			}
			else
			{
				/* Nope, it's really broken.  Forget about it. */
				zc_set_status (ZC_BROKEN);
			}
#endif
		}
	}
}


/** Initialize the AC detection module. */
void ac_init (void)
{
	zc_timer = 0;
	zc_timer_stored = 0;
	ac_zerocross_errors = 0;

	/* Assume working AC/zerocross for now - TODO */
	zc_set_status (ZC_WORKING);
}

