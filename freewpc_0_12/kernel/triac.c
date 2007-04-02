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

/*
 * \file
 * \brief Manage the triacs, which controls the general illumination strings and
 * some additional I/O.
 *
 * Theory of operation.
 *
 * The CPU generates an IRQ once every 960 msec.
 *
 * The GI is driven off 48VAC at a frequency of 60 Hz.
 * This means that 60 times a second, the AC voltage goes through
 * a complete cycle.  AC will cross zero twice during each cycle.
 * In between each of the zero points is a peak (one negative,
 * one positive per cycle).
 *
 * We need to poll the zero cross flag periodically (at interrupt
 * time) to reconstruct the waveform of the original AC voltage.
 * The timing of zerocross may skew over time from the CPU's clock
 * so this must be constantly updated.  Then, the interrupt level
 * routine must enable the triacs at just the right time based on
 * the most recent estimate of the zero cross point, to enable
 * the desired brightness on the lamps.
 */

#include <freewpc.h>

__fastram__ U8 triac_io_cache;

/** Which triac outputs should be enabled normally */
__fastram__ U8 triac_enables;

/** Which triac outputs are allocated to the current running lamp effect */
__fastram__ U8 triac_leff_alloc;

/** Which triac outputs should be enabled for the current lamp effect.
 * This overrides the normal triac enables. */
__fastram__ U8 triac_leff_bits;

U8 gi_brightness;

/** For each GI triac, a dimming factor that controls the brightness of
 * that particular string.  The factor determines the amount of time
 * in which the triac is kept off.  A value of 0 means full brightness,
 * on all the time in which software has enabled it. */
U8 triac_dim_level[NUM_GI_TRIACS];


U8 gi_brightness_table[] =
{
	[8] = 0xFF,
	[7] = 0xFE,
	[6] = 0xFC,
	[5] = 0xF8,
	[4] = 0xF0,
	[3] = 0xE0,
	[2] = 0xC0,
	[1] = 0x80,
	[0] = 0x00,
};


void gi_recalc_zerocross ()
{
}


void triac_rtt (void)
{
#if 0
	extern U8 tick_count;
#endif
	U8 triac_bits;

	gi_recalc_zerocross ();

	triac_bits = triac_enables;
	triac_bits &= ~triac_leff_alloc;
	triac_bits |= triac_leff_bits;
#if 0
	if ((tick_count & gi_brightness_table[gi_brightness]) == tick_count)
	{
		triac_bits &= ~triac_leff_alloc;
		triac_bits |= triac_leff_bits;
	}
	else
	{
		triac_bits &= ~TRIAC_GI_MASK;
	}
#endif
	triac_write (triac_bits);
}


void triac_enable (U8 triac)
{
	triac_enables |= triac;
}


void triac_disable (U8 triac)
{
	triac_enables &= ~triac;
}


void triac_leff_allocate (U8 triac)
{
	triac_leff_bits &= ~triac;
	triac_leff_alloc |= triac;
}


void triac_leff_free (U8 triac)
{
	triac_leff_alloc &= ~triac;
}


void triac_leff_enable (U8 triac)
{
	triac_leff_bits |= triac;
}


void triac_leff_disable (U8 triac)
{
	triac_leff_bits &= ~triac;
}


void triac_set_brightness (U8 val)
{
	gi_brightness = val;
}

U8 triac_get_brightness (void)
{
	return (gi_brightness);
}

void triac_init (void)
{
	gi_brightness = 8;
	triac_enables = 0;
	triac_leff_alloc = 0;
	triac_leff_bits = 0;
	triac_write (0);
	memset (triac_dim_level, 0, NUM_GI_TRIACS);
}


