
/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

#define SOL_COUNT 48

#define SOL_ARRAY_WIDTH	((SOL_COUNT + 8) / 8)

#define SOL_CYCLES 8

#define SOL_DUTY_0		0x0
#define SOL_DUTY_12_88	0x40
#define SOL_DUTY_25_75	0x22
#define SOL_DUTY_50_50	0x55
#define SOL_DUTY_75_25	0x77
#define SOL_DUTY_100		0xFF

/* For compatibility, sol_on and sol_off refer to all-on
 * or all-off. */
#define sol2_on(id)	sol_modify(id, SOL_DUTY_100)

#define sol2_off(id)	sol_modify(id, SOL_DUTY_0)


static U8 sol_cycle;


static U8 sol_state[SOL_ARRAY_WIDTH][SOL_CYCLES];



void
sol2_rtt (void)
{
	/* TODO - align adjacent registers to do 16-bit writes? */
	*(volatile U8 *)WPC_SOL_LOWPOWER_OUTPUT = sol_state[0][sol_cycle];
	*(volatile U8 *)WPC_SOL_HIGHPOWER_OUTPUT = sol_state[1][sol_cycle];
	*(volatile U8 *)WPC_SOL_FLASH1_OUTPUT = sol_state[2][sol_cycle];
	*(volatile U8 *)WPC_SOL_FLASH2_OUTPUT = sol_state[3][sol_cycle];
	*(volatile U8 *)WPC_EXTBOARD1 = sol_state[4][sol_cycle]; /* TODO : TZ */

	/* Advance cycle counter */
	sol_cycle++;
	sol_cycle %= SOL_CYCLES;
}


void
sol2_modify (solnum_t sol, U8 cycle_mask)
{
	/* Set a bit in each of the 8 bitarrays */
	int count = SOL_CYCLES;
	for (count = 0; count < SOL_CYCLES; count++)
	{
		register bitset p = &sol_state[0][count];
		register U8 v = sol;

		if (cycle_mask & 1)
			__setbit (p, v);
		else
			__clearbit (p, v);
	}
}


void
sol2_init (void)
{
	memset (sol_state, 0, sizeof (sol_state));
	sol_cycle = 0;
}

