/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief DMD-related definitions.
 */

#ifndef _SYS_DMD_H
#define _SYS_DMD_H

/** The width of a DMD page, in pixels */
#define DMD_PIXEL_WIDTH	128

/** The height of a DMD page, in pixel */
#define DMD_PIXEL_HEIGHT 32

/** The width of a DMD page, in bytes */
#define DMD_BYTE_WIDTH (DMD_PIXEL_WIDTH / 8)

/** The size of each DMD page */
//#define DMD_PAGE_SIZE (DMD_BYTE_WIDTH * DMD_PIXEL_HEIGHT)
#define DMD_PAGE_SIZE 512UL

/** The number of DMD pages in the controller.  There are physically
 * this number of page buffers that can be drawn to at the same time.
 * However, not all pages are mapped into the 6809 address space
 * simultaneously -- only 2 at a time are mapped.  WPC-95 supports
 * 6 mapped at a time, but FreeWPC doesn't take advantage of that now. */
#define DMD_PAGE_COUNT 16

/** The number of lookaside frames */
#define DMD_LOOKASIDE_FRAME_COUNT 2
#define DMD_LOOKASIDE_PAGE_COUNT (DMD_LOOKASIDE_FRAME_COUNT * 2)

/** The number of blank pages kept */
#define DMD_BLANK_PAGE_COUNT 2

#define DMD_ALLOC_PAGE_COUNT \
	(DMD_PAGE_COUNT - DMD_LOOKASIDE_PAGE_COUNT - DMD_BLANK_PAGE_COUNT)

/** Coordinates that are aligned various ways */
#define DMD_CENTER_X (DMD_PIXEL_WIDTH / 2)
#define DMD_CENTER_Y (DMD_PIXEL_HEIGHT / 2)

#define DMD_LEFT_CENTER_X (DMD_PIXEL_WIDTH / 3)
#define DMD_RIGHT_CENTER_X (2 * DMD_PIXEL_WIDTH / 3)
#define DMD_TOP_CENTER_Y (DMD_PIXEL_HEIGHT / 3)
#define DMD_BOTTOM_CENTER_Y (2 * DMD_PIXEL_HEIGHT / 3)

#define DMD_ALIGN_CENTER         DMD_CENTER_X, DMD_CENTER_Y
#define DMD_ALIGN_TOP_CENTER     DMD_CENTER_X, DMD_TOP_CENTER_Y
#define DMD_ALIGN_BOTTOM_CENTER  DMD_CENTER_X, DMD_BOTTOM_CENTER_Y

/**
 * The row value at which an FIRQ should normally be generated in order to do
 * page flipping.  By requesting the FIRQ just before the complete page is
 * refreshed, software has time to reprogram the VISIBLE page register
 * just before the hardware will latch it.
 */
#define DMD_REFRESH_ROW 30

/** The type of a page number */
typedef U8 dmd_pagenum_t;

/** The type of a page pair, which tracks the two pages
needed to display a 4-color image.  Normally, the
dark and bright page numbers are consecutive, but for
mono images, they could be the same value. */
typedef union
{
	struct {
		dmd_pagenum_t first;
		dmd_pagenum_t second;
	} u;
	U16 pair;
} dmd_pagepair_t;

/** The type of a page buffer pointer */
typedef U8 *dmd_buffer_t;


/** A DMD transition describes all of the properties of a
 * transition, such as the functions that render each
 * successive frame change, timing, etc.
 */
typedef struct
{
	/** Initialization function */
	void (*composite_init) (void);

	/** Render the part of the transitional image that comes from
	 * the previous image. */
	void (*composite_old) (void);

	/** Render the part of the transitional image that comes from
	 * the new image.   This function is also responsible for
	 * determining when the transition is finished; it should
	 * dmd_in_transition to FALSE when this happens.
	 */
	void (*composite_new) (void);

	/** A parameter that determines how fast the transition occurs.
	 * This is the delay between transitional frames, and is given
	 * as a TIME_xxx define. */
	U8 delay;

	/** Extra data used to create multiple transitions using common
	 * code but with slight differences.  Each transition interprets
	 * this field differently.  It could be used to hold a pointer or
	 * an offset, so a union is used. */
	union {
		U16 u16;
		void *ptr;
	} arg;

	/** Number of iterations in the transition */
	U8 count;
} dmd_transition_t;


/**
 * Request an FIRQ to be generated after row 'v' has been displayed.
 */
#define wpc_dmd_set_firq_row(v)		writeb (WPC_DMD_FIRQ_ROW_VALUE, v)


/**
 * Return the last row that was refreshed.
 */
#define wpc_dmd_get_firq_row()      readb(WPC_DMD_FIRQ_ROW_VALUE)


/**
 * Set the visible page.
 */
#define wpc_dmd_set_visible_page(v)	writeb (WPC_DMD_ACTIVE_PAGE, v)

#define dmd_low_buffer			((dmd_buffer_t)DMD_LOW_BASE)
#define dmd_high_buffer			((dmd_buffer_t)DMD_HIGH_BASE)

extern U8 *dmd_trans_data_ptr;
extern bool dmd_in_transition;
extern dmd_transition_t *dmd_transition;
extern dmd_pagepair_t dmd_mapped_pages;
extern dmd_pagepair_t dmd_visible_pages;

#define dmd_low_page dmd_mapped_pages.u.first
#define dmd_high_page dmd_mapped_pages.u.second
#define dmd_dark_page dmd_visible_pages.u.first
#define dmd_bright_page dmd_visible_pages.u.second

extern dmd_transition_t 
	trans_scroll_up,
	trans_scroll_up_avg,
	trans_scroll_up_slow,
	trans_scroll_down,
	trans_scroll_left,
	trans_scroll_right,
	trans_sequential_boxfade,
	trans_random_boxfade,
	trans_vstripe_left2right,
	trans_vstripe_right2left,
	trans_bitfade_slow,
	trans_bitfade_fast
	;


/** Verify that the destination address for writing to a DMD page is valid.
 * This is only used in native mode, to make sure that we do not write
 * outside the bounds of the emulated buffer.  No such check is done
 * in 6809 mode. */
extern inline U8 *wpc_dmd_addr_verify (U8 *addr)
{
#ifdef DMD_LOW_BASE
#ifdef CONFIG_NATIVE
	if ((addr >= DMD_LOW_BASE) && (addr <= DMD_LOW_BASE + DMD_PAGE_SIZE))
		return addr;
	else if ((addr >= DMD_HIGH_BASE) && (addr <= DMD_HIGH_BASE + DMD_PAGE_SIZE))
		return addr;
	else
		fatal (ERR_INVALID_IO_ADDR);
#endif
#endif
	return addr;
}

/*
 * The DMD controller has two registers for controlling which pages
 * are mapped into addressable memory.  
 *
 * Because these registers are write-only, writes are also cached into
 * variables.  Then reads can be done using the cached values.
 */

extern inline dmd_pagepair_t wpc_dmd_get_mapped (void)
{
	return dmd_mapped_pages;
}

extern inline void wpc_dmd_set_mapped (dmd_pagepair_t mapping)
{
	dmd_mapped_pages = mapping;
	writeb (WPC_DMD_LOW_PAGE, mapping.u.first);
	writeb (WPC_DMD_HIGH_PAGE, mapping.u.second);
}

extern inline void wpc_dmd_set_low_page (U8 val)
{
	writeb (WPC_DMD_LOW_PAGE, dmd_low_page = val);
}

extern inline U8 wpc_dmd_get_low_page (void)
{
	return dmd_low_page;
}

extern inline void wpc_dmd_set_high_page (U8 val)
{
	writeb (WPC_DMD_HIGH_PAGE, dmd_high_page = val);
}

extern inline U8 wpc_dmd_get_high_page (void)
{
	return dmd_high_page;
}

extern inline dmd_pagenum_t dmd_get_lookaside (const U8 num)
{
	return DMD_ALLOC_PAGE_COUNT + num * 2;
}

extern inline dmd_pagenum_t dmd_get_blank (const U8 num)
{
	return DMD_ALLOC_PAGE_COUNT + DMD_LOOKASIDE_PAGE_COUNT + num;
}


void dmd_init (void);
extern __fastram__ void (*dmd_rtt) (void);
void dmd_alloc_low (void);
void dmd_alloc_high (void);
void dmd_alloc_pair (void);
void dmd_map_low_high (dmd_pagenum_t page);
void dmd_show_low (void);
void dmd_show_high (void);
void dmd_show_other (void);
void dmd_flip_low_high (void);
void dmd_show2 (void);
void dmd_clean_page (dmd_buffer_t dbuf);
void dmd_memset (dmd_buffer_t dbuf, U8 val);
void dmd_clean_page_low (void);
void dmd_clean_page_high (void);
void dmd_fill_page_low (void);
void dmd_invert_page (dmd_buffer_t dbuf);
void dmd_copy_page (dmd_buffer_t dst, const dmd_buffer_t src);
void dmd_copy_low_to_high (void);
void dmd_alloc_low_clean (void);
void dmd_alloc_pair_clean (void);
void dmd_draw_border (U8 *dbuf);
void dmd_draw_horiz_line (U16 *dbuf, U8 y);
void dmd_shift_up (dmd_buffer_t dbuf);
void dmd_shift_down (dmd_buffer_t dbuf);
void dmd_draw_bitmap (dmd_buffer_t image_bits, U8 x, U8 y, U8 width, U8 height);
void dmd_do_transition (void);
void dmd_sched_transition (dmd_transition_t *trans);
void dmd_reset_transition (void);
__attribute__((deprecated)) const U8 *dmd_draw_fif1 (const U8 *fif);
void frame_draw (U16 id);
void frame_draw2 (U16 id);
void bmp_draw (U8 x, U8 y, U16 id);

__transition__ void dmd_text_outline (void);
__transition__ void dmd_text_blur (void);
__transition__ void dmd_overlay_alpha (dmd_pagepair_t dst, U8 src);
__transition__ void dmd_overlay2 (dmd_pagepair_t dst, U8 src);
__transition__ void dmd_overlay (dmd_pagepair_t dst, U8 src);
__transition__ void dmd_dup_mapped (void);

#ifdef __m6809__
void dmd_and_page (void);
void dmd_or_page (void);
void dmd_xor_page (void);
void frame_decode_rle_asm (U8 *);
void frame_decode_sparse_asm (U8 *);
void dmd_copy_asm (dmd_buffer_t, dmd_buffer_t);
#define frame_decode_rle frame_decode_rle_asm
#define frame_decode_sparse frame_decode_sparse_asm
#else
/* TODO - no C versions for some of these */
#define dmd_and_page()
#define dmd_or_page()
#define dmd_xor_page()
#define frame_decode_rle frame_decode_rle_c
#define frame_decode_sparse(p)
#endif

extern inline void dmd_map_lookaside (const U8 num)
{
	dmd_map_low_high ( dmd_get_lookaside (num) );
}


#define dmd_draw_fif(fif) \
do { \
	extern const U8 fif[]; \
	dmd_draw_fif1 (fif); \
} while (0)


/*
 * The parameters to a rough copy or erase operation.
 * The user parameters are given in terms of pixels, however these
 * must be converted into byte coordinates.
 */

struct dmd_rough_args
{
	U8 *dst;
	U8 bwidth;
	U8 height;
};


/**
 * Copy a portion of the low DMD page into the high page.
 *
 * x,y denote the coordinates of the upper leftmost pixel.
 * w is the width of the area to be copied in pixels.
 * h is the height of the area to be copied in pixels.
 * All pixel values should be a multiple of 8 for correct results.
 */
#define dmd_rough_copy(x, y, w, h) \
	do { \
		extern struct dmd_rough_args dmd_rough_args; \
		dmd_rough_args.dst = DMD_HIGH_BASE + (x / CHAR_BIT) + y * DMD_BYTE_WIDTH; \
		dmd_rough_args.bwidth = w / CHAR_BIT; \
		dmd_rough_args.height = h; \
		dmd_rough_copy1 (); \
	} while (0)

/**
 * Zero a portion of the low DMD page.
 *
 * x,y denote the coordinates of the upper leftmost pixel.
 * width is the width of the area to be erased in pixels.
 * height is the height of the area to be erased in pixels.
 * All pixel values should be a multiple of 8 for correct results.
 */
#define dmd_rough_erase(x, y, width, height) \
	do { \
		extern struct dmd_rough_args dmd_rough_args; \
		dmd_rough_args.dst = DMD_HIGH_BASE + (x / CHAR_BIT) + y * DMD_BYTE_WIDTH; \
		dmd_rough_args.bwidth = w / CHAR_BIT; \
		dmd_rough_args.height = h; \
		dmd_rough_erase1 (); \
	} while (0)


#endif /* _SYS_DMD_H */
