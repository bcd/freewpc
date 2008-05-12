
#include "imglib.h"

struct buffer *buffer_alloc (unsigned int maxlen)
{
	struct buffer *buf = malloc (sizeof (struct buffer));
	buf->len = maxlen;
	buf->data = buf->_data;
	buf->hist = NULL;
	buf->width = buf->height = 0;
	buf->color = 1;
	memset (buf->_data, 0, maxlen);
	return buf;
}


struct buffer *bitmap_alloc (unsigned int width, unsigned int height)
{
	struct buffer *buf = buffer_alloc (width * height);
	buf->width = width;
	buf->height = height;
	return buf;
}


struct buffer *frame_alloc (void)
{
	return bitmap_alloc (FRAME_WIDTH, FRAME_HEIGHT);
}


unsigned int bitmap_pos (struct buffer *buf, unsigned int x, unsigned int y)
{
	return y * buf->width + x;
}


void buffer_read (struct buffer *buf, FILE *fp)
{
	buf->len = fread (buf->data, sizeof (U8), MAX_BUFFER_SIZE, fp);
}

void buffer_write (struct buffer *buf, FILE *fp)
{
	fwrite (buf->data, sizeof (U8), buf->len, fp);
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
			if (buf->data[bitmap_pos (buf, x, y)] != 0)
				fputc ('X', fp);
			else
				fputc ('.', fp);
		fputc ('\n', fp);
	}
}

void buffer_write_c (struct buffer *buf, FILE *fp)
{
	unsigned int off;

	if (buf->hist)
	{
		fprintf (fp, "/* Histogram (%d unique values):\n", buf->hist->unique);
		for (off = 0; off < buf->hist->unique; off++)
		{
			U8 val = buf->hist->most_frequent[off];
			unsigned int count = buf->hist->count[val];
			fprintf (fp, " * %d occurrences of %02X\n",
				count, val);
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
	free (buf);
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
	return ~a;
}

struct buffer *buffer_binop (struct buffer *a, struct buffer *b,
	binary_operator op)
{
	unsigned int off = 0;
	struct buffer *res = buffer_alloc (max (a->len, b->len));

	while (off < a->len && off < b->len)
	{
		res->data[off] = op (a->data[off], b->data[off]);
		off++;
	}
	return res;
}


struct buffer *buffer_unop (struct buffer *buf, unary_operator op)
{
	unsigned int off = 0;
	struct buffer *res = buffer_alloc (buf->len);

	while (off < buf->len)
	{
		res->data[off] = op (buf->data[off]);
		off++;
	}
	return res;
}


struct buffer *buffer_joinbits (struct buffer *buf)
{
	unsigned int off, bit;
	struct buffer *res = buffer_alloc (buf->len / 8);

	for (off = 0; off < buf->len; off++)
	{
		U8 val = 0;
		for (bit = 0; bit < 8; bit++)
			if (buf->data[off * 8 + bit] == 1)
				val |= 1 << bit;
		res->data[off] = val;
	}
	return res;
}

struct buffer *buffer_splitbits (struct buffer *buf)
{
	unsigned int off, bit;
	struct buffer *res = buffer_alloc (buf->len * 8);

	for (off = 0; off < buf->len; off++)
	{
		for (bit = 0; bit < 8; bit++)
		{
			if (buf->data[off] & (1 << bit))
				res->data[off * 8 + bit] = 1;
			else
				res->data[off * 8 + bit] = 0;
		}
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
			return 1;
	return 0;
}


struct buffer *buffer_replace (struct buffer *old, struct buffer *new)
{
	free (old);
	return new;
}


struct histogram *histogram_update (struct buffer *buf)
{
	struct histogram *hist;
	unsigned int off;
	unsigned int i;
	int largest;
	unsigned int already_taken[256] = { 0, };

	if (!buf->hist)
		hist = buf->hist = malloc (sizeof (struct histogram));

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


struct buffer *buffer_rle_encode (struct buffer *buf)
{
	struct buffer *res = buffer_alloc (buf->len);
	return res;
}


struct buffer *buffer_rle_decode (struct buffer *buf)
{
	struct buffer *res = buffer_alloc (MAX_BUFFER_SIZE);
	return res;
}


struct buffer *bitmap_crop (struct buffer *buf)
{
	struct buffer *res = buffer_alloc (buf->len);
	return res;
}


void bitmap_draw_pixel (struct buffer *buf, unsigned int x, unsigned int y)
{
	buf->data[bitmap_pos (buf, x, y)] = buf->color;
}


void bitmap_draw_line (struct buffer *buf,
	int x1, int y1,
	int x2, int y2)
{
	float xm, ym;

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


void bitmap_draw_box (struct buffer *buf,
	int x1, int y1,
	int x2, int y2)
{
	bitmap_draw_line (buf, x1, y1, x2, y1);
	bitmap_draw_line (buf, x1, y1, x1, y2);
	bitmap_draw_line (buf, x2, y1, x2, y2);
	bitmap_draw_line (buf, x1, y2, x2, y2);
}


void bitmap_draw_border (struct buffer *buf, unsigned int width)
{
	unsigned int i;
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


struct buffer *bitmap_translate (struct buffer *buf, translate_operator op)
{
	struct buffer *res = buffer_alloc (buf->len);
	unsigned int x, y;

	res->width = buf->width;
	res->height = buf->height;
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


void bitmap_fill (struct buffer *buf, U8 val)
{
	unsigned int x, y;
	buf->color = val;
	for (x = 0; x < buf->width; x++)
		for (y = 0; y < buf->height; y++)
			bitmap_draw_pixel (buf, x, y);
}


struct buffer *fif_decode (struct buffer *buf, unsigned int plane)
{
	struct buffer *res = buffer_alloc (512);
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
				while (bytes < 512)
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

struct buffer *binary_fif_read (const char *filename)
{
	FILE *fp;
	struct buffer *buf;

	buf = buffer_alloc (540);
	fp = fopen (filename, "rb");
	buffer_read (buf, fp);
	printf ("FIF:\n");
	buffer_write_c (buf, stdout);

	printf ("Decoded:\n");
	buf = buffer_replace (buf, fif_decode (buf, 1));
	buffer_write_c (buf, stdout);

	buf = buffer_replace (buf, buffer_splitbits (buf));
	buf->width = 128;
	buf->height = 32;
	fclose (fp);
	return buf;
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

