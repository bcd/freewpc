/*
 * Copyright 2006, 2007, 2009 by Brian Dominy <brian@oddchange.com>
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

/*
 * Divide an 8-bit value by 10 efficiently.
 *
 * Approximate the quotient as q = i*0.00011001101 (binary):
 *    q = ((i>>2) + i) >> 1; times 0.101
 *    q = ((q   ) + i) >> 1; times 0.1101
 *    q = ((q>>2) + i) >> 1; times 0.1001101
 *    q = ((q   ) + i) >> 4; times 0.00011001101
 * 
 * compute the remainder as r = i - 10*q 
 *    r = ((q<<2) + q) << 1; times 1010.
 *    r = i - r;
 */
U16 div10 (U8 v)
{
#ifdef __m6809__
	register U16 res asm ("d");
	U8 i = v;

	/* Note: this takes about 83 cycles */
	__asm__ volatile (
	   "\ttfr b,a\n"
	   "\tsta	%0\n"
		"\tlsra\n"
		"\tlsra		; a = i>>2\n"
		"\tadda	%0	; a = (i>>2 + i)\n"
		"\tlsra		; q (first row)\n"
		"\tadda	%0					\n"
		"\tlsra		; q (second row)\n"
		"\tlsra\n"
		"\tlsra\n"
		"\tadda	%0\n"
		"\tlsra\n"
		"\tadda	%0\n"
		"\tlsra\n"
		"\tlsra\n"
		"\tlsra\n"
		"\tlsra		; q = quotient (fourth row)\n"
		"\ttfr	a,b	; copy to b\n"
		"\tpshs	b\n"
		"\taslb\n"
		"\taslb\n"
		"\taddb	,s+\n"
		"\taslb\n"
		"\tnegb\n"
		"\taddb	%0\n"
	:
	: "m" (i)
	: "d"
	);
	return (res);
#else
	return (v / 10);
#endif
}

