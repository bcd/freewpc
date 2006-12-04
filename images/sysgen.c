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

#define TRUE 1
#define FALSE 0
#include "pgmlib.h"

void
pgm_to_xbmset (const char *name, int invert_flag)
{
	char infilename[64];
	char outfilename[64];

	sprintf (infilename, "images/%s.pgm", name);
	sprintf (outfilename, "images/%s.xbm", name);

	PGM *pgm = pgm_read (infilename);
	pgm_change_maxval (pgm, 3);

	pgm_dither (pgm, 3);

	if (invert_flag)
		pgm_invert (pgm);
	pgm_write_xbmset (pgm, outfilename, name);
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
	pgm_to_xbmset ("cow", FALSE);
	pgm_to_xbmset ("hitcher", TRUE);
	pgm_to_xbmset ("coinstack", FALSE);
	pgm_to_xbmset ("oldcar", FALSE);

	gen_mborder ();
	gen_multisquare_background ();
	gen_dithered_dark_background ();
	gen_tile_background ();
	gen_ball_border ();
}

