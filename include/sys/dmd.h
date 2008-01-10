/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SYS_DMD_H
#define _SYS_DMD_H

/** The size of each DMD page */
#define DMD_PAGE_SIZE 0x200UL

/** The number of DMD pages */
#define DMD_PAGE_COUNT 16

/** The width of a DMD page, in pixels */
#define DMD_PIXEL_WIDTH	128

/** The width of a DMD page, in bytes */
#define DMD_BYTE_WIDTH (DMD_PIXEL_WIDTH / 8)

/** The height of a DMD page, in pixel */
#define DMD_PIXEL_HEIGHT 32


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


/** The type of a page number */
typedef U8 dmd_pagenum_t;

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


#define wpc_dmd_set_firq_row(v)		wpc_asic_write (WPC_DMD_FIRQ_ROW_VALUE, v)
#define wpc_dmd_set_visible_page(v)	wpc_asic_write (WPC_DMD_ACTIVE_PAGE, v)

#define dmd_low_buffer			((dmd_buffer_t)DMD_LOW_BASE)
#define dmd_high_buffer			((dmd_buffer_t)DMD_HIGH_BASE)

extern U8 *dmd_trans_data_ptr;
extern bool dmd_in_transition;
extern dmd_transition_t *dmd_transition;

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
#ifdef CONFIG_NATIVE
	if ((addr >= DMD_LOW_BASE) && (addr <= DMD_LOW_BASE + DMD_PAGE_SIZE))
		return addr;
	else if ((addr >= DMD_HIGH_BASE) && (addr <= DMD_HIGH_BASE + DMD_PAGE_SIZE))
		return addr;
	else
		fatal (ERR_INVALID_IO_ADDR);
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

extern inline void wpc_dmd_set_low_page (U8 val)
{
	extern U8 dmd_low_page;
	wpc_asic_write (WPC_DMD_LOW_PAGE, dmd_low_page = val);
}

extern inline U8 wpc_dmd_get_low_page (void)
{
	extern U8 dmd_low_page;
	return dmd_low_page;
}

extern inline void wpc_dmd_set_high_page (U8 val)
{
	extern U8 dmd_high_page;
	wpc_asic_write (WPC_DMD_HIGH_PAGE, dmd_high_page = val);
}

extern inline U8 wpc_dmd_get_high_page (void)
{
	extern U8 dmd_high_page;
	return dmd_high_page;
}


void dmd_init (void);
extern __fastram__ void (*dmd_rtt) (void);
void dmd_alloc_low (void);
void dmd_alloc_high (void);
void dmd_alloc_low_high (void);
void dmd_show_low (void);
void dmd_show_high (void);
void dmd_show_other (void);
void dmd_flip_low_high (void);
void dmd_show2 (void);
void dmd_clean_page (dmd_buffer_t dbuf);
void dmd_clean_page_low (void);
void dmd_clean_page_high (void);
void dmd_fill_page_low (void);
void dmd_invert_page (dmd_buffer_t dbuf);
void dmd_mask_page (dmd_buffer_t dbuf, U16 mask);
void dmd_copy_page (dmd_buffer_t dst, const dmd_buffer_t src);
void dmd_copy_low_to_high (void);
void dmd_alloc_low_clean (void);
void dmd_alloc_pair_clean (void);
void dmd_draw_border (U8 *dbuf);
void dmd_draw_horiz_line (U16 *dbuf, U8 y);
void dmd_shift_up (dmd_buffer_t dbuf);
void dmd_shift_down (dmd_buffer_t dbuf);
__attribute__((deprecated)) void dmd_draw_image (const U8 *image_bits);
__attribute__((deprecated)) void dmd_draw_image2 (const U8 *image_bits);
void dmd_draw_bitmap (dmd_buffer_t image_bits, 
	U8 x, U8 y, U8 width, U8 height);
void dmd_erase_region (U8 x, U8 y, U8 width, U8 height);
void dmd_do_transition (void);
void dmd_sched_transition (dmd_transition_t *trans);
void dmd_reset_transition (void);
const U8 *dmd_draw_fif1 (const U8 *fif);

#define dmd_draw_fif(fif) \
do { \
	extern const U8 fif[]; \
	dmd_draw_fif1 (fif); \
} while (0)

#endif /* _SYS_DMD_H */
