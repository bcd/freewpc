/*
 * Copyright 2008, 2010 by Brian Dominy <brian@oddchange.com>
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


/* The watchdog is simulated as follows: every 8ms we decrement a
 * counter that starts at 64.  When it reaches zero, we fire the
 * watchdog.  When software writes to the watchdog register, we
 * just reset the counter back to 64.  So it will take 500ms of
 * inactivity before the watchdog expires. */


/** The maximum number of checks that must elapse to
 * signal a watchdog reset */
#define WDOG_EXPIRY 64


/** The current value of the watchdog timer */
volatile int cycles_until_reset = WDOG_EXPIRY;


/** The lowest value of the watchdog register, for debugging */
int lowest_cycles = WDOG_EXPIRY;


void watchdog_expired (void)	
{
	simlog (SLC_DEBUG, "Watchdog expired\n");
	signal_update (SIGNO_BLANKING, 0);
	sim_exit (ERR_TASK_LOCKUP);
}


static void sim_watchdog_periodic (void *data __attribute__((unused)))
{
	if (--cycles_until_reset < 0)
		watchdog_expired ();
}


void sim_watchdog_reset (void)
{
	/* For debugging, a message will be printed anytime that we
	 * 'almost' failed the watchdog... meaning that we reset it
	 * more slowly than any other time during simulation.  In the
	 * best case, this message will only be displayed once during
	 * early init. */
	if (cycles_until_reset < lowest_cycles)
	{
		simlog (SLC_DEBUG, "Watchdog reset with %d cycles to go", cycles_until_reset);
		lowest_cycles = cycles_until_reset;
	}

	signal_update (SIGNO_BLANKING, 1);
	cycles_until_reset = WDOG_EXPIRY;
}


/** Initialized the simulated watchdog.
 * We install a timer that runs every simulated 8ms. */
void sim_watchdog_init (void)
{
	signal_update (SIGNO_BLANKING, 0);
#ifdef CONFIG_PLATFORM_WPC
	sim_time_register (8, TRUE, sim_watchdog_periodic, NULL);
#endif
}

