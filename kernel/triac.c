
/*
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

__fastram__ U8 triac_enables;
__fastram__ U8 triac_leff_alloc;
__fastram__ U8 triac_leff_bits;

U8 triac_dim_level[NUM_GI_TRIACS];



void gi_recalc_zerocross ()
{
}


void triac_rtt (void)
{
	U8 triac_bits;

	gi_recalc_zerocross ();

	triac_bits = triac_enables;
	triac_bits &= ~triac_leff_alloc;
	triac_bits |= triac_leff_bits;

	triac_write (triac_enables);
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


void triac_init (void)
{
	triac_enables = 0;
	triac_leff_alloc = 0;
	triac_leff_bits = 0;
	triac_write (0);
	memset (triac_dim_level, 0, NUM_GI_TRIACS);
}


