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
 * The basic input image format is PGM, which is a simple bitmap
 * format with 256 colors per pixel.  Frames are expected to be
 * in 128x32 size already.  When not optimizing, at a minimum, the
 * image needs to be converted into two, 128x32 joined buffers.  This
 * is the format that can be sent directly to the dot matrix board.
 * Two buffers are needed for 4-color output; the original 256 colors
 * will be downscaled to 4 colors as necessary.
 *
 * The pinball game ROM may implement a number of decoders; at present,
 * there are two supported decoders.  One does simple run-length encoding
 * of 16-bit words.  It is written for very easy decoding with little
 * runtime overhead, and so does not compress very well.  However,
 * CPU cycles are considered more precious than ROM space.  The second
 * is for sparse images with lots of zeroes.  Different codecs may be
 * added in the future with different properties; e.g. for a long-
 * running animation when the CPU is mostly idle may need to tuned
 * for better compression at the cost of a longer decompression time.
 *
 * Command-line parameters specify the total amount of space that is
 * allocated for images.  The linker should only compress as is
 * necessary: as long as there is ample space, it does not make sense
 * to compress.  If multiple codecs are available, then all should be
 * attempted and the one that matches the performance requirements the
 * best should be selected.  Images can also be tagged to denote those
 * where runtime performance is especially critical -- those would be
 * compressed *last*.  The linker continues to compress until
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

#define MAX_FRAMES 2048

#define error(format, rest...) \
do { \
	fprintf (stderr, "imgld: "); \
	fprintf (stderr, format, ## rest); \
	fprintf (stderr, "\n"); \
	exit (1); \
} while (0)


#define OPT_NEGATE 0x1
#define OPT_FAST   0x2

#define TYPE_BITMAP 0x80


enum image_format {
	FORMAT_BAD, FORMAT_PGM
};


/**
 * Stores everything you need to know about an image.
 */
struct frame
{
	/* The raw, uncompressed format of the image.
	 * This is a type 0, joined buffer that is suitable for
	 * direct dumping to the display.
	 */
	struct buffer *rawbuf;

	/* The best version of the image calculated so far.
	 * Initially this is the same as rawbuf, but if an encoder
	 * is tried and it does well, it will be used instead.
	 */
	struct buffer *curbuf;

	/*
	 * The image type for curbuf.
	 */
	U8 type;

	/*
	 * The optional label name assigned to this frame.
	 */
   const char *name;

	/*
	 * The 'cost' of displaying this frame, in terms of the CPU power
	 * available when it is being rendered.  A lower cost means that
	 * the CPU is more likely to be idle when it is shown, and so it
	 * can afford to be compressed more.  Likewise, a high cost means
	 * that the CPU will be busy and it should not be compressed.
	 *
	 * The default cost is zero, which means average.  Negative cost
	 * can be set for frames that are infrequent; positive cost for the
	 * more difficult times.
	 */
	int cost;

	/*
	 * The starting address for this frame, as an offset from the
	 * start of the image area.
	 */
	unsigned long addr;

	/*
	 * Nonzero if this frame has already been scanned and cannot be
	 * considered for further compression.
	 */
	int already_scanned;
};

/** The master list of all frames */
unsigned int frame_count = 0;
struct frame frame_array[MAX_FRAMES];

/** The file handle for writing the imagemap.h */
FILE *lblfile;

/** The file handle for writing the encoded image data */
FILE *outfile;

/** The default first ROM page used for images.  This can be
changed with the -p option. */
unsigned int base_page = 0x20;

/* By default, images may use up to 64KB.  This can be increased
with the -s option. */
unsigned long max_rom_size = 65536;


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
	if (!strcmp (sep, "pgm"))
		return FORMAT_PGM;
	else
		return FORMAT_BAD;
}


void emit_label (const char *label, unsigned int no)
{
	/* Need to remove the trailing colon from the label name */
	fprintf (lblfile, "#define ");

	while (*label != ':')
		fputc (*label++, lblfile);

	fprintf (lblfile, " %dUL\n", no);
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


/**
 * Converts a linear offset into a target pointer.
 */
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


/**
 * Return the total length of a frame including headers.
 * All frames require an extra byte to say how it was encoded.
 * Frames that are not full-sized require 2 bytes to give the
 * dimensions.
 */
unsigned int frame_length_with_header (struct buffer *buf)
{
	unsigned int len = buf->len + 1;
	if (buf->type & TYPE_BITMAP)
		len += 2;
	return len;
}


/**
 * A list of encoding functions.  Each takes an uncompressed joined
 * bitmap, 512 bytes in length, and returns a new buffer that contains
 * the encoded version.  This list is sorted so that the preferred
 * encoders are listed first.
 */
typedef struct buffer *(*encoder_t) (struct buffer *);
encoder_t encoder_list[] = {
	buffer_rle_encode,
	buffer_sparse_encode,
};


/**
 * Check if frames need to be compressed, and if so, systematically compress
 * them until everything fits.
 */
void compress_frames (void)
{
	struct frame *frame, *aframe;
	int i;
	unsigned long total_size;
	struct buffer *newbuf;

	/* Calculate the total size of ROM space needed.
		We are conservative in our estimate here.
		Add an extra 64 bytes per frame to estimate
		cases where an image has to be pushed to the next page to keep all
		the data together. */
	total_size = (frame_count+1) * 3;
	for (frame = frame_array, i = 0; i < frame_count; i++, frame++)
		total_size += frame->curbuf->len + 1 + 64;

	/* Compress until everything fits */
	while (total_size > max_rom_size)
	{
		//printf ("Total size = %05X, ROM holds %05X\n", total_size, max_rom_size);

		/* Find the uncompressed frame with lowest cost */
		aframe = NULL;
		for (frame = frame_array, i = 0; i < frame_count; i++, frame++)
			if (frame->type == 0 &&
				 !frame->already_scanned &&
				!(frame->curbuf->type & TYPE_BITMAP) &&
				(frame->cost < (aframe ? aframe->cost : 999)))
			{
				aframe = frame;
			}
		//printf ("Will try to compress frame #%d\n", aframe - frame_array);

		/* If everything has been compressed already, and we don't fit still, then
		just give up */
		if (aframe == NULL)
			error ("out of space after compression");

		/* Mark this frame as checked, so we don't try again later */
		aframe->already_scanned = 1;

		/* Try all compression methods.  Take the best one, or none at all
		if compression fails in all cases */
		for (i=0; i < sizeof (encoder_list) / sizeof (encoder_t); i++)
		{
			newbuf = encoder_list[i] (aframe->rawbuf);
			if (newbuf->len < aframe->curbuf->len)
			{
				/* This method is better than all previous ones.
				Make it the current representation. */
				/* printf ("Encoder %d reduces size from %d to %d\n", i,
					aframe->curbuf->len, newbuf->len); */
				newbuf->type |= aframe->rawbuf->type;
				aframe->type = i+1;
				if (aframe->curbuf != aframe->rawbuf)
					buffer_free (aframe->curbuf);
				aframe->curbuf = newbuf;
			}
			else
			{
				/* This method is not better, so just discard it. */
				//printf ("Encoder %d gave %d bytes, ignoring\n", i, newbuf->len);
				buffer_free (newbuf);
			}
		}

		/* Adjust total size, by examining difference between original buffer
		size and the compressed buffer size */
		total_size -= (aframe->rawbuf->len - aframe->curbuf->len);
	}
	/* printf ("Done compressing.  Total size %05X is less than ROM size %05X\n",
		total_size, max_rom_size); */
}


/**
 * Assign a linear start address to each frame.
 */
void assign_addresses (void)
{
	struct frame *frame;
	int i;
	unsigned int addr;

	/* The address of the first image start just beyond the image table header */
	addr = (frame_count+1) * 3;
	for (frame = frame_array, i = 0; i < frame_count; i++, frame++)
	{
		unsigned int len = frame_length_with_header (frame->curbuf);
		frame->addr = round_up_to_page (addr, len);
		//printf ("Frame %d addr = %05lX\n", i, frame->addr);
		addr = frame->addr + len;
	}
}


/**
 * Add a new frame.
 */
void add_frame (const char *label, struct buffer *buf)
{
	struct frame *frame;

	/* If a label was given, go ahead and write that to the
	imagemap.h file */
	if (label)
		emit_label (label, frame_count);

	/* Save the buffer into the frame table */
	frame = &frame_array[frame_count];
	frame->rawbuf = buf;
	frame->curbuf = buf;
	frame->type = 0;
	frame->name = NULL;
	frame->cost = 0;
	frame->addr = 0;
	frame->already_scanned = 0;
	frame_count++;
}


/**
 * Return the Nth plane of a PGM image.
 */
struct buffer *pgm_get_plane (struct buffer *buf, unsigned int plane)
{
	unsigned int level, off;
	struct buffer *planebuf;

	planebuf = buffer_clone (buf);

	for (off = 0; off < planebuf->len; off++)
	{
		/* Each byte in the image contains an intensity value ranging from
		0 to 255.  For FreeWPC images, these need to be scaled down
		to one of 4 levels (0-3). */
		if (buf->data[off] <= 25 * 0xFF / 100)
			level = 0;
		else if (buf->data[off] <= 50 * 0xFF / 100)
			level = 1;
		else if (buf->data[off] <= 75 * 0xFF / 100)
			level = 2;
		else
			level = 3;

		/* Set the data byte to a '1' if the level is enabled in this plane,
		or '0' otherwise. */
		planebuf->data[off] = (level & (1 << plane)) ? 1 : 0;
	}

	/* Convert to a joined buffer, in which the bits are tightly compressed,
	8 pixels to 1 byte */
	planebuf = buffer_replace (planebuf, buffer_joinbits (planebuf));
	return planebuf;
}


/**
 * Add a new image file to the frame list.
 */
void add_image (const char *label, const char *filename, unsigned int options)
{
	FILE *imgfile;
	struct buffer *buf;
	int plane;
	enum image_format format;

	/* Open the image file */
	imgfile = fopen (filename, "r");
	if (!imgfile)
		error ("can't open image file '%s'\n", filename);

	/* See what format the file is in.  For now, only PGM is supported.
	Read the image into a bitmap buffer. */
	format = get_file_format (filename);
	if (format == FORMAT_PGM)
	{
		buf = buffer_alloc (FRAME_BYTE_SIZE);
		buffer_read_pgm (buf, imgfile);
	}
	else
		error ("invalid image format\n");

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
		if ((buf->width < 128) || (buf->height < 32))
			planebuf->type |= TYPE_BITMAP;
		add_frame (!plane ? label : NULL, planebuf);
	}

	/* Free the original image buffer */
	buffer_free (buf);
}


/**
 * Write all images to the output file.
 */
void write_output (const char *filename)
{
	unsigned int frameno;
	unsigned long offset;
	struct buffer *buf;
	unsigned char target_pointer[3];
	unsigned char padding = 0xFF;
	struct frame *frame;
	unsigned int padding_size;

	/* Open the file for output */
	outfile = fopen (filename, "w");
	if (!outfile)
		error ("can't open output file '%s'\n", filename);

	/* Write the frame table header. */
	for (frameno = 0, offset = (frame_count + 1) * 3, frame = frame_array, buf = frame->curbuf;
		frameno < frame_count;
		frameno++, offset += frame_length_with_header (buf), frame++, buf = frame->curbuf)
	{
		/* Round up to the next page boundary if the image doesn't
		 * completely fit in the current page. */
		offset = round_up_to_page (offset, buf->len+1);

		/* Convert the absolute offset from the beginning of image data into
		 * a target pointer, and write it to the table header. */
		convert_to_target_pointer (offset, target_pointer);
		fwrite (target_pointer, sizeof (target_pointer), 1, outfile);

		if (frameno == 0)
		{
			fprintf (lblfile, "\n#define IMAGEMAP_BASE 0x%04X\n", target_props->page_base);
			fprintf (lblfile, "#define IMAGEMAP_PAGE 0x%02X\n", base_page);
		}

		fprintf (lblfile, "/* %d: %02X/%02X%02X, type %02X, len %d (%d x %d) */\n",
			frameno, target_pointer[2], target_pointer[0], target_pointer[1],
			buf->type, buf->len, buf->width, buf->height);
	}

	/* Write a NULL pointer at the end of the table.  Not strictly needed anymore,
	since the number of table entries is available as a #define. */
	convert_to_target_pointer (0, target_pointer);
	fwrite (target_pointer, sizeof (target_pointer), 1, outfile);


	/* Write the frame table data */
	for (frameno = 0, offset = (frame_count + 1) * 3, frame = frame_array, buf = frame->curbuf;
		frameno < frame_count;
		frameno++, offset += frame_length_with_header (buf), frame++, buf = frame->curbuf)
	{
		/* If the object address needed to be pushed to the next
		page boundary, then output padding bytes first. */
		padding_size = round_up_to_page (offset, buf->len+1) - offset;
		while (padding_size > 0)
		{
			fwrite (&padding, sizeof (padding), 1, outfile);
			offset++;
			padding_size--;
		}

		/* Output the image data itself.  Variable-sized bitmaps include the
		width and height. */
		fputc (buf->type, outfile);
		if (buf->type & TYPE_BITMAP)
		{
			fputc (buf->width, outfile);
			fputc (buf->height, outfile);
		}
		buffer_write (buf, outfile);
	}
	fclose (outfile);

	/* Check that the offset has not advanced past the file size limit */
	if (offset > max_rom_size)
		error ("output is too large: 0x%05lX > 0x%05lX\n", offset, max_rom_size);
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

	/* Parse options and process the image lists. */
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
					printf ("-s <1k-blocks>               Set the maximum output file size\n");
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
					max_rom_size = 1024 * strtoul (argv[++argn], NULL, 0);
					break;
			}
		}
		else
		{
			/* Any non-option argument is treated as a config file, which is loaded
			and parsed.  Any images named in these files are loaded. */
			parse_config (arg);
		}
	}

	/* Finalize all output */
	assign_addresses ();
	compress_frames ();

	/* Write the image table */
	write_output (outfilename);

	fprintf (lblfile, "\n#define MAX_IMAGE_NUMBER %d\n", frame_count);
	fprintf (lblfile, "\n#endif /* _IMGLD_IMAGEMAP_H */\n");
	fclose (lblfile);

	exit (0);
}

