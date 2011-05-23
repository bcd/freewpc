/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (bar, __machine4__) */
#include <freewpc.h>

#define TOTAL_BAR_WIDTH 106
#define SEG_SIZE 5

const U8 bar_end_bitmap[] = {
	1,5,1,1,1,1,1,
};
const U8 bar_start_bitmap[] = {
	5,5,31,1,1,1,31,
};

const U8 bar_seg1_bitmap[] = {
	5,5,31,0,0,0,31,
};

const U8 bar_seg2_bitmap[] = {
	5,5,31,1,0,1,31,
};

const U8 bar_fill_bitmap[] = {
	1,5,0,1,1,1,0,
};

__machine4__ void draw_progress_bar (U8 x, U8 y, U8 fill_level, U8 max_level)
{
	U8 i;
	/* Boundary checks */
	if (x + TOTAL_BAR_WIDTH > 128)
		x = 128 - TOTAL_BAR_WIDTH;
	if (y + SEG_SIZE > 32)
		y = 32 - SEG_SIZE;
	if (fill_level > max_level)
		fill_level = max_level;
	/* calculate fill level */
	i = (fill_level * TOTAL_BAR_WIDTH) / max_level;
	/* Draw 1x5 fill slices on the low page */
	while (i > 0)
	{
		bitmap_blit (bar_fill_bitmap, x + i, y);
		i--;
	}
	/* Draw the bar outline on the high page */
	dmd_flip_low_high ();
	bitmap_blit (bar_start_bitmap, x, y);
	for (i = SEG_SIZE; i < TOTAL_BAR_WIDTH - (SEG_SIZE * 2); i += (SEG_SIZE * 2))
	{
		bitmap_blit (bar_seg1_bitmap, x + i, y);
		bitmap_blit (bar_seg2_bitmap, x + SEG_SIZE + i, y);
	}
	bitmap_blit (bar_end_bitmap, x + i, y);
	dmd_flip_low_high ();
}
