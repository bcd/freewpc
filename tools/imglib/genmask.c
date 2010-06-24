/*
 * Copyright 2008, 2010 by Brian Dominy <brian@oddchange.com>
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

#include "imglib.h"


enum output_mode
{
	ASCII,
	XBM,
	PGM,
};

enum output_mode finish_mode = ASCII;

int pause_after_each_finish = 0;
int delay_after_each_finish = 0;


void gen_finish (struct buffer *buf)
{
	switch (finish_mode)
	{
		case ASCII:
			bitmap_write_ascii (buf, stdout);
			break;

		case XBM:
			buf = buffer_replace (buf, buffer_joinbits (buf));
			buffer_write_xbm (buf, "mask", stdout);
			break;
	}

	buffer_free (buf);

	if (pause_after_each_finish)
		getchar ();
	else if (delay_after_each_finish)
		usleep (50 * 1000);
}


void gen_frame_mask (void)
{
	struct buffer *buf = frame_alloc ();
	bitmap_draw_border (buf, 2);
	buf = buffer_replace (buf, buffer_invert (buf));
	gen_finish (buf);
}


void gen_tile_example (void)
{
	struct buffer *tile, *bmp;

	tile = bitmap_alloc (8, 8);
	bitmap_draw_box (tile, 0, 0, 4, 4);
	//bitmap_write_ascii (tile, stdout);

	bmp = frame_alloc ();
	bitmap_tile (bmp, tile);

	buffer_free (tile);
	gen_finish (bmp);
}


/**
 * Generate a test pattern that shows all 4 colors at once.
 */
void gen_test_pattern (void)
{
	struct buffer *buf = frame_alloc ();
	unsigned int color;
	const unsigned int max_colors = 4;
	const unsigned int section_width = FRAME_WIDTH / max_colors;
	struct buffer *plane;

	for (color = 0; color < max_colors; color++)
	{
		bitmap_fill_region (buf,
			color * section_width, 0,
			(color+1) * section_width, FRAME_HEIGHT,
			color);
	}

	plane = bitmap_extract_plane (buf, 0);
	gen_finish (plane);
	plane = bitmap_extract_plane (buf, 1);
	gen_finish (plane);
	buffer_free (buf);
}


void gen_test_font (void)
{
	struct buffer *buf = frame_alloc ();
	bitmap_write_text_center (buf, &font_fixed10, 64, 16, "HELLO WORLD");
	gen_finish (buf);
}


void gen_test_animation (void)
{
	unsigned int x = 0;
	for (x = 0; x < 32; x++)
	{
		struct buffer *buf = frame_alloc ();
		bitmap_write_text (buf, &font_fixed10, x*2, x, "HELLO WORLD");
		bitmap_draw_ellipse (buf, 128-x*2, 32-x, 8, 8);
		gen_finish (buf);
	}
}


void gen_test_alpha (void)
{
	struct buffer *buf = frame_alloc ();
	buf->alpha = 0xFF;
	bitmap_fill (buf, 0xFF);

	bitmap_set_color (buf, 0);
	bitmap_draw_box (buf, 0, 0, 9, 9);

	bitmap_set_color (buf, 1);
	bitmap_draw_box (buf, 1, 1, 8, 8);
	gen_finish (buf);
}


int main (int argc, char *argv[])
{
	int argn = 1;

	while (argn < argc)
	{
		const char *arg = argv[argn++];
		if (*arg == '-')
		{
			arg++;
			switch (*arg)
			{
				case 'x':
					finish_mode = XBM;
					break;
				case 'p':
					pause_after_each_finish = 1;
					break;
				case 'D':
					delay_after_each_finish = 1;
					break;
			}
		}
	}


	//gen_frame_mask ();
	//gen_test_pattern ();
	//gen_tile_example ();
	//gen_test_font ();
	//gen_test_animation ();
	gen_test_alpha ();
	exit (0);
}
