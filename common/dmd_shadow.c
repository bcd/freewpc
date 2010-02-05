/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Dot-matrix text shadowing
 *
 */

#include <freewpc.h>

/* dmd_shadow() is the low-level routine to generate shadows. */
#ifdef __m6809__
extern void dmd_shadow (void);
#else
#define dmd_shadow() /* TODO - no C version of shadowing */
#endif


/**
 * Generate a shadowed image.
 *
 * On input, provide a mono image in the low-mapped buffer.
 * On output, the high mapped buffer will contain an alpha mask.
 */
void dmd_text_outline (void)
{
	/* Create the shadow plane in the high page */
	dmd_shadow ();

	/* Invert the shadow plane, converting it to a bitmask.
	Each '1' bit indicates part of the background that must
	be zeroed. */
	dmd_invert_page (dmd_high_buffer);
	dmd_flip_low_high ();
}


/**
 *
 */
void dmd_text_blur (void)
{
	dmd_shadow ();
	dmd_flip_low_high ();
}


/**
 * Overlay a 1-color image plus alphamask onto a page pair.
 *
 * DST describes the destination pages, which do not have
 * to be consecutive.
 *
 * SRC is the lookaside ID of a page pair which contains
 * the 1-color image and alphamask, respectively.
 *
 * The alpha mask is applied first to reset some bits of
 * the image to black.  Then the 1-color image is ORed
 * into both destination pages, for 100% intensity.
 */
void dmd_overlay_alpha (dmd_pagepair_t dst, U8 src)
{
	wpc_dmd_set_low_page (dst.u.first);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src) + 1 );
	dmd_and_page ();
	wpc_dmd_set_low_page (dst.u.second);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src) + 1 );
	dmd_and_page ();

	wpc_dmd_set_low_page (dst.u.first);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src));
	dmd_or_page ();
	wpc_dmd_set_low_page (dst.u.second);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src));
	dmd_or_page ();

	wpc_dmd_set_mapped (dst);
}


/**
 * Allocate and map a new pair of DMD pages that are initialized
 * with the current mapped contents.
 */
void dmd_dup_mapped (void)
{
	dmd_pagepair_t old, new;

	old = wpc_dmd_get_mapped ();
	dmd_alloc_pair ();
	new = wpc_dmd_get_mapped ();

	wpc_dmd_set_low_page (old.u.second);
	dmd_copy_low_to_high ();

	wpc_dmd_set_low_page (old.u.first);
	wpc_dmd_set_high_page (new.u.first);
	dmd_copy_low_to_high ();

	wpc_dmd_set_mapped (new);
}


