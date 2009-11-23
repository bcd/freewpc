/*
 * Copyright 2009 Brian Dominy <brian@oddchange.com>
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


U8 carry_flag;


/* Implement the decimal after adjust (DAA) instruction that the 6809 offers. */
U8 daa (U8 x)
{
	carry_flag = 0;
	if ((x & 0x0F) >= 0x0A)
		x += 0x06;
	if ((x & 0xF0) >= 0xA0)
	{
		x += 0x60;
		carry_flag = 1;
	}
	return x;
}


void bcd_string_add (bcd_t *dst, const bcd_t *src, U8 len)
{
	S8 i;
	carry_flag = 0;
	for (i=len-1; i >= 0; --i)
		dst[i] = daa (src[i] + dst[i] + carry_flag);
}


void bcd_string_increment (bcd_t *s, U8 len)
{
	S8 i;
	s[len-1] = daa (s[len-1] + 1);
	for (i=len-2; i >= 0; --i)
		s[i] = daa (s[i] + carry_flag);
}


void bcd_string_sub (bcd_t *dst, const bcd_t *src, U8 len)
{
	S8 i;
	carry_flag = 0;
	for (i=len-1; i >= 0; --i)
		dst[i] = daa (src[i] - dst[i] - carry_flag);
}

