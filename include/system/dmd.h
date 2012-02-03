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
 * \brief DMD-related definitions.
 *
 * This file contains definitions which are common to all dot
 * matrix games.  Platform specific values are defined elsewhere
 * with the pinio_ prefix.
 */

#ifndef _SYS_DMD_H
#define _SYS_DMD_H

/** The width of a DMD page, in bytes */
#define DMD_BYTE_WIDTH (PINIO_DMD_WIDTH / 8)

/** The size of each DMD page, in bytes */
#define DMD_PAGE_SIZE (1UL * DMD_BYTE_WIDTH * PINIO_DMD_HEIGHT)

/** The number of pages reserved for the overlay(s).  We reserve a single
 * pair of pages for this now. */
#define DMD_OVERLAY_PAGE_COUNT 2

/** The number of blank pages kept */
#define DMD_BLANK_PAGE_COUNT 2

#define DMD_ALLOC_PAGE_COUNT \
	(PINIO_NUM_DMD_PAGES - DMD_OVERLAY_PAGE_COUNT - DMD_BLANK_PAGE_COUNT)

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


#define dmd_low_buffer			((dmd_buffer_t)pinio_dmd_window_ptr (PINIO_DMD_WINDOW_0))
#define dmd_high_buffer			((dmd_buffer_t)pinio_dmd_window_ptr (PINIO_DMD_WINDOW_1))

extern U8 *dmd_trans_data_ptr;
extern bool dmd_in_transition;
extern dmd_transition_t *dmd_transition;
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
	trans_scroll_down_fast,
	trans_scroll_left,
	trans_scroll_right,
	trans_sequential_boxfade,
	trans_random_boxfade,
	trans_vstripe_left2right,
	trans_vstripe_right2left,
	trans_bitfade_slow,
	trans_bitfade_fast,
	trans_unroll_vertical
	;


/** Verify that the destination address for writing to a DMD page is valid.
 * This is only used in native mode, to make sure that we do not write
 * outside the bounds of the emulated buffer.  No such check is done
 * in 6809 mode for performance. */
extern inline U8 *wpc_dmd_addr_verify (U8 *addr)
{
#ifdef CONFIG_NATIVE
	if ((addr >= pinio_dmd_window_ptr (PINIO_DMD_WINDOW_0)) &&
		(addr <= pinio_dmd_window_ptr (PINIO_DMD_WINDOW_0) + DMD_PAGE_SIZE))
		return addr;
	else if ((addr >= pinio_dmd_window_ptr (PINIO_DMD_WINDOW_1)) &&
		(addr <= pinio_dmd_window_ptr (PINIO_DMD_WINDOW_1) + DMD_PAGE_SIZE))
		return addr;
	else
		fatal (ERR_INVALID_IO_ADDR);
#endif
	return addr;
}


extern inline dmd_pagepair_t wpc_dmd_get_mapped (void)
{
	return dmd_mapped_pages;
}

extern inline void wpc_dmd_set_mapped (dmd_pagepair_t mapping)
{
	pinio_dmd_window_set (PINIO_DMD_WINDOW_0, mapping.u.first);
	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, mapping.u.second);
}

#define DMD_OVERLAY_PAGE DMD_ALLOC_PAGE_COUNT

extern inline dmd_pagenum_t dmd_get_blank (const U8 num)
{
	return DMD_ALLOC_PAGE_COUNT + DMD_OVERLAY_PAGE_COUNT + num;
}


void dmd_init (void);
extern __fastram__ void (*dmd_rtt) (void);
void dmd_alloc_low (void);
void dmd_alloc_pair (void);
void dmd_map_low_high (dmd_pagenum_t page);
void dmd_show_low (void);
void dmd_show_high (void);
void dmd_show_other (void);
void deff_swap_low_high (S8 count, task_ticks_t delay);
void dmd_flip_low_high (void);
void dmd_show2 (void);
void dmd_clean_page (dmd_buffer_t dbuf);
void dmd_clean_page_low (void);
void dmd_clean_page_high (void);
void dmd_fill_page_low (void);
void dmd_invert_page (dmd_buffer_t dbuf);
void dmd_copy_page (dmd_buffer_t dst, const dmd_buffer_t src);
void dmd_copy_low_to_high (void);
void dmd_alloc_low_clean (void);
void dmd_alloc_pair_clean (void);
void dmd_shift_up (dmd_buffer_t dbuf);
void dmd_shift_down (dmd_buffer_t dbuf);
void dmd_draw_bitmap (dmd_buffer_t image_bits, U8 x, U8 y, U8 width, U8 height);
void dmd_do_transition (void);
void dmd_sched_transition (dmd_transition_t *trans);
void dmd_reset_transition (void);
void frame_draw (U16 id);
void frame_draw2 (U16 id);
void frame_draw_plane (U16 id);
void bmp_draw (U8 x, U8 y, U16 id);

__transition__ void dmd_text_outline (void);
__transition__ void dmd_text_blur (void);
__transition__ void dmd_overlay (void);
__transition__ void dmd_overlay_outline (void);
__transition__ void dmd_overlay_color (void);
__transition__ void dmd_overlay_onto_color (void);
__transition__ void dmd_dup_mapped (void);

__effect__ void dmd_draw_border (U8 *dbuf);
__effect__ void dmd_draw_thin_border (U8 *dbuf);
__effect__ void dmd_draw_horiz_line (U16 *dbuf, U8 y);

void dmd_and_page (void);
void dmd_or_page (void);
void dmd_xor_page (void);
#ifdef __m6809__
void frame_decode_rle_asm (U8 *);
void frame_decode_sparse_asm (U8 *);
void dmd_copy_asm (dmd_buffer_t, dmd_buffer_t);
#define frame_decode_rle frame_decode_rle_asm
#define frame_decode_sparse frame_decode_sparse_asm
#else
/* TODO - no C versions for some of these */
#define frame_decode_rle frame_decode_rle_c
#define frame_decode_sparse(p)
#endif

extern inline void dmd_map_overlay (void)
{
	dmd_map_low_high (DMD_OVERLAY_PAGE);
}

/*
 * The parameters to a rough copy or erase operation.
 * The user parameters are completely given in terms of pixels.
 * The x position and size are converted into a byte-based offset.
 */

struct dmd_rough_args
{
	U8 *dst;
	union dmd_coordinate size;
};


/**
 * Copy a portion of the low DMD page into the high page.
 *
 * x,y denote the coordinates of the upper leftmost pixel.
 * w is the width of the area to be copied in pixels.
 * h is the height of the area to be copied in pixels.
 * x and w (col. values) should be a multiple of 8 for correct results.
 */
#define dmd_rough_copy(x, y, w, h) \
	do { \
		extern struct dmd_rough_args dmd_rough_args; \
		dmd_rough_args.dst = pinio_dmd_window_ptr (PINIO_DMD_WINDOW_1) + \
			((x) / CHAR_BIT) + (U16)(y) * DMD_BYTE_WIDTH; \
		dmd_rough_args.size.xy = MKCOORD1 ((w) / CHAR_BIT, (h)); \
		dmd_rough_copy1 (); \
	} while (0)

__transition__ void dmd_rough_copy1 (void);

/**
 * Zero a portion of the low DMD page.
 *
 * x,y denote the coordinates of the upper leftmost pixel.
 * width is the width of the area to be erased in pixels.
 * height is the height of the area to be erased in pixels.
 * x and w (col. values) should be a multiple of 8 for correct results.
 */
#define dmd_rough_erase(x, y, w, h) \
	do { \
		extern struct dmd_rough_args dmd_rough_args; \
		dmd_rough_args.dst = pinio_dmd_window_ptr (PINIO_DMD_WINDOW_0) + \
			((x) / CHAR_BIT) + (U16)(y) * DMD_BYTE_WIDTH; \
		dmd_rough_args.size.xy = MKCOORD1 ((w) / CHAR_BIT, (h)); \
		dmd_rough_erase1 (); \
	} while (0)

__transition__ void dmd_rough_erase1 (void);



/**
 * Invert a portion of the low DMD page.
 *
 * x,y denote the coordinates of the upper leftmost pixel.
 * width is the width of the area to be erased in pixels.
 * height is the height of the area to be erased in pixels.
 * All pixel values should be a multiple of 8 for correct results.
 */
#define dmd_rough_invert(x, y, w, h) \
	do { \
		extern struct dmd_rough_args dmd_rough_args; \
		dmd_rough_args.dst = pinio_dmd_window_ptr (PINIO_DMD_WINDOW_0) + \
			((x) / CHAR_BIT) + (y) * DMD_BYTE_WIDTH; \
		dmd_rough_args.size.xy = MKCOORD1 ((w) / CHAR_BIT, (h)); \
		dmd_rough_invert1 (); \
	} while (0)

__transition__ void dmd_rough_invert1 (void);

__transition__ void dmd_row_copy (U8 *dst);
__transition__ void dmd_column_copy (U8 *dst);

#endif /* _SYS_DMD_H */
