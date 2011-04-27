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
 * Apply a mono overlay onto a mono page.
 */
void dmd_overlay (void)
{
	dmd_pagepair_t dst = wpc_dmd_get_mapped ();
	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, DMD_OVERLAY_PAGE);
	dmd_or_page ();
	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, dst.u.second);
}


/**
 * Apply a color overlay onto the current color pages.
 */
void dmd_overlay_color (void)
{
	dmd_pagepair_t dst = wpc_dmd_get_mapped ();
	pinio_dmd_window_set (PINIO_DMD_WINDOW_0, dst.u.second);
	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, DMD_OVERLAY_PAGE+1);
	dmd_or_page ();

	pinio_dmd_window_set (PINIO_DMD_WINDOW_0, dst.u.first);
	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, DMD_OVERLAY_PAGE);
	dmd_or_page ();

	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, dst.u.second);
}


/**
 * Apply a mono overlay onto the current color pages.
 */
void dmd_overlay_onto_color (void)
{
	dmd_pagepair_t dst = wpc_dmd_get_mapped ();
	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, DMD_OVERLAY_PAGE);

	pinio_dmd_window_set (PINIO_DMD_WINDOW_0, dst.u.second);
	dmd_or_page ();

	pinio_dmd_window_set (PINIO_DMD_WINDOW_0, dst.u.first);
	dmd_or_page ();

	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, dst.u.second);
}

