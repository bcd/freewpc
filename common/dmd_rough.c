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

#include <freewpc.h>


/* The parameter block is global to workaround the limitation of
 * passing large numbers of arguments to the 6809 C compiler. */
struct dmd_rough_args dmd_rough_args;


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


/**
 * Invert a portion of the low DMD page.
 *
 * Use the function dmd_rough_erase() instead of this one.  This function
 * expects the global parameter block 'dmd_rough_args' to be filled in.
 */
__attribute__((noinline)) void dmd_rough_invert1 (void)
{
	U8 *dst;

	do {
		dst = dmd_rough_args.dst;
		U8 bytes = dmd_rough_args.bwidth;
		do {
			*dst++ ^= 0xFF;
			bytes--;
		} while (bytes > 0);
		dmd_rough_args.dst += DMD_BYTE_WIDTH;
		dmd_rough_args.height--;
	} while (dmd_rough_args.height > 0);
}

