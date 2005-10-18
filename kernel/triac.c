
/*
 * Theory of operation.
 *
 * The CPU generates an IRQ once every 960 msec.
 *
 * The GI is driven off 48VAC at a frequency of 60 Hz.
 * This means that 60 times a second,...
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

U8 triac_dim_level[NUM_GI_TRIACS];



void gi_recalc_zerocross ()
{
}


void triac_rtt (void)
{
	gi_recalc_zerocross ();

   /* Ensure that the triacs are disabled until explicitly decided to
	 * turn them on.
	 */
	//triac_write (0);

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



void triac_init (void)
{
	triac_enables = 0;
	triac_write (0);
	memset (triac_dim_level, 0, NUM_GI_TRIACS);
}


