
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

#define sol_on(id)	sol_modify(id, 0xff)
#define sol_off(id)	sol_modify(id, 0


U8 sol_cycle;


U8 sol_state[SOL_ARRAY_WIDTH][SOL_CYCLES];



void sol_rtt (void)
{
}


void sol_modify (U8 id, U8 cycle_mask)
{
	/* Set a bit in each of the 8 bitarrays */
	U8 *bitptr = sol_state[0];
	U8 bitpos = id;
	int count = SOL_CYCLES;
	while (count-- > 0)
	{
		*bitptr &= cycle_mask;
		*bitptr |= cycle_mask;
	}
}


void sol_init (void)
{
	memset (sol_state, 0, sizeof (sol_state));
}

