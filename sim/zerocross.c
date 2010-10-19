/*
 * Copyright 2008-2010 by Brian Dominy <brian@oddchange.com>
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
#include <simulation.h>
#include <math.h>

extern U8 linux_triac_latch;
extern U8 linux_triac_outputs;

/* Simulation of the zerocross circuit */

int ac_hz = 60; /* AC cycle has 60 cycles per second in US, 50 elsewhere */
#define ZC_HZ (ac_hz * 2)  /* There are twice as many zero crossings */
#define ZC_TIMER_MAX  (1000.0 / ZC_HZ)

int sim_zc_always_set = 0;
int sim_zc_always_clear = 0;

/** Nonzero when voltage has crossed zero.  This value is latched
for software to read. */
unsigned int sim_zc_active;

/** The number of 1ms units between zerocrossing points. */
double sim_zc_timer;


/** Return the magnitude of the current position of the AC cycle,
in the range of 0.0 to 1.0 */
double sim_zc_angle (void)
{
	return sin (sim_zc_timer * 3.1415 / ZC_TIMER_MAX);
}


/** Called by the software when reading the zerocross register.
 * Returns nonzero if currently at a zerocrossing.
 * The register is clear-on-read.
 */
int sim_zc_read (void)
{
	int rc = sim_zc_active;
	sim_zc_active = 0;
	return rc;
}


#ifdef CONFIG_TRIAC
/** On machines with one or more triacs, reset all of them
	whenever zerocross is asserted */
void sim_zc_triac_reset (void)
{
	/* Reset the GI outputs to what is held in the input latch.
	Preserve the values of the other IO lines. */
	sim_triac_update ( (linux_triac_latch & PINIO_GI_STRINGS) |
	                   (linux_triac_outputs & ~PINIO_GI_STRINGS) );
}
#endif


/**
 * Update the state of the zerocross circuit every 1ms.
 */
void sim_zc_periodic (void *data __attribute__((unused)))
{
	if (sim_zc_always_set)
	{
		signal_update (SIGNO_ZEROCROSS, 1);
	}
	else if (sim_zc_always_clear)
	{
		signal_update (SIGNO_ZEROCROSS, 0);
	}
	if (--sim_zc_timer <= 0)
	{
		sim_zc_active = 1;
		sim_zc_timer += ZC_TIMER_MAX;
		signal_update (SIGNO_ZEROCROSS, 1);

#ifdef CONFIG_TRIAC
		sim_zc_triac_reset ();
#endif
	}
	else
	{
		signal_update (SIGNO_ZEROCROSS, 0);
	}
}


void sim_zc_init (void)
{
	sim_zc_active = 0;
	sim_zc_timer = ZC_TIMER_MAX;
	conf_add ("hz", &ac_hz);
	conf_add ("zc.stuck_on", &sim_zc_always_set);
	conf_add ("zc.stuck_off", &sim_zc_always_clear);

	/* Register the first callback */
	sim_time_register (1, TRUE, sim_zc_periodic, NULL);
}

