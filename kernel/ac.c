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

__fastram__ U8 ac_zc_count;

__fastram__ U8 ac_cycle_complete;


/**
 * Real-time function that checks to see if we are currently at a
 * zero crossing point.  In the US, zerocrossing occurs 60 times
 * per second, or once every 17.07 IRQs.
 */
void ac_rtt (void)
{
	if (unlikely (wpc_read_ac_zerocross ()))
	{
		/* We are currently at a zero crossing. */
handle_zero_crossing:;

		/* Clear the counter which indicates the number
		of IRQs since the last crossing. */
		ac_zc_count = 0;

		/* Two crossings = 1 AC cycle, so do this every
		other time */
		if (ac_cycle_complete)
		{
			/* Update outputs */

			/* Update duty cycle mask */
		}
		ac_cycle_complete = ~ac_cycle_complete;
	}
	else
	{
		/* We are not at a zero crossing. */

		/* Don't refresh any outputs here */

		/* Increment the crossing counter. */
		ac_zc_count++;
		if (unlikely (ac_zc_count > 9))
		{
			/* We should have gotten a zero crossing by now, but
			we didn't.  We'll just pretend we got one anyway,
			to allow the coils to be refreshed. */
			goto handle_zero_crossing;
		}
	}
}


void ac_init (void)
{
	ac_zc_count = 0;
	ac_cycle_complete = 0;
}

