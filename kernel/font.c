/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/** A global structure that describes all of the attributes for
the next font rendering: location, font, and string.  Using this
avoids passing long argument lists to lots of functions. */
__fastram__ fontargs_t font_args;

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

U8 top_space;

__fastram__ U8 *blit_dmd;

#ifndef __m6809__
__fastram__ const U8 *bitmap_src;
#endif


extern const font_t font_bitmap_common;


/* Returns a pointer to the glyph data for a character 'c'
 * in the font 'font'.  This points directly to the raw bytes
 * that can be ORed into the display.
 *
 * This function also sets some global variables related to
 * the characteristics of this character:
 * font_width : number of bits wide, including spacing
 * font_height: number of bits high.
 */
U8 *font_lookup (const font_t *font, char c)
{
	if (unlikely (c == ' '))
	{
		/* TODO : if every font had an entry for the 'space' character,
		this test could be removed */
		char *data = font->glyphs[(U8)'I'];
		font_width = *data++;
		font_height = 1;
		return (font_space);
	}
	else
	{
		char *data = font->glyphs[(U8)c];
		font_width = *data++;
		font_height = *data++;
		return ((U8 *)data);
	}
}

#ifndef __m6809__

/** Draw one row of font data to the DMD.
 * DST is the byte-aligned pointer to where the bits should be drawn.
 *
 * BYTE_WIDTH is the number of bytes of font data to be written.  It is the
 * character width in bits rounded up to the next multiple of 8, minus
 * spacing.
 *
 * SHIFT says how many bits to the right that the characters should be
 * shifted.  When zero, it is straightforward.  When nonzero, the bits
 * are shifted on the fly, and WIDTH+1 bytes must actually be modified.
 *
 * This is an internal function that is expanded 8 times, for all possible
 * values of shift.
 */
static inline void font_blit_internal (U8 *dst, U8 byte_width, const U8 shift)
{
	register const U8 *src = bitmap_src;

	do {
		if (shift == 0)
		{
			*dst ^= *src;
		}
		else
		{
			dst[0] ^= *src << shift;
			dst[1] = (*src >> (8-shift)) ^ dst[1];
		}
	
		src++;
		bitmap_src = src;
		dst++;
	} while (--byte_width);
}


static void font_blit0 (U8 *dst)
{
	font_blit_internal (dst, font_byte_width, 0);
}

static void font_blit1 (U8 *dst)
{
	font_blit_internal (dst, font_byte_width, 1);
}

static void font_blit2 (U8 *dst)
{
	font_blit_internal (dst, font_byte_width, 2);
}

static void font_blit3 (U8 *dst)
{
	font_blit_internal (dst, font_byte_width, 3);
}

static void font_blit4 (U8 *dst)
{
	font_blit_internal (dst, font_byte_width, 4);
}

static void font_blit5 (U8 *dst)
{
	font_blit_internal (dst, font_byte_width, 5);
}

static void font_blit6 (U8 *dst)
{
	font_blit_internal (dst, font_byte_width, 6);
}

static void font_blit7 (U8 *dst)
{
	font_blit_internal (dst, font_byte_width, 7);
}

void (*font_blit_table[]) (U8 *) = {
	font_blit0,
	font_blit1,
	font_blit2,
	font_blit3,
	font_blit4,
	font_blit5,
	font_blit6,
	font_blit7,
};

#endif /* !__m6809__ */

/** Renders a string whose characteristics have already been
 * computed.  font_args contains the font type, starting
 * coordinates (from the upper left), and pointer to the string
 * data. */
static void fontargs_render_string (void)
{
	static U8 *dmd_base;
	static const char *s;
	char c;
	fontargs_t *args = &font_args;
#ifndef __m6809__
	void (*blitter) (U8 *);
#endif

	dmd_base = ((U8 *)dmd_low_buffer) + args->coord.y * DMD_BYTE_WIDTH;
	s = sprintf_buffer;

	/* When running in native mode, there is no DMD.  However it
	is useful to know what text the program is trying to display,
	so output the text string to the console instead.  Note that
	there is no return here, so the remaining code is still executed,
	and the 'virtual' DMD buffer is indeed written to, although it
	can't actually be seen. */
#ifdef CONFIG_UI
	if (args->font != &font_bitmap_common)
		ui_write_dmd_text (args->coord.x, args->coord.y, s);
#endif

	/* Font data is stored in a separate page of ROM; switch
	 * there to be able to read the font data */
	wpc_push_page (FONT_PAGE);

	top_space = 0;

	/* Loop over every character in the string. */
	while ((c = *s++) != '\0')
	{
		U8 *blit_dmd;

		/* TODO - if the character is a space, much of this can be
		 * bypassed and we only need to shift the output pointer
		 * by a small amount. */

		bitmap_src = font_lookup (args->font, c);
#ifndef __m6809__
		font_byte_width = (font_width + 7) >> 3;
#endif

		/* If the height of this glyph is not the same as the
		height of the overall string, then the character should
		be bottom aligned.  This is needed for commas and periods.
		The starting address is moved down the required number
		of rows.  The amount of space added is saved away so that
		it can be reclaimed later. */
		if (unlikely (font_height < args->font->height))
		{
			top_space = (args->font->height - font_height);
			top_space *= DMD_BYTE_WIDTH;
			dmd_base += top_space;
		}

		/* Set the starting address */
		blit_dmd = wpc_dmd_addr_verify (dmd_base + args->coord.x / 8);

		/* Write the character. */
#ifdef __m6809__
		bitmap_blit_asm (blit_dmd, args->coord.x & 0x7);
#else
		/* The glyph is drawn one row at a time.
		 * TODO - this is pretty inefficient.  Several things could be done
		 * better:
		 *    When a glyph is more than 8 bits wide and unaligned, we are
		 *    performing way more reads and writes than necessary.  We
		 *    have to read a byte, set only the affected bits, write it back.
		 *    Some sort of pipelined approach where we only write to the
		 *    DMD memory when we are done with a byte would be better.
		 *
		 *    We only write 1 byte at a time.  For >8 bits wide, we can
		 *    do better writing 16-bits at a time.
		 */
		blitter = font_blit_table[args->coord.x & 0x7];
		do
		{
			/* TODO : font_blit is applicable to more than just
			fonts; it could be used for arbitrary-sized bitmaps. */
			blitter (wpc_dmd_addr_verify (blit_dmd));
			blit_dmd += DMD_BYTE_WIDTH;
		} while (likely (--font_height)); /* end for each row */
#endif

		/* advance by 1 char ... args->font->width */
		args->coord.x += font_width + 1;

		/* If the height was adjusted just for this character, restore
		back to the original starting row */
		if (unlikely (top_space != 0))
		{
			dmd_base -= top_space;
			top_space = 0;
		}

#ifndef __m6809__
		/* Because the C code is slow, assert that everything is OK so
		the software watchdog doesn't expire. */
		task_dispatching_ok = TRUE;
#endif

	} /* end for each character in the string */
	wpc_pop_page ();
}


/** Draw a bitmap to an arbitrary screen location.  The image is
a single color and drawn into the low-mapped display page.
The format of the image data is the same as for a font glyph:
the first byte is its bit-width, the second byte is its
bit-height, and the remaining bytes are the image data, going
from top to bottom and then left to right.  Also, the image
data must reside in FONT_PAGE for now. */
void bitmap_blit (const U8 *_bitmap_src, U8 x, U8 y)
{
	U8 *dmd_base = ((U8 *)dmd_low_buffer) + y * DMD_BYTE_WIDTH;
#ifndef __m6809__
	void (*blitter) (U8 *);
	U8 i, j;
#endif

	bitmap_src = _bitmap_src;
	wpc_push_page (FONT_PAGE);
	font_width = *bitmap_src++;
#ifndef __m6809__
	font_byte_width = (font_width + 7) >> 3;
#endif
	font_height = *bitmap_src++;
	blit_dmd = wpc_dmd_addr_verify (dmd_base + (x / 8));

#ifdef __m6809__
	bitmap_blit_asm (blit_dmd, x & 0x7);
#else
	blitter = font_blit_table[x / 8];
	for (i=0; i < font_height; i++)
	{
		for (j=0; j < font_byte_width; j++)
		{
			blitter (blit_dmd);
			blit_dmd = wpc_dmd_addr_verify (blit_dmd + 1);
		}
		blit_dmd = wpc_dmd_addr_verify (blit_dmd - font_byte_width);
		blit_dmd = wpc_dmd_addr_verify (blit_dmd + DMD_BYTE_WIDTH);
	}
#endif

	wpc_pop_page ();
}


/** Draw a single color bitmap onto both the low and high
mapped display pages. */
void bitmap_blit2 (const U8 *_bitmap_src, U8 x, U8 y)
{
	bitmap_blit (_bitmap_src, x, y);
	dmd_flip_low_high ();
	bitmap_blit (_bitmap_src, x, y);
	dmd_flip_low_high ();
}


#ifdef NOTUSED
/** Erase an arbitrary region of the DMD.  coord gives the
upper-left corner of the region.  width and height specify the size. */
void blit_erase (union dmd_coordinate coord, U8 width, U8 height)
{
	U8 *dmd_base;
	static U8 xr;
	static U8 partial_left[] = { 
		0x0, 0x7f, 0x3f, 0x1f, 0x0f, 0x7, 0x3, 0x1 };
	static U8 partial_right[] = { 
		0x0, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80 };
	static U16 hoffset;
	S16 hoff;

	dmd_base = ((U8 *)dmd_low_buffer) + coord.y * DMD_BYTE_WIDTH +
		coord.x / 8;
	xr = coord.x % 8;
	hoffset = height * DMD_BYTE_WIDTH;
	
	if (xr)
	{
		/* Erase partial left region */
		U8 mask = partial_left[(8 - xr)];
		width -= xr;
		for (hoff=hoffset; hoff >= 0; hoff -= DMD_BYTE_WIDTH)
			dmd_base[hoff] &= mask;
		dmd_base++;
	}

	while (width >= 16)
	{
		/* Erase middle region */
		for (hoff=hoffset; hoff >= 0; hoff -= DMD_BYTE_WIDTH)
			((U16 *)dmd_base)[hoff] = 0;
		dmd_base += 2;
		width -= 16;
	}

	while (width >= 8)
	{
		/* Erase middle region */
		for (hoff=hoffset; hoff >= 0; hoff -= DMD_BYTE_WIDTH)
			dmd_base[hoff] = 0;
		dmd_base++;
		width -= 8;
	}

	if (width)
	{
		/* Erase partial right region */
		U8 mask = partial_right[width];
		for (hoff=hoffset; hoff >= 0; hoff -= DMD_BYTE_WIDTH)
			dmd_base[hoff] &= mask;
	}
}
#endif /* NOTUSED */


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
		/* TODO - use a real strcpy() here that is more efficient */
		char *ram = sprintf_buffer;
		do {
			*ram++ = *s;
		} while (*s++ != '\0');
		s = sprintf_buffer;
	}

	wpc_push_page (FONT_PAGE);

	font_string_width = 0;
	font_string_height = 0;

	while ((c = *s++) != '\0')
	{
		/* Decode the width/height of the character. */
		(void)font_lookup (font, c);

		/* Update the total width */
		font_string_width += font_width + 1;

		/* Update the total height */
		if (font_height > font_string_height)
			font_string_height = font_height;
	}

	/* Don't count the space at the end of the string */
	font_string_width--;

	wpc_pop_page ();

	/* This can take a while for long strings; don't let the
	software watchdog expire */
	task_dispatching_ok = TRUE;
}


static void fontargs_prep_left (void)
{
	font_get_string_area (font_args.font, font_args.s);
}

static void fontargs_prep_center (void)
{
	font_get_string_area (font_args.font, font_args.s);
	font_args.coord.x = font_args.coord.x - (font_string_width / 2);
	font_args.coord.y = font_args.coord.y - (font_string_height / 2);
}

static void fontargs_prep_right (void)
{
	font_get_string_area (font_args.font, font_args.s);
	font_args.coord.x = font_args.coord.x - font_string_width;
}


void fontargs_render_string_left (void)
{
	fontargs_prep_left ();
	fontargs_render_string ();
}


void fontargs_render_string_center (void)
{
	fontargs_prep_center ();
	fontargs_render_string ();
}


void fontargs_render_string_right (void)
{
	fontargs_prep_right ();
	fontargs_render_string ();
}


void fontargs_render_string_left2 (void)
{
	fontargs_prep_left ();
	fontargs_render_string ();
	font_args.coord.x -= font_string_width + 1;
	dmd_flip_low_high ();
	/* TODO - currently, to draw brightest text in 4-color
	 * mode, we go through the entire process of rendering the
	 * text twice, once per page, since the underlying routine
	 * only knows how to draw on the low mapped page.  Better
	 * to modify the underlying routines to support 4-color mode,
	 * or to do a quicker bitmap copy without decoding the font
	 * again. */
	fontargs_render_string ();
	dmd_flip_low_high ();
}


void fontargs_render_string_center2 (void)
{
	fontargs_prep_center ();
	fontargs_render_string ();
	font_args.coord.x -= font_string_width + 1;
	dmd_flip_low_high ();
	fontargs_render_string ();
	dmd_flip_low_high ();
}


void fontargs_render_string_right2 (void)
{
	fontargs_prep_right ();
	fontargs_render_string ();
	font_args.coord.x -= font_string_width + 1;
	dmd_flip_low_high ();
	fontargs_render_string ();
	dmd_flip_low_high ();
}


/** Draw a bitmap from the 'symbol' font onto the display.
The character selects which symbol to be drawn. */
void bitmap_draw (union dmd_coordinate coord, U8 c)
{
	sprintf_buffer[0] = c;
	sprintf_buffer[1] = '\0';

	font_args.font = &font_bitmap_common;
	font_args.coord = coord;
	font_args.s = sprintf_buffer;

	fontargs_render_string ();
}

