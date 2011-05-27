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

//TODO Add support  different sized segments

/* CALLSET_SECTION (bar, __machine4__) */
#include <freewpc.h>

/* Allows for 11 pixels of padding */
#define DEFAULT_BAR_WIDTH 106
#define SEG_SIZE 5

static const U8 bar_end_bitmap[] = {
	1,5,1,1,1,1,1,
};
static const U8 bar_start_bitmap[] = {
	5,5,31,1,1,1,31,
};

static const U8 bar_seg1_bitmap[] = {
	5,5,31,0,0,0,31,
};

static const U8 bar_seg2_bitmap[] = {
	5,5,31,1,0,1,31,
};

static const U8 bar_fill_bitmap[] = {
	1,5,0,1,1,1,0,
};

void draw_progress_bar (struct progress_bar_ops *ops)
{
	S16 i;
	S16 j;
	/* Boundary checks */
//	if (ops->x + ops->bar_width > 128)
//		ops->x = 128 - ops->bar_width;
//	if (ops->y + SEG_SIZE > 32)
//		ops->y = 32 - SEG_SIZE;
//	if (*ops->fill_level > *ops->max_level)
//		*ops->fill_level = *ops->max_level;

	/* calculate fill level */
	j = *ops->fill_level;
	i = ( j * ops->bar_width) / *ops->max_level;
	/* Draw 1x5 fill slices on the low page */
	while (i > 0)
	{
		bitmap_blit (bar_fill_bitmap, ops->x + i, ops->y);
		i--;
	}
	/* Draw the bar outline on the high page */
	dmd_flip_low_high ();
	bitmap_blit (bar_start_bitmap, ops->x, ops->y);
	for (i = SEG_SIZE; i < ops->bar_width - (SEG_SIZE * 2); i += (SEG_SIZE * 2))
	{
		bitmap_blit (bar_seg1_bitmap, ops->x + i, ops->y);
		bitmap_blit (bar_seg2_bitmap, ops->x + SEG_SIZE + i, ops->y);
	}
	bitmap_blit (bar_end_bitmap, ops->x + i, ops->y);
	dmd_flip_low_high ();
}
