/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SYS_FONT_H
#define _SYS_FONT_H


/**
 * The font descriptor structure.
 */
typedef struct font
{
	/** The preferred spacing between characters, in bits */
	U8 spacing;

	/** The normal height of all characters, in bits */
	U8 height;

	/** A pointer to the table of glyphs */
	char **glyphs;

	/** The first character that has a glyph.  Earlier characters
	 * are not represented at all, thus saving space.  Typically,
	 * there is nothing before a space, which saves 64 bytes.
	 * Old fonts that have not been converted to use this feature
	 * will have a value of zero here. */
	U8 basechar;
} font_t;



/**
 * An identifier for a symbol, which is just a character within the
 * special symbol font.
 */
typedef enum {
	BM_FIRST=0, /* force first value to be '1' */

	BM_BOX3, BM_X3, BM_PLUS3,

	BM_BOX5, BM_X5,

	BM_LEFT_ARROW5, BM_RIGHT_ARROW5,

	BM_LAST,
} symbol_t;


/** A DMD coordinate, sometimes given as separate x and y values,
 * and sometimes combined into a single 16-bit number. */
union dmd_coordinate {
	U16 xy;
	struct {
#ifdef CONFIG_BIG_ENDIAN
		U8 x;
		U8 y;
#else
		U8 y;
		U8 x;
#endif
	};
};


/**
 *  The font rendering arguments -- which font to use and
 *  where to position the upperleftmost pixel.  This static
 *  structure is used primarily to avoid passing too many
 *  arguments to subroutines, which is not well supported
 *  on the 6809.
 */
typedef struct
{
	const font_t *font;
	union dmd_coordinate coord;
} fontargs_t;

extern __fastram__ fontargs_t font_args;


void font_lookup_char (const font_t *font, char c);
void fontargs_render_string_center (const char *);
void fontargs_render_string_right (const char *);
void fontargs_render_string_left (const char *);
void bitmap_blit (const U8 *blit_data, U8 x, U8 y);
void bitmap_blit2 (const U8 *blit_data, U8 x, U8 y);
void fontargs_render_glyph (U8 c);

/**
 * Helper macros for packing two 8-bit coordinates
 * into a single 16-bit, or vice versa.
 */
#define MKCOORD1(x,y) (((U16)(x)<<8)|(y))
#define MKCOORD(x,y) { .xy = MKCOORD1(x,y), }


/**
 * An internal macro for writing all of the font arguments that
 * aren't passed as actual parameters into the global
 * 'font_args' structure.
 */
#define DECL_FONTARGS(_f,_x,_y,_s) \
	font_args.font = _f; \
	font_args.coord.xy = MKCOORD1 (_x, _y); \

/**
 * The top-level API for writing a string, left justified.
 */
#define font_render_string_left(f,x,y,s) \
do { \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_left (s); \
} while (0)
#define font_render_string font_render_string_left


/**
 * The top-level API for writing a string, centered about the
 * given coordinate.
 */
#define font_render_string_center(f,x,y,s) \
do { \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_center (s); \
} while (0)


/**
 * The top-level API for writing a string, right justified.
 * The pixel given is the upperrightmost.
 */
#define font_render_string_right(f,x,y,s) \
do { \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_right (s); \
} while (0)


#define font_render_glyph(f,x,y,c) \
do { \
	DECL_FONTARGS(f,x,y,sprintf_buffer); \
	fontargs_render_glyph (c); \
} while (0)

#define bitmap_draw(_coord, _c) \
do { \
	font_args.font = &font_symbol; \
	font_args.coord = _coord; \
	fontargs_render_glyph (_c); \
} while (0)

#endif /* _SYS_FONT_H */
