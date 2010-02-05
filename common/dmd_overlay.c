/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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
 * \brief Dot-matrix overlay functions
 *
 */

#include <freewpc.h>

/**
 * Overlay the contents of two DMD pages (SRC and SRC+1)
 * onto two other DMD pages, described by DST.
 * The destination pages do not have to be consecutive.
 *
 * On output, the destination pages are also mapped.
 */
void dmd_overlay_color (dmd_pagepair_t dst, U8 src)
{
	wpc_dmd_set_low_page (dst.u.second);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src) + 1 );
	dmd_or_page ();

	wpc_dmd_set_low_page (dst.u.first);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src) );
	dmd_or_page ();

	wpc_dmd_set_high_page (dst.u.second);
}


/**
 * Overlay the contents of a single DMD page, in SRC,
 * onto two other DMD pages, described by DST.
 * The destination pages do not have to be consecutive.
 *
 * On output, the destination pages are also mapped.
 */
void dmd_overlay_onto_color (dmd_pagepair_t dst, U8 src)
{
	wpc_dmd_set_high_page ( dmd_get_lookaside (src) );

	wpc_dmd_set_low_page (dst.u.second);
	dmd_or_page ();

	wpc_dmd_set_low_page (dst.u.first);
	dmd_or_page ();

	wpc_dmd_set_high_page (dst.u.second);
}


