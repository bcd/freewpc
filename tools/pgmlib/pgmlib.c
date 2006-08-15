
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pgmlib.h"

inline void *xmalloc (size_t size)
{
	return malloc (size);
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


void
pgm_write (FILE *fp, PGM *pgm)
{
	unsigned int x, y;

	fprintf (fp, "P2\n");
	fprintf (fp, "#\n");
	fprintf (fp, "%d %d\n", pgm->width, pgm->height);
	fprintf (fp, "%d\n", pgm->maxval);
	for (y = 0; y < pgm->height; y++)
	{
		for (x = 0; x < pgm->width; x++)
		{
			fprintf (fp, "%d ", pgm->bits[y][x]);
			if ((x % 16) == 15)
				fprintf (fp, "\n");
		}
		if ((x % 16) != 0)
			fprintf (fp, "\n");
	}
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

	for (x = 0; x <= src1->width; x++)
		for (y = 0; y <= src1->height; y++)
			pgm_draw_pixel (dst, x, y, 
				pgm_read_pixel (src1, x, y) ^ pgm_read_pixel (src2, x, y));
}


void
pgm_write_xbmset (PGM *pgm, const char *filename, const char *name)
{
	XBMSET *xbmset;
	FILE *fp;

	xbmset = pgm_make_xbmset (pgm);
	xbmset_name (xbmset, name);
	fp = fopen (filename, "wb");
	xbmset_write (fp, xbmset, XBMSET_ALL_PLANES, 0);
	fclose (fp);
	xbmset_free (xbmset);
}


int
main (int argc, char *argv[])
{
	machgen ();
	exit (0);
}

