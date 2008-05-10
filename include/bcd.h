/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef _BCD_H
#define _BCD_H

/* Generic routines for dealing with binary-coded decimal values.
The generic versions are portable, but optimal 6809 versions are also provided
that are a little faster on real WPC hardware. */


/** Adjust a byte to make sure it is in valid BCD format. */
extern inline bcd_t bcd_adjust (bcd_t x)
{
	if ((x & 0x0F) > 9)
		return x+6;
	else
		return x;
}


/** Test whether or not a BCD byte has overflowed/underflowed */
extern inline bool bcd_overflowed_p (bcd_t x)
{
	return (x & 0xF0) >= 0xA0;
}


/** Adds two BCD-encoded 8-bit values */
extern inline bcd_t bcd_add (bcd_t x0, bcd_t x1)
{
	return bcd_adjust (x0 + x1);
}


extern inline bcd_t bcd_add_with_carry (bcd_t x0, bcd_t x1, U8 carry)
{
	return bcd_add (x0, x1+carry);
}


extern inline void longbcd_add (bcd_t *s0, bcd_t *s1, U8 len)
{
	S8 i;
	for (i=len-1; i >= 0; i--)
	{
		s0[i] = bcd_add (s0[i], s1[i]);
	}
}


#ifdef __m6809__

/* Calculate the sum of two BCD-encoded bytes, possibly with carry
from a previous sum.
   The computation is equivalent to "*px = *px + *py + carry"
	for carry == 0 or 1.  (carry == 2 is for something else...)
 */
#define bcd_add8(px, py, carry) \
do { \
	asm volatile ("lda\t,-%0" :: "a" (px)); \
	if (carry == 2) \
		asm volatile ("adca\t#0"); \
	else if (carry == 1) \
		asm volatile ("adca\t,-%0" :: "a" (py)); \
	else \
		asm volatile ("adda\t,-%0" :: "a" (py)); \
	asm volatile ("daa"); \
	asm volatile ("sta\t,%0" :: "a" (px)); \
} while (0)

/* Similarly, do *px = *px - *py - carry */
#define bcd_sub8(px, py, carry) \
do { \
	asm volatile ("lda\t,-%0" :: "a" (px)); \
	if (carry == 2) \
		asm volatile ("sbca\t#0"); \
	else if (carry == 1) \
		asm volatile ("sbca\t,-%0" :: "a" (py)); \
	else \
		asm volatile ("suba\t,-%0" :: "a" (py)); \
	asm volatile ("daa"); \
	asm volatile ("sta\t,%0" :: "a" (px)); \
} while (0)

#else

/* TODO - this isn't BCD format */
#define bcd_add8(px, py, carry) do { --px; *px = *px + *--py + carry; } while (0)
#define bcd_sub8(px, py, carry) do { --px; *px = *px - *--py - carry; } while (0)

#endif /* __m6809__ */

#endif /* _BCD_H */
