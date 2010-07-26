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

#include "imglib.h"

/*
 * \file
 * \brief A library of image manipulation functions.
 * The buffer_xxx functions operate on arbitrary buffers.  You can put
 * anything you want in there.  It's simply a convenient way of tracking
 * data plus a length.  There are a few other things in there for
 * convenience.
 *
 * The bitmap_xxx functions work on buffers, too, but assume that each
 * byte corresponds to a single pixel.  Thus, you can have up to 256
 * colors in a bitmap.  Bitmaps are laid out from row 1 to row N,
 * with the columns in each row given left to right.  Bitmaps require
 * that the 'width' and 'height' fields be set in the buffer so that
 * correlation between (x,y) locations are byte offsets is possible.
 *
 * A frame is just a bitmap of size 128x32, the size of the entire
 * WPC dot matrix display.
 *
 * A joined bitmap is one in which each group of 8 adjacent pixels within
 * one row are combined into a single byte.  Joining requires a bitmap
 * of a single color only (each pixel is either 00h or 01h).  This is the
 * format that can actually be copied directly to the DMD memory.
 */

#define CACHE_SIZE 32

struct buffer *buffer_cache[CACHE_SIZE] = { 0, };

unsigned int buffer_cache_count = 0;


static struct buffer *buffer_cache_alloc (void)
{
	if (buffer_cache_count == 0)
		return NULL;
	return buffer_cache[--buffer_cache_count];
}


static void buffer_cache_free (struct buffer *buf)
{
	if (buffer_cache_count < CACHE_SIZE)
		buffer_cache[buffer_cache_count++] = buf;
	else
		free (buf);
}


/**
 * Allocate a new buffer of MAXLEN bytes.
 */
struct buffer *buffer_alloc (unsigned int maxlen)
{
	struct buffer *buf;

	buf = buffer_cache_alloc ();
	if (!buf)
		buf = malloc (sizeof (struct buffer));

	buf->len = maxlen;
	buf->data = buf->_data;
	buf->hist = NULL;
	buf->width = buf->height = 0;
	buf->color = 1;
	buf->type = 0;
	memset (buf->_data, 0, maxlen);
	return buf;
}


/**
 * Like buffer_alloc(), but uses the attribute of another
 * buffer to determine the initial size of the new buffer.
 * This is also a simplified version of buffer_copy(),
 * in which no pixel data is actually copied.  The intent
 * would be for functions that will redraw the entire
 * buffer contents anyway.
 */
struct buffer *buffer_clone (struct buffer *buf)
{
	struct buffer *copy = buffer_alloc (buf->len);
	copy->width = buf->width;
	copy->height = buf->height;
	copy->color = buf->color;
	return copy;
}


struct buffer *buffer_copy (struct buffer *buf)
{
	struct buffer *copy = buffer_clone (buf);
	memcpy (copy->_data, buf->_data, buf->len);
	if (buf->hist)
		histogram_update (copy);
	return copy;
}


struct buffer *bitmap_alloc (unsigned int width, unsigned int height)
{
	struct buffer *buf = buffer_alloc (width * height);
	buf->width = width;
	buf->height = height;
	buf->alpha = 0;
	return buf;
}


struct buffer *frame_alloc (void)
{
	return bitmap_alloc (FRAME_WIDTH, FRAME_HEIGHT);
}

struct layer *layer_alloc (struct buffer *bitmap)
{
	struct layer *layer = malloc (sizeof (struct layer));
	layer->bitmap = bitmap;
	layer->buf = frame_alloc ();
	layer->coord.x = layer->coord.y = 0;
	return layer;
}

unsigned int bitmap_pos (struct buffer *buf, unsigned int x, unsigned int y)
{
	return y * buf->width + x;
}


void buffer_read (struct buffer *buf, FILE *fp)
{
	buf->len = fread (buf->data, sizeof (U8), buf->len, fp);
	/* TODO - not handling errors very well */
}

void buffer_write (struct buffer *buf, FILE *fp)
{
	fwrite (buf->data, sizeof (U8), buf->len, fp);
}

char default_pixel_ascii (struct buffer *buf, unsigned int pixel)
{
	if (pixel != 0)
	{
		if (pixel == buf->alpha)
			return '?';
		return pixel + 'X' - 1;
	}
	else
		return ' ';
}


char enhanced_pixel_ascii (unsigned int pixel)
{
	switch (pixel)
	{
		default: case 0: return ' ';
		case 1: return ':';
		case 2: return '*';
		case 3: return '#';
	}
}


void bitmap_write_ascii (struct buffer *buf, FILE *fp)
{
	unsigned int x, y;

	if (buf->height * buf->width < buf->len)
	{
		fprintf (stderr, "error: cannot render %dx%d ASCII image with only %d bytes\n",
			buf->width, buf->height, buf->len);
		exit (1);
	}

	for (y = 0; y < buf->height; y++)
	{
		for (x = 0; x < buf->width; x++)
			fputc (enhanced_pixel_ascii (buf->data[bitmap_pos (buf, x, y)]), fp);
		fputc ('\n', fp);
	}
}

void buffer_write_c (struct buffer *buf, FILE *fp)
{
	unsigned int off;

	if (buf->hist)
	{
		fprintf (fp, "/* Histogram (%d unique values)\n", buf->hist->unique);
		if (buf->hist->unique < 16)
		{
			for (off = 0; off < buf->hist->unique; off++)
			{
				U8 val = buf->hist->most_frequent[off];
				unsigned int count = buf->hist->count[val];
				fprintf (fp, " * %d occurrences of %02X\n",
					count, val);
			}
		}
		fprintf (fp, " */\n");
	}

	fprintf (fp, "/* %dx%d %d bytes */\n", buf->width, buf->height, buf->len);

	for (off = 0; off < buf->len; off++)
	{
		if (!(off % 16))
			fprintf (fp, "\n   ");
		fprintf (fp, "0x%02X,", buf->data[off]);
	}
	fprintf (fp, "\n");
}


/* Writes a 'joined bitmap' to an XBM. */
void buffer_write_xbm (struct buffer *buf, const char *ident, FILE *fp)
{
	fprintf (fp, "#define %s_width %d\n", ident, buf->width);
	fprintf (fp, "#define %s_height %d\n", ident, buf->height);
	fprintf (fp, "const unsigned char %s[] = {\n", ident);
	buffer_write_c (buf, fp);
	fprintf (fp, "}\n");
}


/* Read a bitmap from a file in PGM file format. */
void buffer_read_pgm (struct buffer *buf, FILE *fp)
{
	char line[80];
	unsigned int x, y;
	unsigned int c;
	unsigned int newlen = 0;
	char *token = NULL;

	fgets (line, 79, fp); /* magic */
	fgets (line, 79, fp); /* comment */

	fgets (line, 79, fp);
	sscanf (line, "%d %d", &buf->width, &buf->height);

	fgets (line, 79, fp);
	/* sscanf (line, "%d", &pgm->maxval); */
	/* TODO - the max val is not being communicated back, so
	 * it is impossible to do proper scaling. */

	for (y=0; y < buf->height; y++)
		for (x=0; x < buf->width; x++)
		{
			if (token == NULL)
			{
nextline:
				fgets (line, 79, fp);
				if (feof (fp))
					goto done;
				token = strtok (line, " \t\r\n");
			}
			else
			{
				token = strtok (NULL, " \t\r\n");
				if (token == NULL)
					goto nextline;
			}

			c = strtoul (token, NULL, 10);
			//printf ("x,y = %d, %d\n", x, y);
			buf->data[bitmap_pos (buf, x, y)] = c;
			newlen++;
		}
done:
	buf->len = newlen;
}


/* Write a bitmap to a file in PGM file format. */
void buffer_write_pgm (struct buffer *buf, FILE *fp)
{
	unsigned int row, col;
	fprintf (fp, "P2\n");
	fprintf (fp, "#\n");
	fprintf (fp, "%d %d\n", buf->width, buf->height);
	fprintf (fp, "255\n");
	for (row=0; row < buf->height; row++)
		for (col=0; col < buf->width; col++)
			fprintf (fp, "%d\n", buf->data[bitmap_pos (buf, col, row)]);
}


void cdecl_begin (const char ident[], FILE *fp)
{
	fprintf (fp, "unsigned char %s[] = {", ident);
}

void cdecl_end (FILE *fp)
{
	fprintf (fp, "};\n\n");
}


void buffer_free (struct buffer *buf)
{
	if (buf->hist)
		free (buf->hist);
	buffer_cache_free (buf);
}

void layer_free (struct layer *layer)
{
	/* note: layer->bitmap is not freed */
	buffer_cache_free (layer->buf);
	free (layer);
}


U8 xor_operator (U8 a, U8 b)
{
	return a ^ b;
}

U8 and_operator (U8 a, U8 b)
{
	return a & b;
}

U8 com_operator (U8 a)
{
	return a ? 0 : 1;
}

/** Performs a binary operation on two buffers.
 * Byte-by-byte, the operation is applied to each pair of corresponding
 * bytes. */
struct buffer *buffer_binop (struct buffer *a, struct buffer *b,
	binary_operator op)
{
	unsigned int off = 0;
	struct buffer *res = buffer_alloc (max (a->len, b->len));

	res->width = a->width;
	res->height = b->height;

	while (off < a->len && off < b->len)
	{
		res->data[off] = op (a->data[off], b->data[off]);
		off++;
	}
	return res;
}


/** Performs a unary operation on a buffer.  Returns the result in a new
 * buffer.  Byte-by-byte, the operation is applied to the source to
 * determine the corresponding byte of the result.
 */
struct buffer *buffer_unop (struct buffer *buf, unary_operator op)
{
	unsigned int off = 0;
	struct buffer *res = buffer_alloc (buf->len);

	res->width = buf->width;
	res->height = buf->height;

	while (off < buf->len)
	{
		res->data[off] = op (buf->data[off]);
		off++;
	}
	return res;
}


/** Compute the delta (exclusive-OR) between two buffers. */
struct buffer *buffer_compute_delta (struct buffer *dst, struct buffer *src)
{
	return buffer_binop (dst, src, xor_operator);
}


/** Created a joined buffer.  The input image must use 8 bits per pixel and
 * have a single color (0 or 1) per pixel.  The output image has 1 bit per pixel;
 * 8 consecutive pixels are joined together into a single byte. */
struct buffer *buffer_joinbits (struct buffer *buf)
{
	unsigned int off, bit;
	struct buffer *res = buffer_alloc (buf->len / 8);

	for (off = 0; off < buf->len / 8; off++)
	{
		U8 val = 0;
		for (bit = 0; bit < 8; bit++)
			if (buf->data[off * 8 + bit] == 1)
				val |= 1 << bit;
		res->data[off] = val;
	}

	res->width = buf->width;
	res->height = buf->height;
	return res;
}


/** Create a split buffer from a joined one.  This is the reverse process of
 * buffer_joinbits(); see above.
 * The output image contains only 0s and 1s as pixel values.
 */
struct buffer *buffer_splitbits (struct buffer *buf)
{
	unsigned int off;
	struct buffer *res = buffer_alloc (buf->len * 8);
	unsigned char *src;
	unsigned char *dst;

	src = buf->data;
	dst = res->data;

	for (off = 0; off < buf->len; off++)
	{
		if (*src & 0x1)
			*dst = 1;
		dst++;
		if (*src & 0x2)
			*dst = 1;
		dst++;
		if (*src & 0x4)
			*dst = 1;
		dst++;
		if (*src & 0x8)
			*dst = 1;
		dst++;
		if (*src & 0x10)
			*dst = 1;
		dst++;
		if (*src & 0x20)
			*dst = 1;
		dst++;
		if (*src & 0x40)
			*dst = 1;
		dst++;
		if (*src & 0x80)
			*dst = 1;
		dst++;
		src++;
	}
	return res;
}


int buffer_compare (struct buffer *a, struct buffer *b)
{
	unsigned int off;

	if (a->len != b->len)
		return 1;
	for (off = 0; off < a->len; off++)
		if (a->data[off] != b->data[off])
		{
#ifdef DEBUG
			printf ("difference at offset %d (%02X vs. %02X)\n",
				off, a->data[off], b->data[off]);
#endif
			return 1;
		}
	return 0;
}


struct buffer *buffer_replace (struct buffer *old, struct buffer *new)
{
	if (old != new)
		buffer_free (old);
	return new;
}


struct img_histogram *histogram_update (struct buffer *buf)
{
	struct img_histogram *hist;
	unsigned int off;
	unsigned int i;
	int largest;
	unsigned int already_taken[256] = { 0, };

	if (!buf->hist)
		hist = buf->hist = malloc (sizeof (struct img_histogram));

	hist->unique = 0;
	for (off = 0; off < 256; off++)
		hist->count[off] = 0;

	for (off = 0; off < buf->len; off++)
	{
		if (hist->count[buf->data[off]] == 0)
			hist->unique++;
		hist->count[buf->data[off]]++;
	}

	for (i = 0; i < hist->unique; i++)
	{
		/* Find the ith largest value in the list that hasn't already
		been selected */
		largest = -1;

		/* Iterate through the count data, looking for the highest
		entry that has not already been selected from previous loops. */
		for (off = 0; off < 256; off++)
		{
			/* Always consider the first entry, but then only consider
			entries larger than previous ones. */
			if ((largest == -1) || (hist->count[off] > hist->count[largest]))
			{
				/* But skip entries that were taken before */
				if (!already_taken[off])
					largest = off;
			}
		}
		hist->most_frequent[i] = largest;
		already_taken[largest] = 1;
	}

	return hist;
}


/********************************************************************/

/* Compression scheme:

   Byte 0 of the compressed buffer contains flags:
		7:6	Init method
			Says how to initialize the page buffer before
			decompressing:
				00 = No initialization.
				01 = Copy previous image.
				10 = Clean page with all zeroes.
				11 = Reserved.
		5:4	Encryption method
			Says how to decrypt the following bytes:
				00 = No decryption.  Bytes are copied as-is.
						512 bytes must follow.
            01 = Run-Length Encoding.  512 bytes are
						described, but runs of consecutive bytes
						are compressed into a value/count pair.
				10 = Palette Encoding.  This is used when the
				      number of unique bytes in the image is small.
						The palette format is shown below.
				11 = Reserved.
		3		Patch flag
			When this bit is set, after decryption one or more
			'patches' are to be applied.  These are offset/value
			pairs which are used to fixup certain locations.
			This is intended for cases where RLE could be improved
			by damaging the input data in certain ways; and these
			patches are corrections to fix the damage.

	Beginning at byte 1, the encryption method as specified
	in [5:4] is run.  This area is variable-sized.  There is no
	explicit length given here.

	Following the encoded area, if bit 3 is set, are one or more
	patches.  Each patch is 3-bytes in length:
		Byte 0 = offset from previous patch, in bytes.  Up to
			255 positions can be advanced this way.  If more is
			needed, a 'fake patch' that patches identical data as
			before is required.  An offset of zero means no
			more patches, and the following bytes do not occur.
		Byte 1/2 = the 16-bit value to be placed in the image.

	When the init method is 'copy from previous', this affects
	how data and patches are applied.  In this case, we use XOR
	operations rather than plain stores.

   When using the palette method, the data layout is as follows:
	   Byte 1:	Number of palette entries (at most 16)
		Byte 2:  Palette - depending on the length above, each
		         byte here maps to a 2-bit (if 4 or less) or 4-bit
					encoded value.
	   Byte 2+N: The encoded data, either 2 or 4 bytes/encoded byte.
 */

#define CH_INIT_NONE 0x00
#define CH_INIT_COPY 0x40
#define CH_INIT_CLEAN 0x80
#define CH_INIT_MASK 0xC0

#define CH_ENCODE_NONE 0x00
#define CH_ENCODE_RLE 0x10
#define CH_ENCODE_PALETTE 0x20
#define CH_ENCODE_MASK 0x30

#define CH_PATCH 0x08

static U8 *buffer_write_run (U8 *ptr, U8 sentinel, U8 data, unsigned int count)
{
	if (count == 0);
	else if (count < 4)
	{
		do {
			*ptr++ = data;
		} while (--count > 0);
	}
	else
	{
		*ptr++ = sentinel;
		*ptr++ = data;
		*ptr++ = count;
	}

	return ptr;
}


/**
 * Return the estimated size of the image if it were compressed using
 * the palette method.
 */
unsigned int palette_compression_length (struct img_histogram *hist)
{
	if (hist->unique > 16)
		return 10000;

	else if (hist->unique <= 4)
		return (FRAME_BYTE_SIZE / 4) + hist->unique + 1 + 1;

	else
		return (FRAME_BYTE_SIZE / 2) + hist->unique + 1 + 1;
}


/**
 * Given a joined bitmap, return a compressed version.
 * 'buf' is the buffer to be compressed.
 *
 * A non-NULL represents the previous image in an
 * animation.  The compressor will see if the new image can be
 * represented better as a delta from the previous.
 */
struct buffer *buffer_compress (struct buffer *buf, struct buffer *prev)
{
	struct buffer *rle;
	U8 *rleptr;
	unsigned int last, last_count;
	int n;
	U8 sentinel;

	/* Update the image histogram */
	histogram_update (buf);

	/* Find a byte value that does not occur in the image */
	for (n = 0; n <= 0xFF; n++)
	{
		if (buf->hist->count[n] == 0)
		{
			sentinel = n;
			break;
		}
	}

	/* Compute the run length encoded version of the buffer.
	We'll assume for now that just RLE is good enough.
	We will check that assumption later and see how good it really was. */
	rle = buffer_alloc (buf->len);
	rleptr = rle->data;
	last = 1000;
	last_count = 0;

	*rleptr++ = CH_ENCODE_RLE;
	*rleptr++ = sentinel;

	for (n = 0; n < buf->len ; n++)
	{
		if ((buf->data[n] == last) && (last_count < 255))
		{
			last_count++;
		}
		else
		{
			rleptr = buffer_write_run (rleptr, sentinel, last, last_count);
			last = buf->data[n];
			last_count = 1;
		}
	}
	rleptr = buffer_write_run (rleptr, sentinel, last, last_count);
	rle->len = rleptr - rle->data;

	/* See if delta encoding is better */
	if (prev != NULL)
	{
		struct buffer *delta = buffer_compress (prev, NULL);
		if (delta->len < rle->len)
		{
			/* TODO : it's possible that it needs to be _significantly_
			smaller for this to be worthwhile... */
		}
	}

#ifdef DEBUG
	printf ("Unique values in original = %d\n", buf->hist->unique);
	printf ("RLE encoded version = %d bytes\n", rle->len);
	printf ("Palette compression = %d bytes\n",
		palette_compression_length (buf->hist));
#endif
	return rle;
}


/**
 * Given a compressed bitmap, return the uncompressed, joined buffer.
 */
struct buffer *buffer_decompress (struct buffer *buf)
{
	struct buffer *res;
	U8 flags;
	U8 sentinel;
	U8 *inptr = buf->data;
	U8 *outptr;
	U8 val, count;

	res = buffer_alloc (MAX_BUFFER_SIZE);
	outptr = res->data;

	flags = *inptr++;

	switch (flags & CH_INIT_MASK)
	{
		case CH_INIT_NONE:
		case CH_INIT_COPY:
			break;

		case CH_INIT_CLEAN:
			break;
	}

	if ((flags & CH_ENCODE_MASK) == CH_ENCODE_RLE)
	{
		sentinel = *inptr++;
		while (outptr - res->data < FRAME_BYTE_SIZE)
		{
			val = *inptr++;
			if (val == sentinel)
			{
				val = *inptr++;
				count = *inptr++;
				do {
					*outptr++ = val;
				} while (--count > 0);
			}
			else
			{
				*outptr++ = val;
			}
		}
	}

	if (flags & CH_PATCH)
	{
	}

	res->len = outptr - res->data;
	return res;
}


/**
 * Encode a joined bitmap using run-length encoding (RLE).
 *
 * Returns a new buffer with the encoded version.
 */
struct buffer *buffer_rle_encode (struct buffer *buf)
{
	U8 *dstp;
	const U8 *srcp;
	struct buffer *res;
	U8 b;

	res = buffer_clone (buf);
	srcp = buf->data;
	dstp = res->data;

	while (srcp < buf->data + 512)
	{
		b = *srcp++;

		/* The character 'A8' has special meaning; if it occurs in the input
		stream, a special escape sequence must be emitted. */
		if (b == 0xA8)
		{
			*dstp++ = 0xA8;
			*dstp++ = 0x00;
			*dstp++ = *srcp++;
		}
		else
		{
			/* See how many following bytes are the same */
			int run = 0;
			while ((srcp + run < buf->data + 512) && (srcp[run] == b))
				run++;
			/* Include the current byte in the count, too */
			run++;

			/* If this byte begins a run of 4 or more of the same value, then
			it can be emitted in a more compact form. */
			if (run >= 4)
			{
				run &= ~1; /* round down to multiple of 2 */
				*dstp++ = 0xA8;
				*dstp++ = run / 2;
				*dstp++ = b;
				srcp += run-1;
			}
			else
			{
				/* None or not enough to do RLE, just copy literal */
				*dstp++ = b;
				*dstp++ = *srcp++;
			}
		}
	}

	/* Add final 0xA880 to mark end of frame */
	*dstp++ = 0xA8;
	*dstp++ = 0x80;
	res->len = dstp - res->data;
	res->type |= 0x2;
	return res;
}


struct buffer *buffer_rle_decode (struct buffer *buf)
{
	struct buffer *res;

	res = buffer_alloc (512);
	/* TODO */
	res->width = buf->width;
	res->height = buf->height;
	return res;
}


struct buffer *buffer_sparse_encode (struct buffer *buf)
{
	U8 *dstp, *srcp;
	struct buffer *res;
	int zeroes, count;

	res = buffer_clone (buf);
	srcp = buf->data;
	dstp = res->data;

	/* Encode the buffer as a series of <skip, count, data...> tuples.
	 * SKIP indicates the number of zero bytes that are omitted from
	 * the image.  COUNT says how many literal 16-bit words follow.
	 */
	while (srcp < buf->data + 512)
	{

		/* See how many following bytes are zero.
		 * Strip them from the input.
		 * The skip value (zeroes) cannot exceed 126 for the 6809,
		 * due to the way the algorithm is written in assembler.  Detect this,
		 * and emit a 'dummy block' which skips as much as possible.
		 */
		zeroes = 0;
		while (*srcp == 0 && zeroes < 126 && srcp < buf->data + 512)
		{
			zeroes++;
			srcp++;
		}

		/* See how many following bytes are zero.
		 * Don't strip them from the input yet. */
		count = 0;
		while (srcp[count] != 0 && srcp + count < buf->data + 512)
		{
			count++;
		}

		/* The size of each literal block needs to be word aligned.
		 * If not, add a zero back in, preferably from the amount
		 * just skipped, otherwise from the following data. */
		if (count & 1)
		{
			if (zeroes != 0)
			{
				zeroes--;
				srcp--;
			}
			count++;
		}

		/* Write the block */
		*dstp++ = count / 2;
		if (count > 0)
		{
			*dstp++ = zeroes;
			memcpy (dstp, srcp, count);
			dstp += count;
			srcp += count;
		}
	}

	/* Add final 0x00 to mark end of frame */
	*dstp++ = 0;
	res->len = dstp - res->data;
	res->type |= 0x4;
	return res;
}

/********************************************************************/


struct buffer *bitmap_crop (struct buffer *buf)
{
	struct buffer *res = buffer_alloc (buf->len);
	/* TODO */
	return res;
}

void bitmap_set_color (struct buffer *buf, unsigned int color)
{
	buf->color = color;
}


/**
 * Draw a single pixel at (x,y) in a bitmap.
 * The color of the pixel is determined by the last call
 * to bitmap_set_color().
 * All attempts to draw to an invalid location will be ignored.
 */
void bitmap_draw_pixel (struct buffer *buf, unsigned int x, unsigned int y)
{
	unsigned int pos;

	if (x >= buf->width || y >= buf->height)
		return;

	pos = bitmap_pos (buf, x, y);
	if (pos < buf->len)
		buf->data[pos] = buf->color;
}


/**
 * Paste one bitmap onto another.
 * SRC is the source bitmap.  The alpha channel of the source is
 *    obeyed, and can be used to see through to the destination.
 * DST is the destination bitmap.
 * XOFF and YOFF state the location, relative to the destination,
 * where the source should begin.
 */
struct buffer *bitmap_paste (struct buffer *dst, struct buffer *src,
	unsigned int xoff, unsigned int yoff)
{
	unsigned int sx, sy;
	for (sx = 0; sx < src->width; sx++)
	{
		for (sy = 0; sy < src->height; sy++)
		{
			U8 pixel = src->data[bitmap_pos(src, sx, sy)];
			if (pixel != src->alpha)
				bitmap_draw_pixel (dst, sx + xoff, sy + yoff);
		}
	}
	return dst;
}


/**
 * Fill a bitmap with SRC, replicating it as many times as
 * necessary to fill the destination.
 */
struct buffer *bitmap_tile (struct buffer *dst, struct buffer *src)
{
	unsigned int dx, dy;
	for (dx = 0; dx < dst->width; dx++)
	{
		for (dy = 0; dy < dst->height; dy++)
		{
			U8 pixel = src->data[bitmap_pos(src, dx % src->width, dy % src->height)];
			bitmap_set_color (dst, pixel);
			bitmap_draw_pixel (dst, dx, dy);
		}
	}
	return dst;
}


/**
 * Draw a line segment from (x1,y1) to (x2,y2).
 */
void bitmap_draw_line (struct buffer *buf,
	int x1, int y1,
	int x2, int y2)
{
	float xm, ym;

	/* The approach used here is recursive divide-and-conquer:
	1) Calculate the midpoint of the line to be drawn.
	2) Draw a line from one end to the midpoint.
	3) Draw a line from the other end to the midpoint.

	The recursion is terminated by looking for cases where
	the line is to be drawn is only 1 or 2 pixels in width. */

	if (abs (x2-x1) <= 1 && abs (y2-y1) <= 1)
	{
		bitmap_draw_pixel (buf, x1, y1);
		bitmap_draw_pixel (buf, x1, y2);
		bitmap_draw_pixel (buf, x2, y1);
		bitmap_draw_pixel (buf, x2, y2);
		return;
	}

	/* Find the midpoint of the line */
	xm = (x1 + x2 + 0.5) / 2;
	ym = (y1 + y2 + 0.5) / 2;

	bitmap_draw_line (buf, x1, y1, xm, ym);
	bitmap_draw_line (buf, x2, y2, xm, ym);
}


void bitmap_draw_ellipse (struct buffer *buf,
	int x, int y, int rx, int ry)
{
	int px, py;

	/* Check each coordinate in the bounding box */
	for (px = x - rx; px <= x + rx; px++)
		for (py = y - ry; py <= y + ry; py++)
			if ((square (abs (px - x)) + square (abs (py - y))) < square (rx))
				bitmap_draw_pixel (buf, px, py);
}


void bitmap_draw_box (struct buffer *buf,
	int x1, int y1,
	int x2, int y2)
{
	/* A box is just a series of four lines. */
	bitmap_draw_line (buf, x1, y1, x2, y1);
	bitmap_draw_line (buf, x1, y1, x1, y2);
	bitmap_draw_line (buf, x2, y1, x2, y2);
	bitmap_draw_line (buf, x1, y2, x2, y2);
}


void bitmap_draw_border (struct buffer *buf, unsigned int width)
{
	unsigned int i;

	/* For each pixel-width, draw one box. */
	for (i = 0; i < width; i++)
		bitmap_draw_box (buf, i, i, buf->width - i - 1, buf->height - i - 1);
}


struct coord zoom_out_translation (struct coord c)
{
	struct coord center = { .x = 64, .y = 16 };
	c.x = c.x + (center.x - c.x) * 0.2;
	c.y = c.y + (center.y - c.y) * 0.2;
	return c;
}


/**
 * Translate one buffer to another.
 *
 * This function applies a transformation function to the
 * coordinates of all pixels in the source image.  The pixel
 * is then written to the output image at the new location.
 */
struct buffer *bitmap_translate (struct buffer *buf, translate_operator op)
{
	struct buffer *res = buffer_clone (buf);
	unsigned int x, y;

	for (x = 0; x < buf->width; x++)
		for (y = 0 ; y < buf->height; y++)
		{
			struct coord c = { .x = x, .y = y };
			c = op (c);

			res->color = buf->data[bitmap_pos (buf, x, y)];

			bitmap_draw_pixel (res, c.x, c.y);
		}
	return res;
}


/**
 * Fill an entire bitmap with a specific color.
 */
void bitmap_fill (struct buffer *buf, U8 val)
{
	unsigned int x, y;
	buf->color = val;
	for (x = 0; x < buf->width; x++)
		for (y = 0; y < buf->height; y++)
			bitmap_draw_pixel (buf, x, y);
}


/**
 * Fill a rectangular region of a bitmap with a specific color.
 */
void bitmap_fill_region (struct buffer *buf,
	int x1, int y1,
	int x2, int y2, U8 val)
{
	unsigned int x, y;
	buf->color = val;
	for (x = x1; x < x2; x++)
		for (y = y1; y < y2; y++)
			bitmap_draw_pixel (buf, x, y);
}


/**
 * Given a bitmap, return a new bitmap in which each pixel is 1 if the
 * PLANEth bit of the source is 1, else it is 0.
 */
struct buffer *bitmap_extract_plane (struct buffer *buf, unsigned int plane)
{
	unsigned int x, y;
	struct buffer *res = buffer_alloc (buf->len);
	res->height = buf->height;
	res->width = buf->width;

	for (x = 0; x < buf->width; x++)
		for (y = 0; y < buf->height; y++)
		{
			unsigned int color = buf->data[bitmap_pos (buf, x, y)];
			res->color = (color & (1 << plane)) ? 1 : 0;
			bitmap_draw_pixel (res, x, y);
		}
	return res;
}


/**
 * Merge a set of PLANES bitmaps.
 */
struct buffer *bitmap_combine_planes (struct buffer *buf[], unsigned int planes)
{
	return NULL;
}


/**
 * A legacy function that decodes the older FIF format into
 * an uncompressed, joined bitmap.  This allows the imglib
 * functions to be used on existing images.
 */
struct buffer *fif_decode (struct buffer *buf, unsigned int plane)
{
	struct buffer *res = buffer_alloc (FRAME_BYTE_SIZE);
	U8 *in = buf->data;
	U8 *out = res->data;
	U8 planes, method;
	U8 val, loop;
	unsigned int i;
	unsigned int bytes;

	planes = *in++;
	for (i = 0; i < planes; i++)
	{
		method = *in++;
		bytes = 0;
		switch (method)
		{
			case 1: /* RLE */
				while (bytes < FRAME_BYTE_SIZE)
				{
					val = *in++;
					if (val == 0xEE) /* skip */
					{
						loop = *in++;
						while (loop > 0)
						{
							if (i == plane)
								*out++ = 0;
							bytes++;
							loop--;
						}
					}
					else if (val == 0xED) /* repeat */
					{
						val = *in++;
						loop = *in++;
						while (loop > 0)
						{
							if (i == plane)
								*out++ = val;
							bytes++;
							loop--;
						}
					}
					else
					{
						if (i == plane)
							*out++ = val;
						bytes++;
					}
				}
				break;
			default:
				fprintf (stderr, "error: invalid FIF method\n");
				exit (1);
		}
	}

	res->len = out - res->data;
	return res;
}

/**
 * Read a binary FIF file into bitmap format.
 * Returns the frame object.
 */
struct buffer *binary_fif_read (const char *filename)
{
	FILE *fp;
	struct buffer *buf;

	/*
	 * Allocate a raw buffer large enough to hold the
	 * entire FIF data.  Fill it with the file contents.
	 */
	buf = buffer_alloc (540);
	fp = fopen (filename, "rb");
	if (!fp)
		goto error;

	buffer_read (buf, fp);
	fclose (fp);
#if 0
	printf ("FIF:\n");
	buffer_write_c (buf, stdout);
#endif

	/*
	 * Decode the compressed FIF format into an
	 * uncompressed, joined image.
	 */
	buf = buffer_replace (buf, fif_decode (buf, 1));
#if 0
	printf ("Decoded:\n");
	buffer_write_c (buf, stdout);
#endif

	/*
	 * Finally split the buffer, producing a bitmap.
	 */
	buf = buffer_replace (buf, buffer_splitbits (buf));
	buf->width = FRAME_WIDTH;
	buf->height = FRAME_HEIGHT;

	return buf;

error:
	buffer_free (buf);
	return NULL;
}


void bitmap_finish (struct buffer *buf)
{
	buf = buffer_replace (buf, buffer_joinbits (buf));
	histogram_update (buf);
	buffer_write_c (buf, stdout);
	buffer_free (buf);
}


void bitmap_zoom_out (struct buffer *buf)
{
	for (;;)
	{
		buf = buffer_replace (buf, bitmap_translate (buf, zoom_out_translation));
		printf ("\n");
		bitmap_write_ascii (buf, stdout);
		getchar ();
	}
}


void layer_update (struct layer *layer)
{
	bitmap_fill (layer->buf, 0);
}


