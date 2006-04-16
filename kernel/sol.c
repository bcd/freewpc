
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


__fastram__ struct {
	uint8_t high;
	uint8_t low;
	uint8_t flash1;
	uint8_t flash2;
	uint8_t aux1;
	uint8_t aux2;
	uint8_t cksum;
} sol_state;


__fastram__ uint8_t *pulse_queue_head;
__fastram__ uint8_t *pulse_queue_tail;
uint8_t pulse_queue[8];

typedef struct sol_pulse {
	U8 *sol_cache;
	U8 sol_bitmask;
	U8 sol_active_high;
	U8 sol_on_irqs;
	U8 sol_off_irqs;
} sol_pulse_t;



uint8_t sol_calc_cksum (void)
{
	return sol_state.high +
		sol_state.low +
		sol_state.flash1 +
		sol_state.flash2 +
		sol_state.aux1 +
		sol_state.aux2;
}


void sol_update_cksum (void)
{
	sol_state.cksum = sol_calc_cksum ();
}


int sol_verify_cksum (void)
{
	register uint8_t cksum = sol_calc_cksum ();
	/* If the checksums are equal, then the difference is
	 * zero and the negation of that becomes true */
	return (!(cksum - sol_state.cksum));
}


void sol_rtt (void)
{
	/* TODO - checksum verification should be done outside
	 * of IRQ, as this takes up precious cycles.  It should
	 * not hurt too badly to do this every 8ms or 16ms. */
	if (sol_verify_cksum ())
	{
		/* Write cached values to hardware I/O */
		*(uint8_t *)WPC_SOL_LOWPOWER_OUTPUT = sol_state.low;
		*(uint8_t *)WPC_SOL_HIGHPOWER_OUTPUT = sol_state.high;
		*(uint8_t *)WPC_SOL_FLASH1_OUTPUT = sol_state.flash1;
		*(uint8_t *)WPC_SOL_FLASH2_OUTPUT = sol_state.flash2;
		*(uint8_t *)WPC_EXTBOARD1 = sol_state.aux2; /* TZ specific */
	}
	else
	{
		sol_state.low = 0;
		sol_state.high = 0;
		sol_state.flash1 = 0;
		sol_state.flash2 = 0;
		sol_state.aux1 = 0;
		sol_state.aux2 = 0; /* TZ specific */
		sol_update_cksum ();
		nonfatal (ERR_SOL_CKSUM_ERROR);
	}
}


void sol_on (solnum_t sol)
{
	register bitset p = (bitset)&sol_state;
	register uint8_t v = sol;
	__setbit(p, v);
	sol_update_cksum ();
}

void sol_off (solnum_t sol)
{
	register bitset p = (bitset)&sol_state;
	register uint8_t v = sol;
	__clearbit(p, v);
	sol_update_cksum ();
}


void sol_pulse (solnum_t sol)
{
	sol_on (sol);
	task_sleep (TIME_66MS);
	sol_off (sol);
}

void sol_serve (void)
{
	sol_pulse (MACHINE_BALL_SERVE_SOLENOID);
}

void sol_init (void)
{
	memset (&sol_state, 0, sizeof (sol_state));
	sol_update_cksum ();
}

