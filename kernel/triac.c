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

/**
 * \file
 * \brief Manage the triacs.
 *
 * This controls the general illumination strings and some additional I/O.
 *
 * Theory of operation:
 *
 * The CPU generates an IRQ once every 960 microseconds.
 *
 * The GI is driven off 6.3VAC at a frequency of 60 Hz (US).
 * This means that 60 times a second, the sinusoidal AC voltage goes
 * through a complete cycle.  AC will cross zero twice during each cycle.
 * In between each of the zero points is a peak (one negative,
 * one positive per cycle).
 *
 * The AC module (ac.c) monitors the zerocross and tracks how
 * long it has been since the last crossing.
 *
 * The state of each triac is maintained by a latch (LS374) on the
 * power driver board.  When '1', the triac is enabled and allows
 * current to flow until the next zerocrossing point.  During this
 * time, the state of the latch is irrelevant; the triac is kept on.
 * When set to '0', the triac will be turned off at the next
 * zerocross point, if it isn't already off.  Note that the time that
 * a triac can be turned on varies, but the time it is turned off
 * is fixed.
 *
 * GI lamps can either be undimmed or dimmed.  In undimmed mode,
 * we just turn on/off the latched value and leave it there.
 * 'On' strings are permanently receiving power and thus run at
 * full intensity.  At the end of each half-cycle, the latch still
 * has a '1' for that string, and so the triac stays on.
 *
 * In dimmed mode, the latch is enabled and then
 * immediately disabled, at a particular point in the AC phase.
 * The triac is enabled from the point of the first write to the
 * next zerocross point.  The closer the writes are to the next
 * zerocrossing, the dimmer the lamps will be.  Maintaining the
 * lamps at this intensity requires rewriting the latch continuously
 * at the exact point in the AC cycle.
 */

#include <freewpc.h>


/** The normal state of the triacs, not accounting for lamp effects. */
U8 triac_output;

/** The last value written to the triac latch */
U8 triac_io_cache;

/** Says which triacs need to be turned on at specific times
 * during the AC phase.  Each entry is a triac bitset.
 * If entry X is enabled, then X ms after the last zerocross,
 * those triacs are turned on, and remain on until the next ZC.
 */
U8 gi_dimming[NUM_BRIGHTNESS_LEVELS];

/** Which triac outputs are allocated by lamp effects */
U8 gi_leff_alloc;

/** The states of the GI strings currently allocated */
U8 gi_leff_output;

U8 gi_leff_dimming[NUM_BRIGHTNESS_LEVELS];


void triac_dump (void)
{
	dbprintf ("Normal:    %02X\n", triac_output);
	dbprintf ("Dim:       %02X %02X %02X %02X %02X\n",
		gi_dimming[0], gi_dimming[1], gi_dimming[2], gi_dimming[3], gi_dimming[4]);
	dbprintf ("Alloc:     %02X\n", gi_leff_alloc);
	dbprintf ("Leff GI:   %02X\n", gi_leff_output);
	dbprintf ("Leff dim:  %02X %02X %02X %02X %02X\n",
		gi_leff_dimming[0], gi_leff_dimming[1], gi_leff_dimming[2], gi_leff_dimming[3], gi_leff_dimming[4]);
}


/** Update the triacs at interrupt time */
void triac_rtt (void)
{
	/* We only need to update the triacs if dimming
	 * needs to be done during this phase of the AC cycle. */
	if (unlikely (gi_dimming[zc_get_timer ()] | gi_leff_dimming[zc_get_timer ()]))
	{
		U8 triac_bits;

		/* Get the current triac states */
		triac_bits = triac_read ();

		/* Turn on the lamps that need to be dimmed at this level. */
		triac_write (triac_bits
			| gi_dimming[zc_get_timer ()]
			| gi_leff_dimming[zc_get_timer ()]);

		/* Now disable the dimmed lamps for the next phase */
		triac_write (triac_bits);
	}
}


/** Clear the dimming feature on a set of triacs. */
void gi_clear_dimming (U8 triac, U8 *dimming)
{
	U8 i;
	for (i=0; i < NUM_BRIGHTNESS_LEVELS; i++)
		dimming[i] &= ~triac;
}

void triac_update (void)
{
	U8 latch;

	/* Refresh the triac latch by turning on all 'normal'
	 * outputs, masked by anything allocated by a lamp effect. */
	latch = triac_output;
	latch &= ~gi_leff_alloc;
	latch |= gi_leff_output;
	triac_write (latch);
}


/** Turns on one or more triacs */
void triac_enable (U8 triac)
{
	log_event (SEV_INFO, MOD_TRIAC, EV_TRIAC_ON, triac);
	gi_clear_dimming (triac, gi_dimming);
	triac_output |= triac;
	triac_update ();
}


/** Turns off one or more triacs */
void triac_disable (U8 triac)
{
	log_event (SEV_INFO, MOD_TRIAC, EV_TRIAC_OFF, triac);
	gi_clear_dimming (triac, gi_dimming);
	triac_output &= ~triac;
	triac_update ();
}


/** Enable dimming for a GI string. */
void gi_dim (U8 triac, U8 intensity)
{
	gi_clear_dimming (triac, gi_dimming);
	triac_output &= ~triac;
	gi_dimming[intensity] |= triac;
	triac_update ();
}



/** Allocates one or more triacs for a lamp effect.
The leff can override the default value for the strings. */
void triac_leff_allocate (U8 triac)
{
	/* Only allow unallocated strings to be manipulated
	 * by this effect. */
	triac &= ~gi_leff_alloc;

	/* Mark the strings as allocated */
	gi_leff_alloc |= triac;

	/* By default, allocated strings are off. */
	gi_leff_output = 0;

	/* TODO - return actually allocated strings to the caller
	 * so that only those will be freed up on leff exit. */
}


/** Frees a set of triacs at the end of a lamp effect */
void triac_leff_free (U8 triac)
{
	gi_clear_dimming (triac, gi_leff_dimming);
	gi_leff_alloc &= ~triac;
}



/** Enables a triac from a lamp effect at full brightness */
void triac_leff_enable (U8 triac)
{
	gi_clear_dimming (triac, gi_leff_dimming);
	gi_leff_output |= triac;
	triac_update ();
}


/** Disables a triac from a lamp effect */
void triac_leff_disable (U8 triac)
{
	gi_clear_dimming (triac, gi_leff_dimming);
	gi_leff_output &= ~triac;
	triac_update ();
}


/** Sets the intensity (brightness) of a single GI triac */
void triac_leff_dim (U8 triac, U8 brightness)
{
	/* Disable the GI string first. */
	gi_clear_dimming (triac, gi_leff_dimming);
	gi_leff_output &= ~triac;

	if (brightness == 0)
	{
		/* Nothing to do if brightness = off */
	}
	else if (brightness < 7)
	{
		/* We want to dim the lamps at levels 1-6.
		Level 7 doesn't work because the GI string
		would have to be turned on and off very shortly
		because the next zerocross point, which can't be
		guaranteed to work. */
		gi_leff_dimming[7-brightness] |= triac;
	}
	else
	{
		/* If the brightness is greater than the threshold,
		then just it turn on all the way, no need to do any
		dimming at IRQ time. */
		gi_leff_output |= triac;
	}

	triac_update ();
}


/** Initialize the triac module */
void triac_init (void)
{
	gi_leff_alloc = 0;
	triac_output = 0;
	gi_leff_output = 0;
	memset (gi_dimming, 0, NUM_BRIGHTNESS_LEVELS);
	memset (gi_leff_dimming, 0, NUM_BRIGHTNESS_LEVELS);
	triac_update ();
}


