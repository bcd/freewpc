/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

#ifndef _HWSIM_TRIAC_H
#define _HWSIM_TRIAC_H

/* Note: only one triac is supported per platform at this time */

struct sim_triac
{
	/* The input latch that is written by the CPU */
	U8 in;

	/* The output writer that is a function of the inputs
	and the zerocross. */
	void (*out_write) (void *, unsigned int, U8);
};


extern void sim_triac_write (struct sim_triac *triac, unsigned int addr, U8 val)
{
	triac->in |= val;
	triac->out_write (NULL, 0, triac->in);

}

extern void sim_triac_reset (struct sim_triac *triac)
{
	triac->in = 0;
	triac->out_write (NULL, 0, 0);
}


#endif /* _HWSIM_TRIAC_H */
