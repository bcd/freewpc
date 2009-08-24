/*
 * Copyright 2008-2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Dot-matrix manipulation that works in byte-sized blocks
 *
 */

#include <freewpc.h>

__attribute__((noinline))
void dmd_rough_copy (U8 bx, U8 y, U8 bwidth, U8 height)
{
	U8 *dst = DMD_LOW_BASE;
	U8 *src = DMD_HIGH_BASE;

	while (height > 0)
	{
		U8 xoff;
		for (xoff = 0; xoff < bwidth; xoff++)
		{
			dst[xoff] = src[xoff];
		}
		dst += DMD_BYTE_WIDTH;
		src += DMD_BYTE_WIDTH;
		height--;
	}
}


__attribute__((noinline))
void dmd_rough_erase (U8 bx, U8 y, U8 bwidth, U8 height)
{
	U8 *dst = DMD_LOW_BASE;
	U8 *src = DMD_HIGH_BASE;

	while (height > 0)
	{
		U8 xoff;
		for (xoff = 0; xoff < bwidth; xoff++)
		{
			dst[xoff] = 0;
		}
		dst += DMD_BYTE_WIDTH;
		src += DMD_BYTE_WIDTH;
		height--;
	}
}


#if 0
void dmd_rough_test (void)
{
	dmd_rough_erase (0x1, 0x1, 0x8, 0x4);
}
#endif

