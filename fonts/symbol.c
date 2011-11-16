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

/**
 * \file
 * \brief A collection of random, small bitmap images.
 * These are collected together into a single 'font'; writing in this font
 * then produces sequences of the symbols on the display.
 */

#include <freewpc.h>

/* For each bitmap image, declare a character array with the width, height,
and data.  Bits appear in row-major order. */

char bitmap_common_box3[] = { 3, 3, 0x7, 0x5, 0x7 };
static char bitmap_common_x3[] = { 3, 3, 0x5, 0x2, 0x5 };
static char bitmap_common_plus3[] = { 3, 3, 0x2, 0x7, 0x2 };

static char bitmap_common_box5[] = { 5, 5, 0x1f, 0x11, 0x11, 0x11, 0x1f };
static char bitmap_common_x5[] = { 5, 5, 0x1f, 0x1b, 0x15, 0x1b, 0x1f };

static char bitmap_common_left_arrow5[] = { 3, 5, 0x4, 0x6, 0x7, 0x6, 0x4 };
static char bitmap_common_right_arrow5[] = { 3, 5, 0x1, 0x3, 0x7, 0x3, 0x1 };

/* A table of all bitmaps grouped together in this 'font'.  The index
is used to retrieve the bitmap later. */
static char *font_symbol_glyphs[] = {
   [BM_BOX3] = bitmap_common_box3,
   [BM_X3] = bitmap_common_x3,
   [BM_PLUS3] = bitmap_common_plus3,
   [BM_BOX5] = bitmap_common_box5,
   [BM_X5] = bitmap_common_x5,
	[BM_LEFT_ARROW5] = bitmap_common_left_arrow5,
	[BM_RIGHT_ARROW5] = bitmap_common_right_arrow5,
};

/* The font descriptor */
const font_t font_symbol = {
	.spacing = 0,
	.height = 0,
	.glyphs = font_symbol_glyphs,
};

