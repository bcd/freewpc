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

/**
 * \file
 * \brief Dot-matrix manipulation that works in byte-sized blocks
 *
 */

/* TODO - height must be a multiple of 2? */

#include <freewpc.h>

/*
 * The parameters to a rough copy or erase operation.
 * The user parameters are given in terms of pixels, however these
 * must be converted into byte coordinates.
 */

struct dmd_rough_args
{
	U8 *dst;
	U8 bwidth;
	U8 height;
};

/* The parameter block is global to workaround the limitation of
 * passing large numbers of arguments to the 6809 C compiler. */
struct dmd_rough_args dmd_rough_args;


/**
 * Copy a portion of the low DMD page into the high page.
 *
 * x,y denote the coordinates of the upper leftmost pixel.
 * w is the width of the area to be copied in pixels.
 * h is the height of the area to be copied in pixels.
 * All pixel values should be a multiple of 8 for correct results.
 */
#define dmd_rough_copy(x, y, w, h) \
	do { \
		dmd_rough_args.dst = DMD_HIGH_BASE + (x / CHAR_BIT) + y * DMD_BYTE_WIDTH; \
		dmd_rough_args.bwidth = w / CHAR_BIT; \
		dmd_rough_args.height = h; \
		dmd_rough_copy1 (); \
	} while (0)

/**
 * Zero a portion of the low DMD page.
 *
 * x,y denote the coordinates of the upper leftmost pixel.
 * width is the width of the area to be erased in pixels.
 * height is the height of the area to be erased in pixels.
 * All pixel values should be a multiple of 8 for correct results.
 */
#define dmd_rough_erase(x, y, width, height) \
	do { \
		dmd_rough_args.dst = DMD_HIGH_BASE + (x / CHAR_BIT) + y * DMD_BYTE_WIDTH; \
		dmd_rough_args.bwidth = w / CHAR_BIT; \
		dmd_rough_args.height = h; \
		dmd_rough_erase1 (); \
	} while (0)



/**
 * Copy a portion of the low DMD page into the high page.
 *
 * Use the function dmd_rough_copy() instead of this one.  This function
 * expects the global parameter block 'dmd_rough_args' to be filled in.
 */
__attribute__((noinline)) void dmd_rough_copy1 (void)
{
	U8 *dst;
	U8 *src;

	do {
		dst = dmd_rough_args.dst;
		src = dst - DMD_PAGE_SIZE;
		U8 bytes = dmd_rough_args.bwidth;
		do {
			*dst++ = *src++;
			bytes--;
		} while (bytes > 0);
		dmd_rough_args.dst += DMD_BYTE_WIDTH;
		dmd_rough_args.height--;
	} while (dmd_rough_args.height > 0);
}


/**
 * Zero a portion of the low DMD page.
 *
 * Use the function dmd_rough_erase() instead of this one.  This function
 * expects the global parameter block 'dmd_rough_args' to be filled in.
 */
__attribute__((noinline)) void dmd_rough_erase1 (void)
{
	U8 *dst;

	do {
		dst = dmd_rough_args.dst;
		U8 bytes = dmd_rough_args.bwidth;
		do {
			*dst++ = 0;
			bytes--;
		} while (bytes > 0);
		dmd_rough_args.dst += DMD_BYTE_WIDTH;
		dmd_rough_args.height--;
	} while (dmd_rough_args.height > 0);
}


#if 1 /* for testing */
void dmd_rough_test (void)
{
	dmd_rough_copy (32, 4, 64, 4);
}
#endif
