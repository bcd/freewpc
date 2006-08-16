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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pgmlib.h"

unsigned int opt_debug = 0;

unsigned int opt_verbose = 1;

unsigned int opt_compress = 0;

const char *opt_makefile_fragment_name = "build/Makefile.xbms";


#define dprintf(format, rest...)	\
	if (opt_debug) fprintf (stderr, format, ## rest)


inline void *xmalloc (size_t size)
{
	return malloc (size);
}


XBMPROG *
xbmprog_alloc (void)
{
	XBMPROG *xbmprog = xmalloc (sizeof (XBMPROG));

	return (xbmprog);
}


/* Returns zero if two XBM program elements are the same */
int
xbmprog_equal_p (XBMPROG *prog1, XBMPROG *prog2)
{
	if (prog1->op != prog2->op)
		return 1;

	switch (prog1->op)
	{
		case XBMOP_LITERAL:
			return prog1->args.literal.count == prog2->args.literal.count
				&& !memcmp (prog1->args.literal.bytes,
						prog2->args.literal.bytes,
						prog1->args.literal.count);
		case XBMOP_REPEAT_BYTE:
			return prog1->args.repeat.count == prog2->args.repeat.count
				&& prog1->args.repeat.data && prog2->args.repeat.data;
		case XBMOP_REPEAT_WORD:
		case XBMOP_SKIP:
			return prog1->args.skip.count == prog2->args.skip.count;
	}

	return 0;
}


void
xbmprog_write (FILE *fp, XBMPROG *xbmprog)
{
	int i;
	unsigned int size = 0;
	XBMPROG *head = xbmprog;

	fprintf (fp, "/* -- compressed representation\n");
	do {
		switch (xbmprog->op)
		{
			case XBMOP_LITERAL:
				for (i=0; i < xbmprog->args.literal.count; i++)
					fprintf (fp, "%02X, ", xbmprog->args.literal.bytes[i]);
				size += xbmprog->args.literal.count + 1;
				break;
			case XBMOP_SKIP:
				fprintf (fp, "skip %d, ", xbmprog->args.skip.count);
				size++;
				break;
			case XBMOP_REPEAT_BYTE:
				fprintf (fp, "repeat %02X x %d, ", 
					xbmprog->args.repeat.data, xbmprog->args.repeat.count);
				size += 2;
				break;
			default:
				fprintf (fp, "!!! bad opcode %02X\n", xbmprog->op);
				goto done;
		}
		xbmprog = xbmprog->next;
	} while (xbmprog != head);

done:
	fprintf (fp, "\ncompressed size = %d\n", size);
	fprintf (fp, "*/\n");
}


static int
xbm_read_byte (XBM *xbm, int offset)
{
	int y = offset / ((xbm->width + 7) / 8);
	int x = offset % ((xbm->width + 7) / 8);
	return xbm->bytes[y][x];
}


XBMPROG *
xbm_make_prog (XBM *xbm)
{
	XBMPROG *head = NULL, *tail = NULL;
	unsigned int off = 0;
	unsigned int n_bytes;
	unsigned int byte;
	unsigned int count;
	XBMPROG *elem;

	n_bytes = ((xbm->width + 7) / 8) * xbm->height;
	dprintf ("Image has %d bytes total.\n", n_bytes);
	while (off < n_bytes)
	{
		elem = NULL;

		/* At the current offset, which is the best way of compressing
		the next sequence of data? */

		/* Scan for a repeating sequence. */
		byte = xbm_read_byte (xbm, off);
		count = 1;
		while ((xbm_read_byte (xbm, off+count) == byte)
			&& (off+count < n_bytes))
			count++;

		if (count <= 2)
		{
			/* If the run length is 1 or 2, then there's no value
			in compressing this, so output a literal.
			If the last output was a literal, then try to append
			to that, otherwise allocate a new literal. */
			if (tail 
				&& tail->op == XBMOP_LITERAL
				&& tail->args.literal.count < 16)
			{
				tail->args.literal.bytes [ tail->args.literal.count++ ] = byte;
			}
			else
			{
				elem = xbmprog_alloc ();
				elem->op = XBMOP_LITERAL;
				elem->args.literal.count = 0;
				elem->args.literal.bytes [ elem->args.literal.count++ ] = byte;
			}
			off++;
		}
		else if (byte == 0)
		{
			/* Repeated zeroes are especially optimal.
			Output a skip. */
			elem = xbmprog_alloc ();
			elem->op = XBMOP_SKIP;
			elem->args.skip.count = count;
			off += count;
		}
		else
		{
			/* Output a repeat. */
			elem = xbmprog_alloc ();
			elem->op = XBMOP_REPEAT_BYTE;
			elem->args.repeat.count = count;
			elem->args.repeat.data = byte;
			off += count;
		}

		if (elem)
		{
			/* If we created a new element, add it to the chain. */
			if (head == NULL)
			{
				elem->prev = elem->next = elem;
				head = tail = elem;
			}
			else
			{
				tail->next = head->prev = elem;
				elem->prev = tail;
				elem->next = head;
				tail = elem;
			}
		}
	}

	return (head);
}


XBMSET *
pgm_make_xbmset (PGM *pgm)
{
	unsigned int plane, x, y;
	XBMSET *xbmset = xmalloc (sizeof (XBMSET));
	XBM *xbm;

	xbmset->c_name = "image";
	switch (pgm->maxval)
	{
		case 1:
			xbmset->n_planes = 1;
			break;
		case 3:
			xbmset->n_planes = 2;
			break;
		default:
			fprintf (stderr, "Invalid maxval %d for XBM conversion", pgm->maxval);
			exit (1);
	}

	for (plane = 0; plane < xbmset->n_planes; plane++)
	{
		xbm = xbmset->planes[plane] = xmalloc (sizeof (XBM));
		xbm->width = pgm->width;
		xbm->height = pgm->height;
		memset (xbm->bytes, 0, sizeof (xbm->bytes));

		for (x = 0; x < pgm->width; x++)
		{
			for (y = 0; y < pgm->height; y++)
			{
				xbm->bytes[y][x / 8] |= 
					((pgm->bits[y][x] & (1 << plane)) << (x % 8)) >> plane;
			}
		}
	}

	return (xbmset);
}


int
xbm_unique_byte_count (XBM * xbm)
{
	unsigned int x, y;
	unsigned int bytemap[256] = { 0, };
	unsigned int unique_count = 0;

	for (y = 0; y < xbm->height; y++)
		for (x = 0; x < (xbm->width + 7) / 8; x++)
		{
			if (bytemap[xbm->bytes[y][x]] == 0)
				unique_count++;
			bytemap[xbm->bytes[y][x]]++;
		}

	return unique_count;
}


void
xbm_write_stats (FILE *fp, XBM *xbm)
{
	fprintf (fp, "unique_byte_count = %d\n", xbm_unique_byte_count (xbm));
}


void
xbmset_write (FILE *fp, XBMSET *xbmset, int plane, int write_flags)
{
	unsigned int x, y;
	XBM *xbm;
	
	unsigned int bytes = 0;

	if (plane == XBMSET_ALL_PLANES)
	{
		for (plane = xbmset->n_planes-1; plane >=0; plane--)
			xbmset_write (fp, xbmset, plane, write_flags);
		return;
	}

	xbm = xbmset->planes[plane];

	fprintf (fp, "#define %s%d_width %d\n", xbmset->c_name, plane, xbm->width);
	fprintf (fp, "#define %s%d_height %d\n", xbmset->c_name, plane, xbm->height);
	fprintf (fp, "static unsigned char %s%d_bits[] = {\n", xbmset->c_name, plane);

	if (write_flags & XBM_WRITE_HEADER)
		fprintf (fp, "0x%02x, 0x%02x,\n", xbm->width, xbm->height);

	for (y = 0; y < xbm->height; y++)
		for (x = 0; x < (xbm->width + 7) / 8; x++, bytes++)
		{
			fprintf (fp, "0x%02x, ", xbm->bytes[y][x]);
			if ((bytes % 8) == 7)
				fprintf (fp, "\n");
		}

	fprintf (fp, "};\n");

	if (opt_compress)
	{
		XBMPROG *xbmprog = xbm_make_prog (xbm);
		xbmprog_write (fp, xbmprog);
	}
}


void
xbmset_free (XBMSET *xbmset)
{
	free (xbmset);
}


PGM *
pgm_alloc (void)
{
	PGM *pgm = xmalloc (sizeof (PGM));
	pgm->width = MAX_WIDTH;
	pgm->height = MAX_HEIGHT;
	pgm->maxval = MAX_MAXVAL;
	memset (pgm->bits, 0, sizeof (pgm->bits));
	return (pgm);
}


void
pgm_free (PGM *pgm)
{
	free (pgm);
}


void
pgm_resize (PGM *pgm,
	unsigned int width, unsigned int height)
{
	pgm->width = width;
	pgm->height = height;
}


void
pgm_set_maxval (PGM *pgm,
	unsigned int depth)
{
	pgm->maxval = depth;
}


PGM *
pgm_read (const char *filename)
{
	PGM *pgm;
	FILE *fp;
	char line[80];
	unsigned int x, y;

	pgm = pgm_alloc ();
	fp = fopen (filename, "r");

	fgets (line, 79, fp); /* magic */
	fgets (line, 79, fp); /* comment */

	fgets (line, 79, fp);
	sscanf (line, "%d %d", &pgm->width, &pgm->height);

	fgets (line, 79, fp);
	sscanf (line, "%d", &pgm->maxval);

	for (y=0; y < pgm->height; y++)
		for (x=0; x < pgm->width; x++)
		{
			fgets (line, 79, fp);
			sscanf (line, "%d\n", &pgm->bits[y][x]);
		}

	fclose (fp);
	return (pgm);
}


void
pgm_write (FILE *fp, PGM *pgm)
{
	unsigned int x, y;

	fprintf (fp, "P2\n");
	fprintf (fp, "#\n");
	fprintf (fp, "%d %d\n", pgm->width, pgm->height);
	fprintf (fp, "%d\n", pgm->maxval);
	for (y = 0; y < pgm->height; y++)
		for (x = 0; x < pgm->width; x++)
			fprintf (fp, "%d\n", pgm->bits[y][x]);
}


void
pgm_draw_pixel (PGM *pgm, 
	unsigned int x, unsigned int y, unsigned int val)
{
	pgm->bits[y][x] = val;
}


unsigned int
pgm_read_pixel (PGM *pgm, 
	unsigned int x, unsigned int y)
{
	return pgm->bits[y][x];
}


void
pgm_draw_hline (PGM *pgm, 
	unsigned int x1, unsigned int x2, unsigned int y, unsigned int val)
{
	unsigned int x;
	for (x = x1; x <= x2; x++)
		pgm_draw_pixel (pgm, x, y, val);
}


void
pgm_draw_vline (PGM *pgm, 
	unsigned int x, unsigned int y1, unsigned int y2, unsigned int val)
{
	unsigned int y;
	for (y = y1; y <= y2; y++)
		pgm_draw_pixel (pgm, x, y, val);
}


void
pgm_draw_box (PGM *pgm,
	unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
	unsigned int val)
{
	pgm_draw_hline (pgm, x1, x2, y1, val);
	pgm_draw_hline (pgm, x1, x2, y2, val);
	pgm_draw_vline (pgm, x1, y1, y2, val);
	pgm_draw_vline (pgm, x2, y1, y2, val);
}


void
pgm_draw_border (PGM *pgm,
	unsigned int width, unsigned int val)
{
	unsigned int off;
	for (off = 0; off < width; off++)
		pgm_draw_box (pgm, off, off, pgm->width - off - 1, pgm->height - off - 1, val);
}


void
pgm_fill_box (PGM *pgm,
	unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
	unsigned int val)
{
	unsigned int x, y;
	for (x = x1; x <= x2; x++)
		for (y = y1; y <= y2; y++)
			pgm_draw_pixel (pgm, x, y, val);
}


void
pgm_fill (PGM *pgm, unsigned int val)
{
	pgm_fill_box (pgm, 0, 0, pgm->width - 1, pgm->height - 1, val);
}


void
pgm_translate (PGM *dst, PGM *src,
	int xshift, int yshift)
{
}


void
pgm_paste (PGM *dst, PGM *src, unsigned int xpos, unsigned int ypos)
{
	unsigned int x, y;
	for (x = 0; x < src->width; x++)
		for (y = 0; y < src->height; y++)
			pgm_draw_pixel (dst, xpos+x, ypos+y, pgm_read_pixel (src, x, y));
}


void
pgm_scale (PGM *pgm,
	double factor)
{
}


void
pgm_xor (PGM *dst, PGM *src1, PGM *src2)
{
	unsigned int x, y;
	/* verify dst, src1 and src2 are the same size */
	for (x = 0; x < src1->width; x++)
		for (y = 0; y < src1->height; y++)
			pgm_draw_pixel (dst, x, y, 
				pgm_read_pixel (src1, x, y) ^ pgm_read_pixel (src2, x, y));
}


void
pgm_change_maxval (PGM *pgm, unsigned int new_maxval)
{
	unsigned int x, y;
	double factor;

	if (new_maxval == pgm->maxval)
		return;

	factor = (pgm->maxval + 1) / (new_maxval+1);

	for (y = 0; y < pgm->height; y++)
		for (x = 0; x < pgm->width; x++)
			pgm->bits[y][x] = ((pgm->bits[y][x] + 1) / factor) - 1;
	pgm->maxval = new_maxval;
}


void
pgm_invert (PGM *pgm)
{
	unsigned int x, y;
	for (y = 0; y < pgm->height; y++)
		for (x = 0; x < pgm->width; x++)
			pgm->bits[y][x] = pgm->maxval - pgm->bits[y][x];
}


void
pgm_output_file (const char *filename)
{
	FILE *fp;
	static int first_output = 0;
	char objname[64], *p;

	fp = fopen (opt_makefile_fragment_name, first_output ? "a" : "w");
	first_output = 1;

	strcpy (objname, filename);
	p = strchr (objname, '.');
	if (p)
		sprintf (p+1, "o");
	else
		strcat (objname, ".o");

	fprintf (fp, "%s : %s\n", objname, filename);
	fprintf (fp, "XBMGEN_OBJS += %s\n", objname);
	fclose (fp);
}


void
pgm_finish_output_file (void)
{
	FILE *fp = fopen (opt_makefile_fragment_name, "a");
	fprintf (fp, "xbmgen_objs : $(XBMGEN_OBJS)\n");
	fclose (fp);
}


void
pgm_write_xbmset (PGM *pgm, const char *filename, const char *name)
{
	XBMSET *xbmset;
	FILE *fp;

	xbmset = pgm_make_xbmset (pgm);
	xbmset_name (xbmset, name);
	fp = fopen (filename, "wb");
	pgm_output_file (filename);
	xbmset_write (fp, xbmset, XBMSET_ALL_PLANES, 0);
	fclose (fp);
	xbmset_free (xbmset);
}


int
main (int argc, char *argv[])
{
	if (argv[1])
	{
		char *arg = argv[1];
		while (*arg != '\0')
		{
			switch (*arg++)
			{
				case 'd':
					opt_debug = 1;
					break;

				case 'q':
					opt_verbose = 0;
					break;

				case 'c':
					opt_compress = 1;
					break;
			}
		}
	}

	machgen ();

	pgm_finish_output_file ();
	exit (0);
}

