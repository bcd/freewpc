/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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

/* The maximum length of the AC period when dimming via triacs.
 * See kernel/ac.c for a similar define.  TODO : these should be merged.
 */
#define ZC_MAX_PERIOD 11

/** The normal state of the triacs, not accounting for lamp effects. */
U8 triac_output;

/** The last value written to the triac latch */
U8 triac_io_cache;

/** Which triac outputs are allocated by lamp effects */
U8 gi_leff_alloc;

/** The states of the GI strings currently allocated */
U8 gi_leff_output;

#ifdef CONFIG_TRIAC
/** Says which triacs need to be turned on at specific times
 * during the AC phase.  Each entry is a triac bitset.
 * If entry X is enabled, then X ms after the last zerocross,
 * those triacs are turned on, and remain on until the next ZC.
 * Note that only values between 0..NUM_BRIGHTNESS_LEVELS are actually
 * written, but values up to ZC_MAX_PERIOD are read.
 */
U8 gi_dimming[ZC_MAX_PERIOD];

/** Like gi_dimming, but for lamp effects */
U8 gi_leff_dimming[ZC_MAX_PERIOD];
#endif


void triac_dump (void)
{
	dbprintf ("Normal:    %02X\n", triac_output);
#ifdef CONFIG_TRIAC
	dbprintf ("Dim:       %02X %02X %02X %02X %02X\n",
		gi_dimming[0], gi_dimming[1], gi_dimming[2], gi_dimming[3], gi_dimming[4]);
#endif
	dbprintf ("Alloc:     %02X\n", gi_leff_alloc);
	if (gi_leff_alloc)
	{
		dbprintf ("Leff GI:   %02X\n", gi_leff_output);
#ifdef CONFIG_TRIAC
		dbprintf ("Leff dim:  %02X %02X %02X %02X %02X\n",
			gi_leff_dimming[0], gi_leff_dimming[1], gi_leff_dimming[2],
			gi_leff_dimming[3], gi_leff_dimming[4]);
#endif
	}
}


#ifdef CONFIG_TRIAC
/**
 * Update the triacs at interrupt time, when GI dimming is in effect
 * DIM_BITS says which triac strings need to be turned on briefly
 * during this phase of the AC cycle.
 */
static __attribute__((noinline)) void triac_rtt_1 (U8 dim_bits)
{
	static U8 triac_bits;

	/* Get the current triac states */
	triac_bits = pinio_read_triac ();

	/* Turn on the lamps that need to be dimmed at this level. */
	pinio_write_triac (triac_bits | dim_bits);

	/* Now disable the dimmed lamps for the next phase */
	pinio_write_triac (triac_bits);
}


/** Update the triacs at interrupt time */
/* RTT(name=triac_rtt freq=1) */
void triac_rtt (void)
{
	/* We only need to update the triacs if dimming
	 * needs to be done during this phase of the AC cycle.
	 *
	 * Moved the mechanics of the triac update into a separate function
	 * above, to optimize the function in the common case when nothing
	 * needs to be done.
	 */
	U8 dim_bits = gi_dimming[zc_get_timer ()] | gi_leff_dimming[zc_get_timer ()];
	if (unlikely (dim_bits))
	{
		triac_rtt_1 (dim_bits);
	}
}
#endif /* CONFIG_TRIAC */


/** Clear the dimming feature on a set of triacs. */
#ifdef CONFIG_TRIAC
void gi_clear_dimming (U8 triac, U8 *dimming)
{
	U8 i;
	for (i=0; i < NUM_BRIGHTNESS_LEVELS; i++)
	{
		dimming[i] &= ~triac;
	}
}
#else
#define gi_clear_dimming(triac,dimming)
#endif


void triac_update (void)
{
	U8 latch;

	/* Refresh the triac latch by turning on all 'normal'
	 * outputs, masked by anything allocated by a lamp effect. */
	latch = triac_output;
	latch &= ~gi_leff_alloc;
	latch |= gi_leff_output;
#ifdef CONFIG_TRIAC
	pinio_write_triac (latch);
#else
	pinio_write_gi (latch);
#endif
}


/** Turns on one or more triacs */
void gi_enable (U8 triac)
{
	log_event (SEV_INFO, MOD_TRIAC, EV_TRIAC_ON, triac);
	gi_clear_dimming (triac, gi_dimming);
	triac_output |= triac;
	triac_update ();
}


/** Turns off one or more triacs */
void gi_disable (U8 triac)
{
	log_event (SEV_INFO, MOD_TRIAC, EV_TRIAC_OFF, triac);
	gi_clear_dimming (triac, gi_dimming);
	triac_output &= ~triac;
	triac_update ();
}


#ifdef CONFIG_TRIAC
/** Enable dimming for a GI string. */
void gi_dim (U8 triac, U8 brightness)
{
	gi_clear_dimming (triac, gi_dimming);
	triac_output &= ~triac;
	if (brightness == 0)
		;
	else if (brightness < 7 && system_config.allow_dim_illum == YES)
		gi_dimming[7 - brightness] |= triac;
	else
		triac_output |= triac;
	triac_update ();
}
#endif


/** Allocates one or more triacs for a lamp effect.
The leff can override the default value for the strings. */
void gi_leff_allocate (U8 triac)
{
	/* Only allow unallocated strings to be manipulated
	 * by this effect. */
	triac &= ~gi_leff_alloc;

	/* Mark the strings as allocated */
	gi_leff_alloc |= triac;

	/* By default, allocated strings are off. */
	gi_leff_output &= ~triac;

	/* TODO - return actually allocated strings to the caller
	 * so that only those will be freed up on leff exit. */
	triac_update ();
}


/** Frees a set of triacs at the end of a lamp effect */
void gi_leff_free (U8 triac)
{
	gi_clear_dimming (triac, gi_leff_dimming);
	gi_leff_alloc &= ~triac;
	gi_leff_output &= ~triac;
	triac_update ();
}



/** Enables a triac from a lamp effect at full brightness */
void gi_leff_enable (U8 triac)
{
	gi_clear_dimming (triac, gi_leff_dimming);
	gi_leff_output |= triac;
	triac_update ();
}


/** Disables a triac from a lamp effect */
void gi_leff_disable (U8 triac)
{
	gi_clear_dimming (triac, gi_leff_dimming);
	gi_leff_output &= ~triac;
	triac_update ();
}


#ifdef CONFIG_TRIAC
/** Sets the intensity (brightness) of a single GI triac */
void gi_leff_dim (U8 triac, U8 brightness)
{
	/* Disable the GI string first. */
	gi_clear_dimming (triac, gi_leff_dimming);
	gi_leff_output &= ~triac;

	if (brightness == 0)
	{
		/* Nothing to do if brightness = off */
	}
	else if (brightness < 7 && system_config.allow_dim_illum == YES)
	{
		/* We want to dim the lamps at levels 1-6.
		Level 7 doesn't work because the GI string
		would have to be turned on and off very shortly
		before the next zerocross point, which can't be
		guaranteed to work. */
		gi_leff_dimming[7-brightness] |= triac;
	}
	else
	{
		/* If the brightness is greater than the threshold,
		or if dimming has been disabled in the menu adjustments,
		then just it turn on all the way -- no need to do any
		dimming at IRQ time. */
		gi_leff_output |= triac;
	}

	triac_update ();
}
#endif


/** Initialize the triac module */
void gi_init (void)
{
	gi_leff_alloc = 0;
	gi_leff_output = 0;
	triac_output = 0;
#ifdef CONFIG_TRIAC
	memset (gi_dimming, 0, ZC_MAX_PERIOD);
	memset (gi_leff_dimming, 0, ZC_MAX_PERIOD);
#endif
	triac_update ();
}


