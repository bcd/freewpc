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

/* \file imgld.c
 * \description The FreeWPC image linker
 *
 * The image linker reads in one or more image linker command files (.ild)
 * which gives a list of images to be included in the ROM image, and
 * optionally, C-style labels that can be used to reference the images
 * from code.
 *
 * The objective is to load in all of the image files, optimize them, and
 * create an index table with pointers to the beginning of each image.
 * The C defines are simple 16-bit incremental values which are used to
 * index the table and get a pointer to the image data.  These pointers
 * are all 3-bytes long, stored as pointer/ROM page pairs.
 *
 * Optimization is not yet implemented completely, but here's what should
 * happen:  The basic input image format is PGM, which is a simple
 * bitmap format with 256 colors per pixel.  Frames are expected to be
 * in 128x32 size already.  When not optimizing, at a minimum, the
 * image needs to be converted into two, 128x32 joined buffers.  This
 * is the format that can be sent directly to the dot matrix board.
 * Two buffers are needed for 4-color output; the original 256 colors
 * will be downscaled to 4 colors as necessary.
 *
 * The pinball game ROM may implement a number of decoders; at present,
 * only one decoder is supported, which does simple run-length encoding of
 * 16-bit words.  It is written for very easy decoding with little
 * runtime overhead, and so does not compress very well.  However,
 * CPU cycles are considered more precious than ROM space.  Different
 * codecs may be added in the future with different properties; e.g.
 * a long-running animation when the CPU is mostly idle may need to tuned
 * for better compression at the cost of a longer decompression time.
 *
 * Command-line parameters specify the total amount of space that is
 * allocated for images.  The linker will only decompress as is
 * necessary; as long as there is ample space, it does not make sense
 * to compress.  If multiple codecs are available, then all should be
 * attempted and the one that matches the performance requirements the
 * best should be selected.  Images can also be tagged to denote those
 * where runtime performance is especially critical -- those would be
 * compressed *last*.  The linker would continue to compress until
 * everything fits.  Then it builds the table and outputs the final image.
 *
 * The output is placed in a file dedicated for images.  A separate part
 * of the build process copies that file into the actual game ROM.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <imglib.h>
#include <pgmlib.h>

#define MAX_FRAMES 256

#define error(format, rest...) \
do { \
	fprintf (stderr, "imgld: "); \
	fprintf (stderr, format, ## rest); \
	fprintf (stderr, "\n"); \
	exit (1); \
} while (0)


#define OPT_NEGATE 0x1


enum image_format {
	FORMAT_BAD, FORMAT_XBM, FORMAT_PGM, FORMAT_FIF
};



/**
 * Stores everything you need to know about an image.
 */
struct image
{
	/* The raw, uncompressed format of the image.
	 * This is a type 0, joined buffer that is suitable for
	 * direct dumping to the display. */
	struct buffer *raw;

	struct buffer *compressed;

	/* The preferred representation */
	struct buffer *buf;
};


struct buffer *frame_table[MAX_FRAMES];
struct buffer *rle_frame_table[MAX_FRAMES];
struct buffer *sparse_frame_table[MAX_FRAMES];

unsigned int frame_count = 0;

FILE *lblfile;

FILE *outfile;

unsigned int base_page = 0x20;

unsigned int max_rom_size = 65536;

/**
 * Target properties are used to describe things specific
 * to the CPU or platform.  The target image can be divided
 * into _pages_ of a particular length, and linear addresses
 * are translated into a <logical_addr,page number> pair.
 * If a target doesn't require paging, then use a really
 * large page size.  The page number must range from 0-255.
 */

struct target_properties
{
	unsigned int page_base;
	unsigned int page_size;
};

struct target_properties target_m6809 =
{
	.page_base = 0x4000,
	.page_size = 0x4000,
};

struct target_properties *target_props = &target_m6809;


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
	rle_frame_table[frame_count] = buffer_rle_encode (buf);
	sparse_frame_table[frame_count] = buffer_sparse_encode (buf);

//#define COMPRESS_IT
#ifdef COMPRESS_IT
	frame_table[frame_count] = sparse_frame_table[frame_count];
#else
	frame_table[frame_count] = buf;
#endif

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

#if 0
	/* TODO - assume PGM format for now */
	enum image_format format = get_file_format (filename);
#endif

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


/**
 * Returns offset if an object is of SIZE can be placed there without
 * crossing a page boundary.  Otherwise, returns the address of to the
 * next page.
 */
unsigned long round_up_to_page (unsigned long offset, unsigned int size)
{
	if ((offset / target_props->page_size) == ((offset + size) / target_props->page_size))
		return offset;
	else
		return ((offset / target_props->page_size) + 1) * target_props->page_size;
}


void convert_to_target_pointer (unsigned long offset, unsigned char pointer[])
{
	/* Compute the offset and page components */
	unsigned int target_offset = (offset % target_props->page_size) + target_props->page_base;
	unsigned int target_page = (offset / target_props->page_size) + base_page;

	/* Store these into the target pointer */
	pointer[0] = target_offset >> 8;
	pointer[1] = target_offset & 0xFF;
	pointer[2] = target_page;
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
		offset = round_up_to_page (offset, buf->len+1);

		/* Convert the absolute offset from the beginning of image data into
		 * a target pointer, and write it to the table header. */
		convert_to_target_pointer (offset, target_pointer);
		fwrite (target_pointer, sizeof (target_pointer), 1, outfile);

		if (frame == 0)
		{
			fprintf (lblfile, "\n#define IMAGEMAP_BASE 0x%04X\n", target_props->page_base);
			fprintf (lblfile, "#define IMAGEMAP_PAGE 0x%02X\n", base_page);
		}

		fprintf (lblfile, "/* %d: %02X/%02X%02X, type %02X, len %d */\n",
			frame, target_pointer[2], target_pointer[0], target_pointer[1],
			buf->type, buf->len);
		fprintf (lblfile, "   /* RLE encoded version has len %d */\n", rle_frame_table[frame]->len);
		fprintf (lblfile, "   /* Sparse encoded version has len %d */\n", sparse_frame_table[frame]->len);
	}

	/* Write a NULL pointer at the end of the table.  Not strictly needed anymore,
	since the number of table entries is available as a #define. */
	convert_to_target_pointer (0, target_pointer);
	fwrite (target_pointer, sizeof (target_pointer), 1, outfile);


	/* Write the frame table data */
	for (frame = 0, offset = frame_count * sizeof (target_pointer), buf = frame_table[0];
		frame < frame_count;
		frame++, offset += buf->len + 1, buf = frame_table[frame])
	{
		/* If the object address needed to be pushed to the next
		page boundary, then output padding bytes first. */
		padding = round_up_to_page (offset, buf->len+1) - offset;
		while (padding-- > 0)
		{
			fwrite (&padding, sizeof (padding), 1, outfile);
			offset++;
		}

		/* Output the image data itself */
		fputc (buf->type, outfile);
		buffer_write (buf, outfile);
	}
	fclose (outfile);
}


/**
 * Parse the configuration file.
 */
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
				case 'h':
					printf ("Options:\n");
					printf ("-i <include-file>            Writes #defines to this include file\n");
					printf ("-o <output-file>             Writes final image data to this file\n");
					printf ("-p <page>                    Set the base page number\n");
					printf ("-s <size>                    Set the maximum amount of space available\n");
					exit (0);

				case 'i':
					lblfile = fopen (argv[++argn], "w");
					fprintf (lblfile, "/* Automatically generated by imgld */\n\n");
					fprintf (lblfile, "#ifndef _IMGLD_IMAGEMAP_H\n");
					fprintf (lblfile, "#define _IMGLD_IMAGEMAP_H\n\n");
					break;

				case 'o':
					outfilename = argv[++argn];
					break;

				case 'p':
					base_page = strtoul (argv[++argn], NULL, 0);
					break;

				case 's':
					max_rom_size = strtoul (argv[++argn], NULL, 0);
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

