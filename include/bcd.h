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

#if 0

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

#endif


void bcd_zero (bcd_t *val, U8 len);
void bcd_copy (bcd_t *dst, const bcd_t *src, U8 len);
void bcd_add (bcd_t *s1, const bcd_t *s2, U8 len);
void bcd_add_int (bcd_t * s1, U8 val, U8 len);
void bcd_sub (bcd_t *s1, const bcd_t *s2, U8 len);
void bcd_mul (bcd_t * s, U8 multiplier, U8 len);
I8 bcd_compare (const bcd_t * s1, const bcd_t * s2, U8 len);

void bcd_string_add (bcd_t *dst, const bcd_t *src, U8 len);
void bcd_string_increment (bcd_t *s, U8 len);
void bcd_string_sub (bcd_t *dst, const bcd_t *src, U8 len);

#ifdef __m6809__

/* Calculate the sum of two BCD-encoded bytes, possibly with carry
from a previous sum.
   The computation is equivalent to "*px = *px + *py + carry"
	for carry == 0 or 1.  (carry == 2 is for something else...)

	px and py are modified by the call to point to the previous
	byte of memory, so that repeated calls to these macros can
	implement BCD string operations (see below).

 */
#define bcd_add8(px, py, carry) \
do { \
	asm volatile ("lda\t,-%0" :: "a" (px) : "d"); \
	if (carry == 2) \
		asm volatile ("adca\t#0"); \
	else if (carry == 1) \
		asm volatile ("adca\t,-%0" :: "a" (py) : "d"); \
	else \
		asm volatile ("adda\t,-%0" :: "a" (py) : "d"); \
	asm volatile ("daa"); \
	asm volatile ("sta\t,%0" :: "a" (px) : "d"); \
} while (0)

/* Similarly, do *px = *px - *py - carry */
#define bcd_sub8(px, py, carry) \
do { \
	asm volatile ("lda\t,-%0" :: "a" (px) : "d"); \
	if (carry == 2) \
		asm volatile ("sbca\t#0"); \
	else if (carry == 1) \
		asm volatile ("sbca\t,-%0" :: "a" (py) : "d"); \
	else \
		asm volatile ("suba\t,-%0" :: "a" (py) : "d"); \
	asm volatile ("daa"); \
	asm volatile ("sta\t,%0" :: "a" (px) : "d"); \
} while (0)

#else

/* TODO - this isn't BCD format */
#define bcd_add8(px, py, carry) do { --px; *px = *px + *--py + carry; } while (0)
#define bcd_sub8(px, py, carry) do { --px; *px = *px - *--py - carry; } while (0)

#endif /* __m6809__ */


/* Calculate the sum of two BCD strings of constant size.  A looping
 * counter is not needed because of the fixed length, which helps save a
 * register.
 *
 * px and py still need to point to the _last_ byte of the strings.
 * They must also be the same size.
 */

#define bcd_string_add0(px, py, carry) bcd_add8 (px, py, carry)
#define bcd_string_add1(px, py, carry) do { bcd_add8 (px, py, carry); px--; py--; bcd_string_add0 (px, py, 1); } while (0)
#define bcd_string_add2(px, py, carry) do { bcd_add8 (px, py, carry); px--; py--; bcd_string_add1 (px, py, 1); } while (0)
#define bcd_string_add3(px, py, carry) do { bcd_add8 (px, py, carry); px--; py--; bcd_string_add2 (px, py, 1); } while (0)
#define bcd_string_add4(px, py, carry) do { bcd_add8 (px, py, carry); px--; py--; bcd_string_add3 (px, py, 1); } while (0)
#define bcd_string_add5(px, py, carry) do { bcd_add8 (px, py, carry); px--; py--; bcd_string_add4 (px, py, 1); } while (0)

#endif /* _BCD_H */
