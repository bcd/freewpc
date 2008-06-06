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

/**
 * \file
 * \brief Manage the triacs.
 * 
 * This controls the general illumination strings and some additional I/O.
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


extern __fastram__ U8 ac_zc_count;


/** The last value written to the triacs */
__fastram__ U8 triac_io_cache;

/** Which triac outputs should be enabled normally */
__fastram__ U8 triac_enables;

/** Which triac outputs are allocated to the current running lamp effect */
__fastram__ U8 triac_leff_alloc;

/** Which triac outputs should be enabled for the current lamp effect.
 * This overrides the normal triac enables. */
__fastram__ U8 triac_leff_bits[NUM_BRIGHTNESS_LEVELS];


/** Update the triacs at interrupt time */
void triac_rtt (void)
{
	U8 triac_bits;

	/* Get the default triac states */
	triac_bits = triac_enables;

	/* Override with the lamp effect states.
	This can vary according to the position of the AC wave from the
	zerocross point. */
	triac_bits &= ~triac_leff_alloc;
	triac_bits |= triac_leff_bits[ac_zc_count];

	/* Update the triac hardware */
	triac_write (triac_bits);
}


/** Turns on one or more triacs */
void triac_enable (U8 triac)
{
	log_event (SEV_INFO, MOD_TRIAC, EV_TRIAC_ON, triac);
	triac_enables |= triac;
}


/** Turns off one or more triacs */
void triac_disable (U8 triac)
{
	log_event (SEV_INFO, MOD_TRIAC, EV_TRIAC_OFF, triac);
	triac_enables &= ~triac;
}


/** Allocates one or more triacs for a lamp effect.
The leff can override the default value for the triac. */
void triac_leff_allocate (U8 triac)
{
	U8 i;
	for (i=0; i < NUM_BRIGHTNESS_LEVELS; i++)
		triac_leff_bits[i] &= ~triac;
	triac_leff_alloc |= triac;
}


/** Frees a set of triacs at the end of a lamp effect */
void triac_leff_free (U8 triac)
{
	triac_leff_alloc &= ~triac;
}


/** Enables a triac from a lamp effect at full brightness */
void triac_leff_enable (U8 triac)
{
	U8 i;
	for (i=0; i < NUM_BRIGHTNESS_LEVELS; i++)
		triac_leff_bits[i] |= triac;
}


/** Disables a triac from a lamp effect */
void triac_leff_disable (U8 triac)
{
	U8 i;
	for (i=0; i < NUM_BRIGHTNESS_LEVELS; i++)
		triac_leff_bits[i] &= ~triac;
}


/** Sets the intensity (brightness) of a single GI triac */
void triac_set_brightness (U8 triac, U8 brightness)
{
	U8 i;
	brightness = 8 - brightness;
	for (i=0; i < NUM_BRIGHTNESS_LEVELS; i++)
	{
		if (i >= brightness)
			triac_leff_bits[i] |= triac;
		else	
			triac_leff_bits[i] &= ~triac;
	}
#ifdef TRIAC_DEBUG
	dbprintf ("triac bits:");
	for (i=0; i< NUM_BRIGHTNESS_LEVELS; i++)
		dbprintf ("%02X ", triac_leff_bits[i]);
	db_puts ("\n");
	dbprintf ("ac_zc_count = %d\n", ac_zc_count);
	dbprintf ("triac_enables = %02X\n", triac_enables);
	dbprintf ("triac_io_cache = %02X\n", triac_io_cache);
#endif
}


/** Initialize the triac module */
void triac_init (void)
{
	triac_enables = 0;
	triac_leff_alloc = 0;
	memset (triac_leff_bits, 0, NUM_BRIGHTNESS_LEVELS);
	triac_write (0);
}


