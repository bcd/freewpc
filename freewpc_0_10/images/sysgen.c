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

#include "pgmlib.h"

void
pgm_read_test (void)
{
	PGM *pgm = pgm_read ("images/brianhead.pgm");
	pgm_change_maxval (pgm, 3);
	// pgm_dither (pgm, 3);
	pgm_invert (pgm);
	pgm_write_xbmset (pgm, "images/readtest.xbm", "readtest");
}


/* machgen is the real main function for the xbm generator.
 * The actual main() is in the library in pgmlib.c, which
 * then calls this function after the library has been
 * initialized.
 *
 * Each gen_xxx() function here creates an XBMSET file with
 * .xbm extension, that is capable of being displayed by the
 * FreeWPC runtime DMD module.
 */
void
machgen (void)
{
#if 1
	pgm_read_test ();
#endif
	gen_mborder ();
	gen_multisquare_background ();
	gen_dithered_dark_background ();
	gen_tile_background ();
	gen_ball_border ();
}

