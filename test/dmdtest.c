/*
 * Copyright 2008-2010 by Brian Dominy <brian@oddchange.com>
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

void dmd_fill_color_line (U16 *ptr, U8 color)
{
	ptr[0] = ptr[1] = (color & 0x01) ? 0xFFFF: 0;
	ptr[256] = ptr[257] = (color & 0x02) ? 0xFFFF: 0;
}

void dmd_fill_color_pattern (U8 color)
{
	U16 *p = (U16 *)dmd_low_buffer + color * 2;
	U8 n;
	for (n=0; n < 32; n++)
	{
		dmd_fill_color_line (p, color);
		p += 8;
	}
}

void dmd_show_color_pattern (void)
{
	U8 color;

	dmd_alloc_pair ();
	for (color = 0; color < 4; color++)
		dmd_fill_color_pattern (color);
	dmd_show2 ();
}
