/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

#define SIM_ZC_NONE
//#define SIM_ZC_DOMESTIC
//#define SIM_ZC_EXPORT


/* Simulation of the zerocross circuit */

unsigned int sim_zc_active;

unsigned int sim_zc_timer_start;
unsigned int sim_zc_timer;

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


void sim_zc_periodic (void *data __attribute__((unused)))
{
	if (--sim_zc_timer == 0)
	{
		sim_zc_active = 1;
		sim_zc_timer = sim_zc_timer_start;
		signal_update (SIGNO_ZEROCROSS, 1);
	}
	else
	{
		signal_update (SIGNO_ZEROCROSS, 0);
	}
}


void sim_zc_init (void)
{
	sim_zc_active = 0;

	/* Generate zerocross every 8ms -- close enough */
	sim_zc_timer = sim_zc_timer_start = 8;

	/* Register a callback every 1ms */
	sim_time_register (1, TRUE, sim_zc_periodic, NULL);
}

