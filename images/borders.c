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

/**
 * \file
 * \brief Generates some generic frame borders.
 *
 */


/* Generate the multi-color border */
void
gen_mborder (void)
{
	PGM *pgm;
	XBMSET *xbmset;
	FILE *fp;

	pgm = pgm_alloc ();
	pgm_set_plane_count (pgm, 2);
	pgm_draw_border (pgm, 3, PGM_DARK(pgm));
	pgm_draw_border (pgm, 2, PGM_BRIGHT(pgm));
	pgm_draw_border (pgm, 1, PGM_WHITE(pgm));
	pgm_write (pgm, "images/mborder.pgm");
	pgm_write_xbmset (pgm, "images/mborder.xbm", "mborder");
	pgm_free (pgm);
}


void
gen_ball_border (void)
{
	PGM *pgm[3];
	int i, x, y;
	PGM *main;
	int next_ball = 0;

#define next ({ next_ball = (next_ball+1) % 3; pgm[next_ball]; })

	for (i=0; i < 3; i++)
	{
		unsigned int color = i+1;
		pgm[i] = pgm_alloc ();
		pgm_set_plane_count (pgm[i], 2);
		pgm_resize (pgm[i], 4, 4);
		pgm_draw_pixel (pgm[i], 1, 0, color);
		pgm_draw_pixel (pgm[i], 0, 1, color);
		pgm_draw_pixel (pgm[i], 1, 1, color);
		pgm_draw_pixel (pgm[i], 2, 1, color);
		pgm_draw_pixel (pgm[i], 1, 2, color);
	}

	main = pgm_alloc ();
	pgm_set_plane_count (main, 2);

	for (x=0; x < 128; x += 8)
		pgm_paste (main, next, x, 0);
	for (x=128-4; x >= 0; x -= 8)
		pgm_paste (main, next, x, 31-4);

	pgm_write (main, "images/ballborder.pgm");
	pgm_write_xbmset (main, "images/ballborder.xbm", "ballborder");
	pgm_free (main);
}

