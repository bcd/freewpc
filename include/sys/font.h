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
 *  A descriptor that encapsulates a font, a position
 *  on the display, and a constant string to be rendered
 *  there.  It is primarily to avoid passing too many
 *  arguments to subroutines.
 */
typedef struct
{
	const font_t *font;
	union dmd_coordinate coord;
	const char *s;
} fontargs_t;

extern __fastram__ fontargs_t font_args;

void font_lookup_char (const font_t *font, char c);

void font_get_string_area (const font_t *font, const char *s);

void fontargs_render_string_center (void);
void fontargs_render_string_right (void);
void fontargs_render_string_left (void);

/* The _2 versions will render the string to both mapped pages
(low and high), and are more efficient than writing twice by hand. */
void fontargs_render_string_center2 (void);
void fontargs_render_string_right2 (void);
void fontargs_render_string_left2 (void);

void bitmap_blit (const U8 *blit_data, U8 x, U8 y);
void bitmap_blit2 (const U8 *blit_data, U8 x, U8 y);
void bitmap_draw (union dmd_coordinate coord, U8 c);

#define MKCOORD1(x,y) (((U16)(x)<<8)|(y))
#define MKCOORD(x,y) { .xy = MKCOORD1(x,y), }

#define DECL_FONTARGS(_f,_x,_y,_s) \
	font_args.font = _f; \
	font_args.coord.xy = MKCOORD1 (_x, _y); \
	font_args.s = _s;


#define font_render_string_left(f,x,y,s) \
{ \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_left (); \
}

#define font_render_string font_render_string_left


#define font_render_string_center(f,x,y,s) \
{ \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_center (); \
}

#define font_render_string_right(f,x,y,s) \
{ \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_right (); \
}

#define font_render_string_center2(f,x,y,s) \
{ \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_center2 (); \
}

#endif /* _SYS_FONT_H */
