/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Binary-coded decimal arithmetic
 */

#include <freewpc.h>
#include <bcd.h>


/** Zeroes a BCD value */
void bcd_zero (bcd_t *val, U8 len)
{
	slow_memset (val, 0, len);
}


/** Copy from one BCD to another */
void bcd_copy (bcd_t *dst, const bcd_t *src, U8 len)
{
	slow_memcpy (dst, src, len);
}


/** Adds one BCD number to another. */
void bcd_add (bcd_t *s1, const bcd_t *s2, U8 len)
{
	register U8 _len __m1reg__ = len;

	/* Advance to just past the end */
	s1 += _len;
	s2 += _len;

	bcd_add8 (s1, s2, 0);
	--_len;

	do
	{
		bcd_add8 (s1, s2, 1);
	} while (--_len);
}


/** Increments a BCD number by another value
 * in which only one byte is nonzero (e.g. 40K = 04 00 00).
 * 'offset' identifies the position of the nonzero byte, with
 * zero always meaning the 'ones' byte, and higher values
 * referring to larger place values.  'val' gives the byte
 * value. */
void bcd_add_int (bcd_t * s1, U8 val, U8 len)
{
	bcd_t s2[len];

	slow_memset (s2, 0, len-1);
	s2[len-1] = val;
	bcd_add (s1, s2, len);
}


void bcd_sub (bcd_t *s1, const bcd_t *s2, U8 len)
{
	const bcd_t *s1_init = s1;

	/* Advance to just past the end */
	s1 += len-1;
	s2 += len-1;

	bcd_sub8 (s1, s2, 0);
	while (s1_init <= s1)
	{
		s1--;
		s2--;
		bcd_sub8 (s1, s2, 1);
	}
}


/** Multiply a BCD (in place) by the given integer.
 * Zero is not supported, as it should never be called this
 * way. */
void bcd_mul (bcd_t * s, U8 multiplier, U8 len)
{
	/* If multiplier is 1, nothing needs to be done. */
	if (multiplier > 1)
	{
		/* Otherwise, we need to perform 'multiplier-1'
		 * additions of the value into itself.  This is
		 * not the most elegant way, but multiplications
		 * are not common, and the multipliers are often
		 * going to be small. */
		bcd_t copy[len];
		bcd_copy (copy, s, len);
	
		do {
			bcd_add (s, copy, len);
		} while (--multiplier > 1);
	}
}


/** Compares two BCD numbers.  Returns -1, 0, or 1 accordingly, like memcmp. */
I8 bcd_compare (const bcd_t * s1, const bcd_t * s2, U8 len)
{
	while (len > 0)
	{
		if (*s1 > *s2)
			return (1);
		else if (*s1 < *s2)
			return (-1);
		else
		{
			s1++;
			s2++;
			len--;
		}
	}
	return (0);
}

