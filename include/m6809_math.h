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

#ifndef _M6809_MATH_H
#define _M6809_MATH_H

uint16_t div10 (uint8_t val);


#define DIV10(u8, quot, rem) \
do \
{ \
	uint16_t __unused_quot_rem __attribute__ ((unused)) = div10 (u8); \
	asm ("sta\t%0" :: "m" (quot)); \
	asm ("stb\t%0" :: "m" (rem)); \
} while (0)


#endif /* _M6809_MATH_H */
