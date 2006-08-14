
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WIDTH 128
#define MAX_HEIGHT 32
#define MAX_MAXVAL 15
#define MAX_XBMSET_PLANES 4

#define PGM_COLOR(pgm, percent)	((unsigned int)(((pgm)->maxval) * (percent)))
#define PGM_BLACK(pgm)	PGM_COLOR(pgm, 0.0)
#define PGM_DARK(pgm)	PGM_COLOR(pgm, 0.34)
#define PGM_BRIGHT(pgm)	PGM_COLOR(pgm, 0.67)
#define PGM_WHITE(pgm)	PGM_COLOR(pgm, 1.0)

#define pgm_set_plane_count(pgm,n)	pgm_set_maxval (pgm, ((1 << (n)) - 1))
#define pgm_set_mono(pgm)				pgm_set_plane_count (pgm, 1)
#define pgm_set_four_color(pgm)		pgm_set_plane_count (pgm, 2)

#define XBMSET_ALL_PLANES -1

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
	unsigned int n_planes;
	XBM *planes[MAX_XBMSET_PLANES];
} XBMSET;


typedef enum {
	/* Opcode 0x00-0x1F is followed by literal data.  The amount is embedded
	in the opcode and can be from 1 to 32 bytes */
	XBMOP_LITERAL,

	/* Opcode 0x20-0x3F is followed by a single byte of literal data,
	which repeats up to 32 times in a row */
	XBMOP_REPEAT_BYTE,

	/* Opcode 0x40-0x5F is followed by a single word of literal data,
	which repeats up to 32 times in a row (for up to 64 bytes) */
	XBMOP_REPEAT_WORD,

	/* Opcode 0x60-0x7F indicates up to 32 bytes needs to be "skipped",
	with nothing done at those locations */
	XBMOP_SKIP,

	/* Opcode 0x80-0x8F precedes a sequence of up to 16 bytes, that defines
	a character map. */
	XBMOP_CHARMAP_DEFINE,

	/* Opcode 0x90-0xCF spans 64 possible values.  Each of these values
	refers to a 2-element set, with each element having eight possible
	values.  Those values refer to the character map.  This allows semi-random
	data to be compressed more efficiently. */
	XBMOP_CHARMAP_REF,
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
} XBMPROG;


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
		for (plane = 0; plane < xbmset->n_planes; plane++)
			xbmset_write (fp, xbmset, plane, write_flags);
		return;
	}

	xbm = xbmset->planes[plane];

	fprintf (fp, "#define image%d_width %d\n", plane, xbm->width);
	fprintf (fp, "#define image%d_height %d\n", plane, xbm->height);
	fprintf (fp, "static unsigned char image%d_bits[] = {\n", plane);

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
pgm_translate (PGM *dst, PGM *src,
	int xshift, int yshift)
{
}


void
pgm_paste (PGM *dst, PGM *src)
{
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


/********************************************************************/

int
main (int argc, char *argv[])
{
	PGM *pgm;
	XBMSET *xbmset;

	pgm = pgm_alloc ();
	pgm_set_plane_count (pgm, 2);
	pgm_draw_border (pgm, 3, PGM_DARK(pgm));
	pgm_draw_border (pgm, 2, PGM_BRIGHT(pgm));
	pgm_draw_border (pgm, 1, PGM_WHITE(pgm));

	xbmset = pgm_make_xbmset (pgm);
	xbmset_write (stdout, xbmset, XBMSET_ALL_PLANES, XBM_WRITE_HEADER);
	xbm_write_stats (stdout, xbmset->planes[0]);
	
	/* pgm_write (stdout, pgm); */
	pgm_free (pgm);
}

