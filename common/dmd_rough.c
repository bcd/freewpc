/*
 * Copyright 2008-2011 by Brian Dominy <brian@oddchange.com>
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
		U8 bytes = dmd_rough_args.size.x;
		do {
			*dst++ = *src++;
			bytes--;
		} while (bytes > 0);
		dmd_rough_args.dst += DMD_BYTE_WIDTH;
		dmd_rough_args.size.y--;
	} while (dmd_rough_args.size.y > 0);
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
		U8 bytes = dmd_rough_args.size.x;
		do {
			*dst++ = 0;
			bytes--;
		} while (bytes > 0);
		dmd_rough_args.dst += DMD_BYTE_WIDTH;
		dmd_rough_args.size.y--;
	} while (dmd_rough_args.size.y > 0);
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
		U8 bytes = dmd_rough_args.size.x;
		do {
			*dst++ ^= 0xFF;
			bytes--;
		} while (bytes > 0);
		dmd_rough_args.dst += DMD_BYTE_WIDTH;
		dmd_rough_args.size.y--;
	} while (dmd_rough_args.size.y > 0);
}


/**
 * Copy a row of data from one DMD page to another.
 */
void dmd_row_copy (U8 *dst)
{
	register U16 *src asm ("u") = (U16 *)(dst - DMD_PAGE_SIZE);
	((U16 *)dst)[0] = src[0];
	((U16 *)dst)[1] = src[1];
	((U16 *)dst)[2] = src[2];
	((U16 *)dst)[3] = src[3];
}


/**
 * Copy a column of data from one DMD page to another.
 */
void dmd_column_copy (U8 *dst)
{
	register U8 *src asm ("u") = dst - DMD_PAGE_SIZE;
	register U8 count = 8;
	do {
		dst[0 * DMD_BYTE_WIDTH] = src[0 * DMD_BYTE_WIDTH];
		dst[1 * DMD_BYTE_WIDTH] = src[1 * DMD_BYTE_WIDTH];
		dst[2 * DMD_BYTE_WIDTH] = src[2 * DMD_BYTE_WIDTH];
		dst[3 * DMD_BYTE_WIDTH] = src[3 * DMD_BYTE_WIDTH];
		dst += 4 * DMD_BYTE_WIDTH;
		src += 4 * DMD_BYTE_WIDTH;
		count--;
	} while (count != 0);
}
