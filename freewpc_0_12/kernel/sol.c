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

#include <freewpc.h>

/**
 * \file
 * \brief Handles updating the solenoids.
 * The state of the solenoids is maintained in memory; the
 * actual I/O is written at IRQ time to refresh the hardware.
 *
 * Normal solenoids that are pulsed are updated using a cyclical
 * buffer of solenoid states.  A coil can be pulsed with a "mask"
 * where each 1 bit indicates that the coil should be on, and 0
 * keeps it off.  So a mask of 0x55 would turn on the coil at
 * 50% power.  The ports to be turned on at any given IRQ are
 * precalculated at the time the coil is turned on/off for
 * efficiency.
 */


/** The current stage of the solenoid update cycle */
static U8 sol_cycle;

/** The current state of the duty cycled solenoids */
static U8 sol_state[SOL_CYCLES][SOL_ARRAY_WIDTH];

/** The current state of the non-duty cycled solenoids */
__fastram__ U8 sol_rt_state[SOL_ARRAY_WIDTH];


/* Realtime update of the solenoids.
 *
 * Copy the solenoid states for the current cycles to the hardware.
 */
void
sol_rtt (void)
{
	/* TODO - align adjacent registers to do 16-bit writes? */
	wpc_asic_write (WPC_SOL_HIGHPOWER_OUTPUT,
		sol_state[sol_cycle][0] | sol_rt_state[0]);
	wpc_asic_write (WPC_SOL_LOWPOWER_OUTPUT,
		sol_state[sol_cycle][1] | sol_rt_state[1]);
	wpc_asic_write (WPC_SOL_FLASH1_OUTPUT,
		sol_state[sol_cycle][2] | sol_rt_state[2]);
	wpc_asic_write (WPC_SOL_FLASH2_OUTPUT,
		sol_state[sol_cycle][3] | sol_rt_state[3]);

#ifdef MACHINE_SOL_EXTBOARD1
	wpc_asic_write (WPC_EXTBOARD1, sol_state[sol_cycle][5] | sol_rt_state[5]);
#endif

	/* Advance cycle counter */
	sol_cycle++;
	sol_cycle %= SOL_CYCLES;
}


/*
 * Turns on/off a solenoid.
 * The cycle_mask controls the "strength" at which the coil will be
 * on. */
void
sol_modify (solnum_t sol, U8 cycle_mask)
{
	/* Set a bit in each of the 8 bitarrays */
	U8 count;
	for (count = 0; count < SOL_CYCLES; count++)
	{
		register bitset p = &sol_state[count][0];
		register U8 v = sol;

		if (cycle_mask & 1)
			__setbit (p, v);
		else
			__clearbit (p, v);

		cycle_mask >>= 1;
	}
}


/*
 * Pulses a solenoid.  The solenoid is turned on, a delay occurs,
 * and then it is turned off.
 * TODO - should this be performed in a new task context, to avoid
 * blocking the caller?
 */
void
sol_modify_pulse (solnum_t sol, U8 cycle_mask)
{
	sol_modify (sol, cycle_mask);
	task_sleep (TIME_66MS);
	sol_modify (sol, 0);
}


void
sol_init (void)
{
	memset (sol_state, 0, sizeof (sol_state));
	memset (sol_rt_state, 0, sizeof (sol_rt_state));
	sol_cycle = 0;
}

