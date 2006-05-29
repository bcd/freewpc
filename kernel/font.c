
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

#include <freewpc.h>

/**
 * \file
 * \brief Routines for writing text to the DMD in a particular font.
 *
 * The font functions take strings, which can be true C constants or
 * a pointer to a buffer that is already formatted, and renders the
 * string at a particular location in a particular font.
 *
 * Coordinate values are given in pixels.
 *
 * Three variants exist, one for each justification: left, right, or
 * center.
 */


/* Space characters are not embedded in each font, because it would
 * just be a bunch of zero characters.  All fonts share the same
 * space glyph data, given here */
static U8 font_space[16] = { 0, };


#if 1
#define GET_FONT_SPACING(font)	1
#else
#define GET_FONT_SPACING(font)	(font->spacing)
#endif


fontargs_t font_args;

/** The width of the current character being rendered in bits */
__fastram__ U8 font_width;

/** The width of the current character being rendered in bytes */
__fastram__ U8 font_byte_width;

/** The height of the current character being rendered */
__fastram__ U8 font_height;

/** The total width of the current string being rendered */
U8 font_string_width;

/** The overall height of the current string being rendered, which
 * is the maximum height of all its characters */
U8 font_string_height;


/* Returns a pointer to the glyph data for a character 'c'
 * in the font 'font'.  This points directly to the raw bytes
 * that can be ORed into the display.
 *
 * This function also sets some global variables related to
 * the characteristics of this character:
 * font_width : number of bits wide, including spacing
 * font_byte_width: number of bytes wide, excluding spacing
 * font_height: number of bits high.
 */
U8 *font_lookup (const font_t *font, char c)
{
	if (c == ' ')
	{
		U8 *data = font->glyphs[(U8)'I'];
		font_width = *data++;
		font_byte_width = (font_width + 7) >> 3;
		font_width += GET_FONT_SPACING (font);
		font_height = *data++;
		return (font_space);
	}
	else
	{
		U8 *data = font->glyphs[(U8)c];
		font_width = *data++;
		font_byte_width = (font_width + 7) >> 3;
		font_width += GET_FONT_SPACING (font);
		font_height = *data++;
		return (data);
	}
}


/** Renders a string whose characteristics have already been
 * computed.  font_args contains the font type, starting
 * coordinates (from the upper left), and pointer to the string
 * data. */
static void fontargs_render_string (void)
{
#ifndef GCC4
	static U8 *dmd_base;
	static const char *s;
	U8 x;
	char c;
	const fontargs_t *args = &font_args;

	dmd_base = ((U8 *)dmd_low_buffer) + args->y * DMD_BYTE_WIDTH;
	s = sprintf_buffer;
  	x = args->x;

	/* Font data is stored in a separate page of ROM; switch
	 * there to be able to read the font data */
	wpc_push_page (FONT_PAGE);

	while ((c = *s) != '\0')
	{
		static U8 i, j;
		static U8 xb;
		static U8 top_space;
		register U8 *data;
		register U8 *dmd;

		/* Nonprintable characters are skipped. */
		if (c < ' ')
			continue;

		data = font_lookup (args->font, c);

		if (font_height < args->font->height)
		{
			top_space = (args->font->height - font_height);
			top_space *= DMD_BYTE_WIDTH;
			dmd_base += top_space;
		}
		else
			top_space = 0;

		xb = x / 8;

		for (i=0; i < font_height; i++)
		{
			task_dispatching_ok = TRUE;
			for (j=0; j < font_byte_width; j++)
			{
				dmd = dmd_base + xb + i * DMD_BYTE_WIDTH + j;
				switch (x % 8)
				{
					default: /* should not happen */
					case 0:
						dmd[0] = *data++;
						break;
					case 1:
						dmd[0] |= *data << 1;
						dmd[1] = (*data >> 7) | dmd[1];
						data++;
						break;
					case 2:
						dmd[0] |= *data << 2;
						dmd[1] = (*data >> 6) | dmd[1];
						data++;
						break;
					case 3:
						dmd[0] |= *data << 3;
						dmd[1] = (*data >> 5) | dmd[1];
						data++;
						break;
					case 4:
						dmd[0] |= *data << 4;
						dmd[1] = (*data >> 4) | dmd[1];
						data++;
						break;
					case 5:
						dmd[0] |= *data << 5;
						dmd[1] = (*data >> 3) | dmd[1];
						data++;
						break;
					case 6:
						dmd[0] |= *data << 6;
						dmd[1] = (*data >> 2) | dmd[1];
						data++;
						break;
					case 7:
						dmd[0] |= *data << 7;
						dmd[1] = (*data >> 1) | dmd[1];
						data++;
						break;
				}
			}
		}

		/* advance by 1 char ... args->font->width */
		x += font_width;
		s++;
		if (top_space != 0)
			dmd_base -= top_space;
	}
	wpc_pop_page ();
#endif /* GCC4 */
}


/** Calculate font_string_width and font_string_height
 * in advance for a particular string to be rendered in a
 * particular font.  This is needed when doing centered or
 * right-justified writing, in order to calculate how to translate
 * the input coordinates to the real starting coordinates. */
void font_get_string_area (const font_t *font, const char *s)
{
	U8 c;

	/* Copy the string to be rendered into the RAM buffer,
	 * if it is not already there.  This allows us to be
	 * more efficient later on, as page flipping between
	 * static strings and the font data is not required.
	 */
	if (s != sprintf_buffer)
	{
		char *ram = sprintf_buffer;
		while (*s != '\0')
			*ram++ = *s++;
		*ram = '\0';
		s = sprintf_buffer;
	}

	wpc_push_page (FONT_PAGE);

	font_string_width = 0;
	font_string_height = 0;

	while ((c = *s++) != '\0')
	{
		(void)font_lookup (font, c);
		font_string_width += font_width;
		if (font_height > font_string_height)
			font_string_height = font_height;
	}

	wpc_pop_page ();
	task_dispatching_ok = TRUE;
}


void fontargs_render_string_left (const fontargs_t *args)
{
	font_get_string_area (args->font, args->s);
	if (args != &font_args)
	{
		font_args.x = args->x;
		font_args.y = args->y;
		font_args.font = args->font;
		font_args.s = args->s;
	}
	fontargs_render_string ();
}


void fontargs_render_string_center (const fontargs_t *args)
{
	font_get_string_area (args->font, args->s);
	font_args.x = args->x - (font_string_width / 2);
	font_args.y = args->y - (font_string_height / 2);
	if (args != &font_args)
	{
		font_args.font = args->font;
		font_args.s = args->s;
	}
	fontargs_render_string ();
}


void fontargs_render_string_right (const fontargs_t *args)
{
	font_get_string_area (args->font, args->s);
	font_args.x = args->x - font_string_width;
	if (args != &font_args)
	{
		font_args.y = args->y;
		font_args.font = args->font;
		font_args.s = args->s;
	}
	fontargs_render_string ();
}

