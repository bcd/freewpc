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

#ifndef __PGMLIB_H
#define __PGMLIB_H

#include <stdio.h>

#define MAX_WIDTH 128
#define MAX_HEIGHT 32
#define MAX_MAXVAL 15
#define MAX_XBMSET_PLANES 4
#define MAX_XBMSET_FRAMES 256

#define PGM_COLOR(pgm, percent)	((unsigned int)(((pgm)->maxval) * (percent)))
#define PGM_BLACK(pgm)	PGM_COLOR(pgm, 0.0)
#define PGM_DARK(pgm)	PGM_COLOR(pgm, 0.34)
#define PGM_BRIGHT(pgm)	PGM_COLOR(pgm, 0.67)
#define PGM_WHITE(pgm)	PGM_COLOR(pgm, 1.0)

#define pgm_set_plane_count(pgm,n)	pgm_set_maxval (pgm, ((1 << (n)) - 1))
#define pgm_set_mono(pgm)				pgm_set_plane_count (pgm, 1)
#define pgm_set_four_color(pgm)		pgm_set_plane_count (pgm, 2)
#define pgm_copy(dst, src)				pgm_paste (dst, src, 0, 0)

#define XBMSET_ALL_PLANES -1

#define xbmset_name(xbmset, name)	xbmset->c_name = name

#define XBM_WRITE_HEADER 0x1

typedef struct {
	unsigned int width;
	unsigned int height;
	unsigned int maxval;
	unsigned int bits[MAX_HEIGHT][MAX_WIDTH];
} PGM;


typedef struct {
	unsigned int width;
	unsigned int height;
	unsigned int bytes[MAX_HEIGHT][MAX_WIDTH / 8];
} XBM;

typedef struct {
	const char *c_name;
	unsigned int n_planes;
	unsigned int frame_count;
	XBM *planes[MAX_XBMSET_PLANES][MAX_XBMSET_FRAMES];
} XBMSET;

#define XBMOP_MAX_LITERALS	0x20
#define XBM_LITERAL_P(op) \
	(((op) >= XBMOP_LITERAL) && ((op) < XBM_OP_LITERAL+XBMOP_MAX_LITERALS))
#define XBM_LITERAL_SIZE(op)	((op)+1)

typedef enum {
	/* Opcode 0x00-0x1F is followed by literal data.  The amount is embedded
	in the opcode and can be from 1 to 32 bytes */
	XBMOP_LITERAL=0x0,

	/* Opcode 0x20-0x3F is followed by a single byte of literal data,
	which repeats up to 32 times in a row */
	XBMOP_REPEAT_BYTE=0x20,

	/* Opcode 0x40-0x5F is followed by a single word of literal data,
	which repeats up to 32 times in a row (for up to 64 bytes) */
	XBMOP_REPEAT_WORD=0x40,

	/* Opcode 0x60-0x7F indicates up to 32 bytes needs to be "skipped",
	with nothing done at those locations */
	XBMOP_SKIP=0x60,

	/* Opcode 0x80-0x8F precedes a sequence of up to 16 bytes, that defines
	a character map. */
	XBMOP_CHARMAP_DEFINE=0x80,

	/* Opcode 0x90-0xCF spans 64 possible values.  Each of these values
	refers to a 2-element set, with each element having eight possible
	values.  Those values refer to the character map.  This allows semi-random
	data to be compressed more efficiently. */
	XBMOP_CHARMAP_REF=0xA0,

	/* Opcode 0xFF indicates end of image */
	XBMOP_EOI=0xEF,
} XBMOP;

typedef struct xbm_prog_elem {
	struct xbm_prog_elem *prev, *next;
	XBMOP op;
	union {
		struct {
			unsigned int count;
			unsigned int bytes[16];
		} literal;
		struct {
			unsigned int count;
			unsigned int data;
		} repeat;
		struct {
			unsigned int count;
		} skip;
		struct {
		} set_cursor;
	} args;
	struct {
		unsigned int flags;
		unsigned int size;
	} stats;
} XBMPROG;


#define pgm_make_xbmset(pgm)  pgm_append_xbmset (NULL, pgm)

XBMSET * pgm_append_xbmset (XBMSET *xbmset, PGM *pgm);
int xbm_unique_byte_count (XBM * xbm);
void xbm_write_stats (FILE *fp, XBM *xbm);
void xbmset_write (FILE *fp, XBMSET *xbmset, int plane, int write_flags);
void xbmset_free (XBMSET *xbmset);
XBM *xbmset_plane (XBMSET *xbmset, int plane);
XBMPROG *xbm_make_prog (XBM *xbm);
void xbmprog_write (FILE *fp, const char *name, int plane, XBMPROG *xbmprog);
void xbmprog_free (XBMPROG *prog);

PGM * pgm_alloc (void);
void pgm_free (PGM *pgm);
void pgm_resize (PGM *pgm, unsigned int width, unsigned int height);
void pgm_set_maxval (PGM *pgm, unsigned int depth);
void pgm_write (PGM *pgm, const char *filename);
void pgm_draw_pixel (PGM *pgm, unsigned int x, unsigned int y, unsigned int val);
unsigned int pgm_read_pixel (PGM *pgm, unsigned int x, unsigned int y);
void pgm_draw_hline (PGM *pgm, unsigned int x1, unsigned int x2, unsigned int y, unsigned int val);
void pgm_draw_vline (PGM *pgm, unsigned int x, unsigned int y1, unsigned int y2, unsigned int val);
void pgm_draw_box (PGM *pgm, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int val);
void pgm_draw_border (PGM *pgm, unsigned int width, unsigned int val);
void pgm_fill_box (PGM *pgm, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int val);
void pgm_fill (PGM *pgm, unsigned int val);
void pgm_translate (PGM *dst, PGM *src, int xshift, int yshift);
void pgm_paste (PGM *dst, PGM *src, unsigned int xpos, unsigned int ypos);
void pgm_scale (PGM *pgm, double factor);
void pgm_xor (PGM *dst, PGM *src1, PGM *src2);
void pgm_write_xbmset (PGM *pgm, const char *filename, const char *name);
void pgm_write_xbm (PGM *pgm, const char *filename, const char *name, int plane);
void pgm_change_maxval (PGM *pgm, unsigned int new_maxval);
PGM * pgm_read (const char *filename);
void pgm_dither (PGM *pgm, unsigned int new_maxval);
void pgm_invert (PGM *pgm);

extern void machgen (void);

#endif /* __PGMLIB_H */
