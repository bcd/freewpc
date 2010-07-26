/*
 * Copyright 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief A library of image generation and manipulation functions.
 */

#ifndef _IMGLIB_H
#define _IMGLIB_H

#include <freewpc.h>

#define FRAME_WIDTH 128
#define FRAME_HEIGHT 32

#define MAX_BUFFER_SIZE (FRAME_WIDTH * FRAME_HEIGHT)

#define FRAME_BYTE_SIZE (FRAME_WIDTH * FRAME_HEIGHT / 8)

#define max(a,b) (((a) > (b)) ? (a) : (b))

#define abs(n) (((n) >= 0) ? (n) : -(n))

#define square(n) ((n) * (n))


struct img_histogram
{
	unsigned int count[256];
	unsigned int most_frequent[256];
	unsigned int unique;
};

struct buffer
{
	unsigned int len;
	unsigned int width, height;
	U8 *data;
	U8 _data[MAX_BUFFER_SIZE];
	struct img_histogram *hist;

	/** The current color used by all drawing functions. */
	U8 color;

	/**
	 * The color which indicates transparency.
	 * This is only used by layering functions.
	 */
	U8 alpha;

	/**
	 * The type of the buffer.
	 */
	U8 type;
};

struct coord
{
	int x;
	int y;
};


struct layer
{
	struct buffer *bitmap;
	struct buffer *buf;
	struct coord coord;
};


/**
 * A unary operator maps two pixel values to another.
 */
typedef U8 binary_operator (U8, U8);

/**
 * A unary operator maps one pixel value to another.
 */
typedef U8 unary_operator (U8);

/**
 * A translation operator maps one coordinate to another.
 */
typedef struct coord translate_operator (struct coord);

struct buffer *buffer_alloc(unsigned int maxlen);
struct buffer *buffer_clone (struct buffer *buf);
struct buffer *buffer_copy (struct buffer *buf);
struct buffer *bitmap_alloc(unsigned int width, unsigned int height);
struct buffer *frame_alloc(void);
unsigned int bitmap_pos(struct buffer *buf, unsigned int x, unsigned int y);
void buffer_read(struct buffer *buf, FILE *fp);
void buffer_write(struct buffer *buf, FILE *fp);
char enhanced_pixel_ascii (unsigned int pixel);
void bitmap_write_ascii(struct buffer *buf, FILE *fp);
void buffer_read_pgm(struct buffer *buf, FILE *fp);
void buffer_write_pgm(struct buffer *buf, FILE *fp);
void buffer_write_c(struct buffer *buf, FILE *fp);
void cdecl_begin(const char ident[], FILE *fp);
void cdecl_end(FILE *fp);
void buffer_free(struct buffer *buf);
U8 xor_operator(U8 a, U8 b);
U8 and_operator(U8 a, U8 b);
U8 com_operator(U8 a);
struct buffer *buffer_binop(struct buffer *a, struct buffer *b, binary_operator op);
struct buffer *buffer_unop(struct buffer *buf, unary_operator op);
struct buffer *buffer_joinbits(struct buffer *buf);
struct buffer *buffer_splitbits(struct buffer *buf);
int buffer_compare(struct buffer *a, struct buffer *b);
struct buffer *buffer_replace(struct buffer *old, struct buffer *new);
struct img_histogram *histogram_update(struct buffer *buf);
struct buffer *buffer_compress(struct buffer *buf, struct buffer *prev);
struct buffer *buffer_decompress(struct buffer *buf);
struct buffer *buffer_rle_encode (struct buffer *buf);
struct buffer *buffer_sparse_encode (struct buffer *buf);
struct buffer *bitmap_crop(struct buffer *buf);
void bitmap_draw_pixel(struct buffer *buf, unsigned int x, unsigned int y);
void bitmap_draw_line(struct buffer *buf, int x1, int y1, int x2, int y2);
void bitmap_draw_box(struct buffer *buf, int x1, int y1, int x2, int y2);
void bitmap_draw_border(struct buffer *buf, unsigned int width);
struct coord zoom_out_translation(struct coord c);
struct buffer *bitmap_translate(struct buffer *buf, translate_operator op);
void bitmap_fill(struct buffer *buf, U8 val);
void bitmap_fill_region (struct buffer *buf, int x1, int y1, int x2, int y2, U8 val);
struct buffer *bitmap_extract_plane (struct buffer *buf, unsigned int plane);
struct buffer *bitmap_combine_planes (struct buffer *buf[], unsigned int planes);
struct buffer *fif_decode(struct buffer *buf, unsigned int plane);
struct buffer *binary_fif_read(const char *filename);
void bitmap_finish(struct buffer *buf);
void bitmap_zoom_out(struct buffer *buf);

#define buffer_invert(buf) buffer_unop(buf, com_operator)

extern const font_t mono5;
extern const font_t mono9;
extern const font_t fixed10;

struct glyph
{
	U8 width;
	U8 height;
	U8 bits[0];
};

void bitmap_write_glyph (struct buffer *bmp, const struct glyph *g, unsigned int x, unsigned int y);
void bitmap_write_text (struct buffer *bmp, const font_t *font, unsigned int x, unsigned int y, const char *text);

#endif /* _IMGLIB_H */

