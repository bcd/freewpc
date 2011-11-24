/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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
 * \brief Render full-sized DMD frames.
 *
 * This module provides an API for drawing full-screen 128x32 4-color images
 * to the dot matrix.  Use the function frame_draw(), passing one of the
 * IMG_xxxx defines given in the machine's image list (ild) file.
 */

#include <freewpc.h>

/**
 * The way that images are accessed is very different in 6809 vs. native mode.
 */

#ifdef CONFIG_NATIVE
#undef IMAGEMAP_BASE
U8 IMAGEMAP_BASE[350000];

struct frame_pointer
{
	U8 ptr_hi;
	U8 ptr_lo;
	U8 page;
} __attribute__((packed));

#define PTR(p) (&IMAGEMAP_BASE[((p->ptr_hi - 0x40) * 256UL + p->ptr_lo) + (p->page * 0x4000UL)])

#else /* 6809 */

struct frame_pointer
{
	unsigned char *ptr;
	U8 page;
};

#define PTR(p) (p->ptr)

#endif

#ifdef IMAGEMAP_PAGE

#ifndef __m6809__
void frame_decode_rle_c (U8 *data)
{
	U16 *src = (U16 *)data;
	U16 *dst = (U16 *)dmd_low_buffer;

	while ((U8 *)dst < dmd_low_buffer + 512)
	{
		U16 val = *src++;
		if ((val & 0xFF00) == 0xA800)
		{
			U8 words = val & 0xFF;
			U8 repeater = *((U8 *)src);
			U16 repeated_word = repeater | ((U16)repeater << 8);
			src = (U16 *)((U8 *)src + 1);
			while (words > 0)
			{
				*dst++ = repeated_word;
				words--;
			}
		}
		else
		{
			*dst++ = val;
		}
	}
}
#endif


/**
 * Decode the source of a DMD frame.  DATA points to the
 * source data; the ROM page is already mapped.  TYPE
 * says how it was encoded.
 */
void frame_decode (U8 *data, U8 type)
{
	if (type == 0)
	{
		dmd_copy_page (dmd_low_buffer, (const dmd_buffer_t)data);
	}
	else if (type == 2)
	{
		frame_decode_rle (data);
	}
	else if (type == 4)
	{
		frame_decode_sparse (data);
	}
}

/**
 * Draw one plane of a DMD frame.
 * ID identifies the source of the frame data.
 * The output is always drawn to the low-mapped buffer.
 */
void frame_draw_plane (U16 id)
{
	/* Lookup the image number in the global table.
	 * For real ROMs, this is located at a fixed address.
	 * In native mode, the images are kept in a separate file.
	 */
	U8 type;
	struct frame_pointer *p;
	U8 *data;

	page_push (IMAGEMAP_PAGE);
	p = (struct frame_pointer *)IMAGEMAP_BASE + id;
	data = PTR(p);

	/* Switch to the page containing the image data.
	 * Pull the type byte out, then decode the remaining bytes
	 * to the display buffer. */
	pinio_set_bank (PINIO_BANK_ROM, p->page);
	type = data[0];
	frame_decode (data + 1, type & ~0x1);

	page_pop ();
}


/**
 * Draw a 2-plane, 4-color DMD frame.
 * ID identifies the first plane of the frame.  The two
 * frames have consecutive IDs.
 */
void frame_draw (U16 id)
{
	frame_draw_plane (id++);
	dmd_flip_low_high ();
	frame_draw_plane (id);
	dmd_flip_low_high ();
}


/**
 * Draw an arbitrary sized bitmap at a particular region
 * of the display.
 */
void bmp_draw (U8 x, U8 y, U16 id)
{
	struct frame_pointer *p;

	page_push (IMAGEMAP_PAGE);
	p = (struct frame_pointer *)IMAGEMAP_BASE + id;
	page_push (p->page);
	bitmap_blit (PTR(p) + 1, x, y);
	if (PTR(p)[0] & 0x1)
	{
		dmd_flip_low_high ();
		p++;
		bitmap_blit (PTR(p) + 1, x, y);
		dmd_flip_low_high ();
	}
	page_pop ();
	page_pop ();
}


CALLSET_ENTRY (frame, init)
{
	/* In native mode, read the images from an external file into memory. */
#ifdef CONFIG_NATIVE
	FILE *fp;
	const char *filename = "build/" MACHINE_SHORTNAME "_images.rom";
	fp = fopen (filename, "rb");
	if (!fp)
	{
		dbprintf ("Cannot open image file %s\n", filename);
		return;
	}
	if (!fread (IMAGEMAP_BASE, sizeof (U8), 262144, fp))
	{
		dbprintf ("Image file read error\n");
		return;
	}
#endif
}

#endif /* IMAGEMAP_PAGE */

