/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

#ifndef _M6809_MATH_H
#define _M6809_MATH_H

/* Math functions specific to the 6809 */

/* Prototype for div10 */
U16 div10 (U8 val);

/* Frontend macro for div10, which extracts the quotient and remainder
from the 16-bit result and puts this into separate 8-bit locations */
#define DIV10(u8, quot, rem) \
do \
{ \
	U16 __unused_quot_rem __attribute__ ((unused)) = div10 (u8); \
	asm ("sta\t%0" :: "m" (quot)); \
	asm ("stb\t%0" :: "m" (rem)); \
} while (0)


/* Rotate an 8-bit value in memory */

extern inline void m6809_rotate8 (U8 *mem)
{
	asm ("lsl\t%0\n"
	     "\trol\t%0\n"
		  "\tbcc\t__IL%=\n"
		  "\tinc\t%0\n"
		  "__IL%=::\n" : "=m" (*mem));
}

struct divrem32
{
	U32 quotient;
	U32 remainder;
};

void divide32 (U32 *dividend, U32 *divisor, struct divrem32 *divrem);

#endif /* _M6809_MATH_H */
