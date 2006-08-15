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
gen_multisquare_background (void)
{
	PGM *main, *box;

	main = pgm_alloc ();
	pgm_set_four_color (main);

	box = pgm_alloc ();
	pgm_set_four_color (box);
	pgm_resize (box, 32, 16);

	pgm_fill (box, PGM_DARK (box));
	pgm_paste (main, box, 5, 5);

	pgm_fill (box, PGM_BRIGHT (box));
	pgm_paste (main, box, 80, 11);

	pgm_fill (box, PGM_DARK (box));
	pgm_paste (main, box, 64, 8);

	pgm_fill (box, PGM_BRIGHT (box));
	pgm_paste (main, box, 12, 14);

	pgm_write_xbmset (main, "images/msqback.xbm", "msqback");

	pgm_free (box);
	pgm_free (main);
}

