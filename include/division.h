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


/* Macros for implementing fast division by a constant.
 * The algorithm is the standard elementary division algorithm,
 * just in binary. */

#ifdef STANDALONE
typedef unsigned char U8;
#define FASTREG
#else
#include <freewpc.h>
#define FASTREG __areg__
#endif


#define shiftbit(quotient, dividend, divisor, n) \
do { \
	if (((U16)divisor << n) <= 0xFF) { \
		if (dividend >= (divisor << n)) \
		{ \
			quotient |= (1 << n); \
			dividend -= (divisor << n); \
		} \
	} \
} while (0)



extern inline U8 div (U8 x, const U8 y)
{
	register U8 q FASTREG = 0;
	shiftbit(q, x, y, 7);
	shiftbit(q, x, y, 6);
	shiftbit(q, x, y, 5);
	shiftbit(q, x, y, 4);
	shiftbit(q, x, y, 3);
	shiftbit(q, x, y, 2);
	shiftbit(q, x, y, 1);
	shiftbit(q, x, y, 0);
	return q;
}


U8 div_by_10 (U8 x)
{
	return div (x, 10);
}

U8 div_by_47 (U8 x)
{
	return div (x, 47);
}

#ifdef STANDALONE
int main (void)
{
	U8 x;
	for (x=0; x <= 250; x += 10)
	{
		printf ("%d/10 = %d\n", x, div10new (x));
	}
}
#endif
