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

/**
 * \file
 * \brief Specialized math functions
 * 
 * These functions provide math services that cannot be done efficiently
 * (or at all!) using normal C methods.  It is mostly to work around
 * deficiencies in the 6809 compiler.  These services are intended to be
 * called infrequently, and are therefore placed into a separate page
 * of ROM.
 */

#include <freewpc.h>

static U32 powers_of_two_table[] = {
	1UL << 0,
	1UL << 1,
	1UL << 2,
	1UL << 3,
	1UL << 4,
	1UL << 5,
	1UL << 6,
	1UL << 7,
	1UL << 8,
	1UL << 9,
	1UL << 10,
	1UL << 11,
	1UL << 12,
	1UL << 13,
	1UL << 14,
	1UL << 15,
	1UL << 16,
	1UL << 17,
	1UL << 18,
	1UL << 19,
	1UL << 20,
	1UL << 21,
	1UL << 22,
	1UL << 23,
	1UL << 24,
	1UL << 25,
	1UL << 26,
	1UL << 27,
	1UL << 28,
	1UL << 29,
	1UL << 30,
	1UL << 31,
};

/** Divide a 32-bit value by a 16-bit value, and return the
 * quotient and the remainder */
void udiv32 (U32 reg, U16 divisor, U32 *quotientp, U32 *remainderp)
{
	U32 quotient = 0;

	S16 y0 = 16 - __builtin_clz (divisor);
	while (reg >= divisor)
	{
		S16 guess = 31 - __builtin_clzl (reg) - y0;
		if ((guess > 0) && (guess < 32))
		{
			quotient += powers_of_two_table[guess];
			reg -= divisor * powers_of_two_table[guess];
		}
		else
		{
			quotient++;
			reg -= divisor;
		}
	}

	if (*remainderp)
		*remainderp = reg;
	if (*quotientp)
		*quotientp = quotient;
}

