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

/* TODO - height must be a multiple of 2? */

#include <freewpc.h>

struct dmd_rough_args
{
	U8 *dst;
	U8 bwidth;
	U8 height;
};

struct dmd_rough_args dmd_rough_args;

/*
 * The user parameters are given in terms of pixels, however these
 * must be converted into byte coordinates.
 */

#define dmd_rough_copy(x, y, w, h) \
	do { \
		dmd_rough_args.dst = DMD_HIGH_BASE + (x / CHAR_BIT) + y * DMD_BYTE_WIDTH; \
		dmd_rough_args.bwidth = w / CHAR_BIT; \
		dmd_rough_args.height = h; \
		dmd_rough_copy1 (); \
	} while (0)

#define dmd_rough_erase(x, y, width, height) \
	do { \
		dmd_rough_args.dst = DMD_HIGH_BASE + (x / CHAR_BIT) + y * DMD_BYTE_WIDTH; \
		dmd_rough_args.bwidth = w / CHAR_BIT; \
		dmd_rough_args.height = h; \
		dmd_rough_erase1 (); \
	} while (0)

#define args dmd_rough_args


/**
 * Copy a portion of the low DMD page into the high page.
 */
__attribute__((noinline)) void dmd_rough_copy1 (void)
{
	U8 *dst;
	U8 *src;

	do {
		dst = args.dst;
		src = dst - DMD_PAGE_SIZE;
		U8 bytes = args.bwidth;
		do {
			*dst++ = *src++;
			bytes--;
		} while (bytes > 0);
		args.dst += DMD_BYTE_WIDTH;
		args.height--;
	} while (args.height > 0);
}


/**
 * Zero a portion of the low DMD page into the high page.
 */
__attribute__((noinline)) void dmd_rough_erase1 (void)
{
	U8 *dst;

	do {
		dst = args.dst;
		U8 bytes = args.bwidth;
		do {
			*dst++ = 0;
			bytes--;
		} while (bytes > 0);
		args.dst += DMD_BYTE_WIDTH;
		args.height--;
	} while (args.height > 0);
}


void dmd_rough_test (void)
{
	dmd_rough_copy (32, 4, 64, 4);
}

