/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef __CSUM_H
#define __CSUM_H


/** Describes an area of protected memory that should be checksummed.
The data area and the checksum itself need not be contiguous.
A reset function is provided which says what to do to the block
if checksum validation fails. */
struct area_csum
{
	/** A pointer to the beginning of the block */
	/* __nvram__ */ U8 *area;

	/** The length of the protected block, in bytes */
	U8 length;

	/** A pointer to the variable that actually holds the checksum */
	/* __nvram__ */ U8 *csum;

	/** A function that will reset the block to factory defaults */
	void (*reset) (void);

	/** The ROM page in which the reset function resides */
	U8 reset_page;
};

void csum_area_update (const struct area_csum *csi);
void csum_area_check (const struct area_csum *csi);
void csum_area_check_all (void);

#endif /* __CSUM_H */
