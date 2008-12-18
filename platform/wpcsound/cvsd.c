/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>

/** A pointer to the next byte of CVSD data to send */
U8 *cvsd_output;

/** A pointer to one past the end of the CVSD data.  When
the output pointer reaches this value, the CVSD is stopped. */
U8 *cvsd_end;

/** The page of ROM in which the CVSD data comes from. */
U8 cvsd_page;

U8 cvsd_data;

U8 cvsd_count;


/** Service the CVSD device, by sending at most a single bit. */
void cvsd_service (void)
{
}


void cvsd_start (U8 *start, U8 *end, U8 page)
{
	cvsd_data = far_read8 (start++, page);
	cvsd_output = start;
	cvsd_end = end;
	cvsd_count = 8;
}


void cvsd_stop (void)
{
	cvsd_count = 0;
	cvsd_output = NULL;
}

