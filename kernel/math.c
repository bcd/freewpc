
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

//#define CONFIG_HOST

#ifndef CONFIG_HOST
#include <freewpc.h>
#else
typedef unsigned char U8;
typedef unsigned short U16;
#endif

static U8 seed1;
static U8 seed2;



static inline U8 read_random_register (void)
{
#ifdef CONFIG_HOST
	static U8 readings[] = {
		14, 7, 11, 8, 10, 2, 5, 9, 11, 13
	};
	static int offset = 0;
	offset = (offset + 1) % 10;
	return readings[offset];
#else
	return *(U8 *)WPC_DMD_FIRQ_ROW_VALUE;
#endif
}

U8 random (void)
{
	U8 val;

	seed1 ^= read_random_register ();
	seed1 += 17;
	seed2 ^= ~seed1;
	seed2 += 61;
	val = ~seed2;
	return (val);
}


U8 random_bounded (U8 upper_bound)
{
	U8 val = random ();
	/* TODO : bound the value */
	return (val);
}


void random_reseed (void)
{
	seed1++;
}


#ifdef CONFIG_HOST
int main (void)
{
	U8 a, b, q, r;
	for (;;) {
		scanf ("%d %d", &a, &b);
		div (a, b, &q, &r);
		printf ("%d %d\n", q, r);
	}
}
#endif

