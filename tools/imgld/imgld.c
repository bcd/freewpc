/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
#include <string.h>
#include <stdlib.h>
#include <imglib.h>
#include <pgmlib.h>

#define MAX_INFILES 128
#define MAX_FRAMES 256

#define error(format, rest...) \
do { \
	fprintf (stderr, "imgld: "); \
	fprintf (stderr, format, ## rest); \
	fprintf (stderr, "\n"); \
	exit (1); \
} while (0)

#define output_write(ofp, b) fprintf (ofp, "0x%02X, ", b)


#define OPT_NEGATE 0x1
#define OPT_DITHER 0x2
#define OPT_DELTA 0x4


enum image_format { 
	FORMAT_BAD, FORMAT_XBM, FORMAT_PGM, FORMAT_FIF
}; 

unsigned int n_infiles = 0;

const char *infile[MAX_INFILES];

unsigned long infile_options[MAX_INFILES];

struct buffer *frame_table[MAX_FRAMES];

unsigned int frame_count = 0;

FILE *lblfile;

FILE *outfile;

unsigned int base_page = 0x20;


/** Return the format of a file, based on its extension. */
enum image_format get_file_format (const char *filename)
{
	char *sep = strrchr (filename, '.');
	if (!sep)
		return FORMAT_BAD;

	sep++;
	if (!strcmp (sep, "xbm"))
		return FORMAT_XBM;
	else if (!strcmp (sep, "pgm"))
		return FORMAT_PGM;
	else if (!strcmp (sep, "fif"))
		return FORMAT_FIF;
	else
		return FORMAT_BAD;
}


#if 0
/** Write a FIF formatted file */
void write_fif (void)
{
	FILE *ofp;
	int n;
	int plane;
	PGM *pgm;
	XBM *xbm;
	XBMSET *xbmset;
	XBMPROG *xbmprog;
	enum image_format format;
	int n_planes = 2;

	for (n=0 ; n < n_infiles; n++)
	{
		format = get_file_format (infile[n]);

		/* Output the format (XBM or PGM).  Equivalently, this
		is the number of bitplanes in the image. */
		fprintf (ofp, "   ");
		output_write (ofp, format);
		fprintf (ofp, "/* format */\n");

		switch (format)
		{
			case FORMAT_XBM:
				n_planes = 1;
				/* FALLTHROUGH */

			case FORMAT_PGM:	
				/* Convert XBM/PGM to FIF. */
				pgm = pgm_read (infile[n]);
				if (!pgm)
					error ("cannot open %s for reading\n", infile[n]);

				/* Apply any options to the input file before proceeding. */
				if (infile_options[n] & OPT_DITHER)
					pgm_dither (pgm, (1 << n_planes) - 1);
				else
					pgm_change_maxval (pgm, (1 << n_planes) - 1);

				if (infile_options[n] & OPT_NEGATE)
					pgm_invert (pgm);

				/* Convert into the internal xbmset format.
				This divides the PGM into 2 XBMs. */
				xbmset = pgm_make_xbmset (pgm);
	
				/* Now convert each XBM plane into FIF format. */
				for (plane = 0; plane < n_planes; plane++)
				{
					fprintf (ofp, "   ");
					xbm = xbmset_plane (xbmset, plane);

					/* Use RLE encoding to save space, or RLE_DELTA
					if requested */
					xbmprog = xbm_make_prog (xbm);

					if (infile_options[n] & OPT_DELTA)
						fprintf (ofp, "XBMPROG_METHOD_RLE_DELTA, ");
					else
						fprintf (ofp, "XBMPROG_METHOD_RLE, ");

					xbmprog_write (ofp, NULL, 0, xbmprog);
					xbmprog_free (xbmprog);

					fprintf (ofp, "\n");
				}
				xbmset_free (xbmset);
				pgm_free (pgm);
				break;

			default:
				error ("invalid input file format");
		}
	}
}
#endif


void emit_label (const char *label, unsigned int no)
{
	/* Need to remove the trailing colon from the label name */
	fprintf (lblfile, "#define ");

	while (*label != ':')
		fputc (*label++, lblfile);

	fprintf (lblfile, " %d\n", no);
}


void add_frame (const char *label, struct buffer *buf)
{
	frame_table[frame_count] = buf;
	if (label)
		emit_label (label, frame_count);
	frame_count++;
}

struct buffer *pgm_get_plane (struct buffer *buf, unsigned int plane)
{
	unsigned int level, off;
	struct buffer *planebuf;

	planebuf = buffer_clone (buf);
	for (off = 0; off < planebuf->len; off++)
	{
		if (buf->data[off] <= 25 * 0xFF / 100)
			level = 0;
		else if (buf->data[off] <= 50 * 0xFF / 100)
			level = 1;
		else if (buf->data[off] <= 75 * 0xFF / 100)
			level = 2;
		else
			level = 3;
		planebuf->data[off] = (level & (1 << plane)) ? 1 : 0;
	}

	planebuf = buffer_replace (planebuf, buffer_joinbits (planebuf));
	return planebuf;	
}


void add_image (const char *label, const char *filename, unsigned int options)
{
	FILE *imgfile;
	struct buffer *buf;
	int plane;

	/* TODO - assume PGM format for now */
	enum image_format format = get_file_format (filename);

	/* Read the PGM into a bitmap */
	imgfile = fopen (filename, "r");
	if (!imgfile)
		error ("can't open image file '%s'\n", filename);

	buf = buffer_alloc (128 * 32);
	buffer_read_pgm (buf, imgfile);
	fclose (imgfile);

	/* Apply any transformations */
	if (options & OPT_NEGATE)
	{
		int off;
		for (off = 0; off < buf->len; off++)
			buf->data[off] = 0xFF - buf->data[off];
	}

	/* Convert into two bitplanes and process each */
	for (plane = 0; plane < 2; plane++)
	{
		struct buffer *planebuf = pgm_get_plane (buf, plane);
		planebuf->type = (!plane ? 0x1 : 0x0);
		add_frame (!plane ? label : NULL, planebuf);
	}

	buffer_free (buf);
}


void write_output (const char *filename)
{
	unsigned int frame;
	unsigned long offset;
	struct buffer *buf;
	unsigned char target_pointer[3];
	unsigned char padding = 0xFF;

	outfile = fopen (filename, "w");
	if (!outfile)
		error ("can't open output file '%s'\n", filename);

	/* Write the frame table header. */
	for (frame = 0, offset = (frame_count + 1) * 3, buf = frame_table[0];
		frame < frame_count; 
		frame++, offset += buf->len + 1, buf = frame_table[frame])
	{
		/* Round up to the next page boundary if the image doesn't
		 * completely fit in the current page. */
		if ((offset / 0x4000) != ((offset + buf->len) / 0x4000))
			offset = ((offset / 0x4000) + 1) * 0x4000;

		/* Convert the absolute offset from the beginning of image data into
		 * a target pointer. */
		unsigned int target_offset = (offset % 0x4000) + 0x4000;
		unsigned int target_page = (offset / 0x4000) + base_page;

		target_pointer[0] = target_offset >> 8;
		target_pointer[1] = target_offset & 0xFF;
		target_pointer[2] = target_page;
		fwrite (target_pointer, sizeof (target_pointer), 1, outfile);

		if (frame == 0)
		{
			fprintf (lblfile, "\n#define IMAGEMAP_BASE 0x4000\n", target_offset);
			fprintf (lblfile, "#define IMAGEMAP_PAGE 0x%02X\n", target_page);
		}
	}

	/* Write a NULL pointer at the end of the table */
	target_pointer[0] = 0;
	target_pointer[1] = 0;
	target_pointer[2] = 0;
	fwrite (target_pointer, sizeof (target_pointer), 1, outfile);


	/* Write the frame table data */
	for (frame = 0, offset = frame_count * 3, buf = frame_table[0];
		frame < frame_count; 
		frame++, offset += buf->len + 1, buf = frame_table[frame])
	{
		if ((offset / 0x4000) != ((offset + buf->len) / 0x4000))
		{
			unsigned long new_offset = ((offset / 0x4000) + 1) * 0x4000;
			unsigned int diff = new_offset - offset;
			while (diff-- > 0)
				fwrite (&padding, sizeof (padding), 1, outfile);
			offset = new_offset;
		}

		fputc (buf->type, outfile);
		buffer_write (buf, outfile);
	}
	fclose (outfile);
}

void parse_config (const char *filename)
{
	FILE *cfgfile = fopen (filename, "r");
	if (!cfgfile)
		error ("can't open config file '%s'\n", filename);

	for (;;)
	{
		char line[256];
		char *word;
		char *label;
		char *filename;
		unsigned int options = 0;

		fgets (line, 255, cfgfile);
		if (feof (cfgfile))
			break;

		label = filename = NULL;
		word = strtok (line, " \t\n");
		while (word != NULL)
		{
			if (*word == '#')
				break;
			else if (strchr (word, ':'))
				label = word;
			else if (*word == '!')
			{
				filename = word+1;
				options |= OPT_NEGATE;
			}
			else if (*word)
				filename = word;
			word = strtok (NULL, " \t\n");
		}

		if (filename)
			add_image (label, filename, options);
	}
	fclose (cfgfile);
}


int main (int argc, char *argv[])
{
	int argn;
	const char *arg;
	const char *outfilename;

	for (argn = 1; argn < argc; argn++)
	{
		arg = argv[argn];
		if (*arg == '-')
		{
			switch (arg[1])
			{
				case 'i':
					lblfile = fopen (argv[++argn], "w");
					fprintf (lblfile, "/* Automatically generated by imgld */\n\n");
					fprintf (lblfile, "#ifndef _IMGLD_IMAGEMAP_H\n");
					fprintf (lblfile, "#define _IMGLD_IMAGEMAP_H\n\n");
					break;

				case 'o':
					outfilename = argv[++argn];
					break;
			}
		}
		else
		{
			parse_config (arg);
		}
	}

	write_output (outfilename);

	fprintf (lblfile, "\n#define MAX_IMAGE_NUMBER %d\n", frame_count);
	fprintf (lblfile, "\n#endif /* _IMGLD_IMAGEMAP_H */\n");
	fclose (lblfile);

	exit (0);
}

