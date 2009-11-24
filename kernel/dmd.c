/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Manages the dot matrix controller (DMD)
 *
 * The DMD module manages the physical DMD resources.  The DMD
 * supports 16 pages which can be written to.  One of these pages
 * can be designated as "visible", which is what the player sees.
 * Two of these pages can be "mapped" into address space for
 * write access; unmapped pages aren't accessible.  The visible
 * and mapped pages may or may not be the same.
 *
 * 4-color images are supported through the use of page flipping.
 * Two DMD pages are reserved, one to hold the "dark" bits, another
 * to hold the "bright" bits.  The FIRQ handler switches the
 * visible image rapidly to fool the eye into seeing 4 colors.
 * The dark bits are shown 1/3 of the time; the bright bits are
 * shown 2/3 of the time.  When a mono image is desired, page
 * flipping still happens, but the dark page and the bright page
 * are the same.
 *
 * Two virtual registers, low_page and high_page, are implemented
 * in RAM over the real mapping registers.  This is because the
 * hardware registers are not readable.  Likewise, two virtual
 * registers named dark_page and bright_page, track which pages
 * have been allocated for 4-color imaging.
 *
 * The DMD controller contains the logic for refreshing the
 * display from the current visible page contents.  It clocks
 * 1 byte into the display every 32 CPU cycles, which equates to
 * one full page in roughly 8ms, or 122Hz.  In 4-color mode, two
 * pages are drawn in 3 phases (see above) for an actual refresh
 * rate of about 40Hz.  Trying to use more pages for more color
 * fails to work as the refresh rate becomes so low that the
 * page flipping becomes detectable by the human eye.
 *
 * This module also implements the generic transition algorithm.
 * All transitions share some common logic that is done here.  The
 * specifics of a particular transition are implemented in callback
 * functions defined in dmdtrans.c.
 *
 * The typical usage model for a display effect or any other code
 * that wants to write to the DMD is to (1) allocate fresh DMD pages,
 * (2) draw on them, and (3) show them.  This is so that during the
 * time the pages are being rendered, the old image continues to be
 * visible, with no drawing artifacts.
 *
 * Allocation is done very simply by iteration.  Pages are always
 * allocated in pairs in case 4-colors are desired.  No more than
 * two pairs are ever needed at once, so there is no concern for
 * overflow.  A few pages are reserved for special use and are
 * always skipped by the allocator.
 */

#include <freewpc.h>

/** Points to the next free page that can be allocated */
dmd_pagenum_t dmd_free_page;

/** Low/High cache the current pages that are mapped into
 * visible memory.  Note that you can't read the I/O
 * register directly; they are write-only. */
dmd_pagepair_t dmd_mapped_pages;

/** Dark/Bright store the 2 pages that are used to actually
 * draw on the display.  These values are programmed into
 * the DMD 'visible' register.  The values are switched fast
 * to give the appearance of 3-color images.  The 'dark'
 * page is shown 1/3 of the time, while the 'bright' page
 * is shown 2/3 of the time.  (The brightest pixels are
 * those that are set in both of the pages at the same time.)
 */
__fastram__ dmd_pagepair_t dmd_visible_pages;

/** dmd_show_hook is normally set to a nop function.
 * However, whenever a deff is started/stopped that defines
 * an entry/exit transition function, the show_hook is changed
 * to invoke dmd_do_transition, which performs the required
 * transition before continuing.
 */
bool dmd_in_transition;

/** Pointer to the current transition in effect.  This is
 * only used by the transition show hook. */
dmd_transition_t *dmd_transition;

__fastram__ void (*dmd_rtt) (void);


/** The trans data pointer provides transition functions with
 * state.  The pointer is initialized to NULL before the
 * first call to the transition functions; they can use it
 * however they wish.  Typically, it would be used to
 * save a pointer into the DMD buffer, indicating what should
 * be updated on the next cycle of the transition.
 */
U8 *dmd_trans_data_ptr;
U8 *dmd_trans_data_ptr2;

/** The page number of the composite page, used during
 * transitions.  Each frame of the transition sequence
 * is stored here.  If the frame is 4-color, then two
 * pages are allocated with consecutive numbers, and
 * this holds the lower of the two values (the dark page).
 */
U8 dmd_composite_page;

/* Forward declarations of the 3 phases of the DMD FIRQ functions */
void dmd_rtt0 (void);
void dmd_rtt1 (void);
void dmd_rtt2 (void);

#ifdef CONFIG_DEBUG_ADJUSTMENTS
U8 firq_row_value;
#define dmd_request_firq() wpc_dmd_set_firq_row (firq_row_value)
#else
#define dmd_request_firq() wpc_dmd_set_firq_row (DMD_REFRESH_ROW)
#endif

U8 *dmd_phase_ptr;

U8 dmd_phase_table[] = {
	0, 1, 1, 0, 1, 1
};


void dmd_new_rtt (void)
{
	wpc_dmd_set_visible_page ( ((U8 *)&dmd_visible_pages) [*dmd_phase_ptr++]);
	if (dmd_phase_ptr >= dmd_phase_table + sizeof (dmd_phase_table))
		dmd_phase_ptr = dmd_phase_table;
	dmd_request_firq ();
}


/**
 * Initialize the DMD subsystem.
 */
void dmd_init (void)
{
	//dmd_rtt = dmd_rtt0;
	dmd_rtt = dmd_new_rtt;
	dmd_phase_ptr = dmd_phase_table;
	dmd_in_transition = FALSE;
	dmd_transition = NULL;

	/* If DMD_BLANK_PAGE_COUNT is defined, this says how
	 * many DMD pages should not be allocatable, but should be
	 * reserved and kept blank.  This is handy for some effects.
	 * Initialize those pages now.
	 */
#ifdef DMD_BLANK_PAGE_COUNT
	wpc_dmd_set_low_page (dmd_get_blank (0));
	dmd_clean_page_low ();
	wpc_dmd_set_high_page (dmd_get_blank (1));
	dmd_clean_page_high ();
#endif

	wpc_dmd_set_low_page (0);
	wpc_dmd_set_high_page (0);
	dmd_clean_page_low ();
	wpc_dmd_set_visible_page (dmd_dark_page = dmd_bright_page = 0);
	dmd_free_page = 2;

	/* Program the DMD controller to generate interrupts */
#ifdef CONFIG_DEBUG_ADJUSTMENTS
	firq_row_value = DMD_REFRESH_ROW;
#endif
	dmd_request_firq ();
}


/**
 * Handle the DMD whenever the DMD controller has finished updating
 * the display.  This function is invoked from the FIRQ handler.
 *
 * We support 4-color images through rapid page flipping.  One page
 * is shown 2/3 of the time; the other 1/3 of the time.  There are
 * 3 distinct phases; each is implemented by a separate function
 * to minimize control flow instructions.  The pointer 'dmd_rtt'
 * points to the next phase to be invoked.
 *
 * If a mono image needs to be drawn, then the bright/dark pages
 * contain the same value, and the flipping effectively doesn't
 * change anything.
 */

void dmd_rtt0 (void)
{
	wpc_dmd_set_visible_page (dmd_dark_page);
	dmd_request_firq ();
	/* IDEA: only the last byte of 'dmd_rtt' needs to be
	 * updated, as long as all three functions reside within
	 * the same 256-byte region, which could be verified at
	 * init time */
	dmd_rtt = dmd_rtt1;
}

void dmd_rtt1 (void)
{
	wpc_dmd_set_visible_page (dmd_bright_page);
	dmd_request_firq ();
	dmd_rtt = dmd_rtt2;
}

void dmd_rtt2 (void)
{
	wpc_dmd_set_visible_page (dmd_bright_page);
	dmd_request_firq ();
	dmd_rtt = dmd_rtt0;
}


/**
 * Allocate a new page of DMD memory.
 *
 * In order to support 4-color images, we actually reserve two pages
 * everytime an allocation is requested.  The page number returned
 * is always the lower numbered of the two pages.  The two pages
 * always have consecutive numbers.
 *
 * This function does not map the new pages into memory.
 */
static __attribute__((noinline)) dmd_pagenum_t dmd_alloc (void)
{
	dmd_pagenum_t page = dmd_free_page;
	dmd_free_page += 2;
	if (dmd_free_page >= DMD_ALLOC_PAGE_COUNT)
		dmd_free_page = 0;
	return page;
}


/**
 * Allocate and map a single page, for a mono image.
 *
 * Since the image is mono, we map the same page into both the low
 * and high pages.
 */
void dmd_alloc_low (void)
{
	wpc_dmd_set_low_page (dmd_alloc ());	
	wpc_dmd_set_high_page (wpc_dmd_get_low_page ());	
}


/** Allocate and map a single page, into the 'high' region. */
void dmd_alloc_high (void)
{
	wpc_dmd_set_high_page (dmd_alloc ());	
}


void dmd_map_low_high (dmd_pagenum_t page)
{
	wpc_dmd_set_low_page (page);
	wpc_dmd_set_high_page (page + 1);
}


/**
 * Allocate and map two different pages.
 */
void dmd_alloc_low_high (void)
{
	wpc_dmd_set_low_page (dmd_alloc ());	
	wpc_dmd_set_high_page (wpc_dmd_get_low_page () + 1);	
}


/**
 * Show a mono image.  Program the hardware to display the
 * page that is currently mapped into the low page.  The same
 * page is stored into the dark/bright page values.
 */
void dmd_show_low (void)
{
	if (unlikely (dmd_transition))
	{
		dmd_high_page = dmd_low_page;
		dmd_low_page = dmd_get_blank (0);
		dmd_do_transition ();
	}
	else
	{
		dmd_dark_page = dmd_get_blank (0);
		dmd_bright_page = dmd_low_page;
	}
}

void dmd_show_high (void)
{
	if (unlikely (dmd_transition))
	{
		dmd_low_page = dmd_get_blank (0);
		dmd_do_transition ();
	}
	else
	{
		dmd_dark_page = dmd_get_blank (0);
		dmd_bright_page = dmd_high_page;
	}
}


/** Flip the mapping between low and high pages. */
void dmd_flip_low_high (void)
{
	/* Note: tmp is made volatile here, to ensure that
	 * tmp is actually used in wpc_dmd_set_high() below.
	 * Without it, the compiler might optimize and try
	 * to read from the hardware register, which doesn't
	 * work.
	 */
	register dmd_pagenum_t tmp __areg__ = wpc_dmd_get_low_page ();
	wpc_dmd_set_low_page (wpc_dmd_get_high_page ());
	wpc_dmd_set_high_page (tmp);
	/* TODO - for the 6809, this can be optimized by using
	 * the EXG instruction to swap the values. */
}


/** Flip the currently visible page, alternating between the two 
currently mapped */
void dmd_show_other (void)
{
	dmd_visible_pages.pair ^= 0x0101;
}


/**
 * Show a 4-color image.
 */
void dmd_show2 (void)
{
	if (unlikely (dmd_transition))
		dmd_do_transition ();
	else
	{
		/* Changing both the active dark and bright page
		must be atomic in order to prevent visual artifacts
		from occurring.  Without such, portions of the
		old image and new image would be displayed simultaneously,
		but not in any meaningful way, due to the way images are
		divided into planes.
		   Earlier, this was done solely by disabling the FIRQ,
		which masks the DMD interrupt.  However, this did not
		keep IRQ disabled.  It was possible for IRQ to occur
		in between the two writes, which could take some time
		and would therefore keep the display from refreshing
		for much longer.  The effect is that only part of the image
		appears during that window.  So, we disabled IRQ as well
		here.
			However, now the operation can be performed atomically
		without any locking is at all, because the following
		results in a single store of 16-bits operation. */
		dmd_visible_pages = dmd_mapped_pages;
	}
}


/**
 * Clean an entire DMD page.  This is the C portable version
 * of the function; there is a special assembler version of this
 * for the 6809. */
#ifndef __m6809__
void dmd_clean_page (dmd_buffer_t dbuf)
{
	__blockclear16 (dbuf, DMD_PAGE_SIZE);
}
#endif /* __m6809__ */


void dmd_fill_page_low (void)
{
#ifdef __m6809__
	dmd_memset (dmd_low_buffer, 0xFF);
#else
	memset (dmd_low_buffer, 0xFF, DMD_PAGE_SIZE);
#endif
}


void dmd_clean_page_low (void)
{
	dmd_clean_page (dmd_low_buffer);
}


void dmd_clean_page_high (void)
{
	dmd_clean_page (dmd_high_buffer);
}


/** Invert all pixels in a given page.  This function is unrolled by hand for
 * better performance. */
void dmd_invert_page (dmd_buffer_t dbuf)
{
	register int16_t count = DMD_PAGE_SIZE / (2 * 4);
	register U16 *dbuf16 = (U16 *)dbuf;
	while (--count >= 0)
	{
		*dbuf16 = ~*dbuf16;
		dbuf16++;
		*dbuf16 = ~*dbuf16;
		dbuf16++;
		*dbuf16 = ~*dbuf16;
		dbuf16++;
		*dbuf16 = ~*dbuf16;
		dbuf16++;
	}
}


void dmd_copy_page (dmd_buffer_t dst, const dmd_buffer_t src)
{
#ifdef __m6809__
	dmd_copy_asm (dst, src);
#else
	__blockcopy16 (dst, src, DMD_PAGE_SIZE);
#endif
}


void dmd_copy_low_to_high (void)
{
	dmd_copy_page (dmd_high_buffer, dmd_low_buffer);
}


void dmd_alloc_low_clean (void)
{
	dmd_alloc_low ();
	dmd_clean_page (dmd_low_buffer);
}


void dmd_alloc_pair_clean (void)
{
	dmd_alloc_low_high ();
	dmd_clean_page (dmd_low_buffer);
	dmd_clean_page (dmd_high_buffer);
}


/** Draw a thin border 2 pixels wide around the given display page. */
void dmd_draw_border (U8 *dbuf)
{
	const dmd_buffer_t dbuf_bot = (dmd_buffer_t)((char *)dbuf + 480);
	register U16 *dbuf16 = (U16 *)dbuf;
	register U16 *dbuf16_bot = (U16 *)dbuf_bot;
	U8 i;

	for (i=0; i < 16; i++)
		*dbuf16_bot++ = *dbuf16++ = 0xFFFFUL;
	dbuf += 32;
	for (i=0; i < 28; i++)
	{
		dbuf[0] = 0x03;
		dbuf[15] = 0xC0;
		dbuf += 16;
	}
}

/**
 * Draw a horizontal line at the specified y-coordinate.
 */
void dmd_draw_horiz_line (U16 *dbuf, U8 y)
{
	dbuf += y * (16 / 2);

	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
}


/*
 * Helper function used to do a DMD transition.
 * This contains common logic that needs to happen several times during
 * a transition.
 */
static inline void dmd_do_transition_cycle (U8 old_page, U8 new_page)
{
	/* On entry, the composite buffer must be mapped into the 
	 * high page. */
 
	/* Map the old image in low memory. */
	wpc_dmd_set_low_page (old_page);

	/* Initialize the composite from the old image. */
	dmd_transition->composite_old ();

	/* Now remap the new image into low memory */
	wpc_dmd_set_low_page (new_page);

	/* Update the composite using the new image data.
	 * This function should set dmd_in_transition to FALSE when
	 * the transition is done. */
	dmd_transition->composite_new ();
}


/**
 * Do a DMD transition.
 *
 * Transitions are complicated because the old/new images may have
 * different color depths (mono or 4-color).  Also, we can only map
 * two pages at a time, but there may be up to 4 different pages
 * involved.
 *
 * First, we check to see if both old and new images are mono.  If so,
 * then the transition can be optimized slightly.
 *
 * For debugging transitions, define STEP_TRANSITION.  The transition
 * will take place one frame at a time; use the launch button to
 * step through each frame.
 */
void dmd_do_transition (void)
{
	const U8 new_dark_page = dmd_low_page;
	const U8 new_bright_page = dmd_high_page;

	dmd_trans_data_ptr = NULL;
	dmd_trans_data_ptr2 = NULL;

	page_push (TRANS_PAGE);
	
	if (dmd_transition->composite_init)
	{
		(*dmd_transition->composite_init) ();
		dmd_trans_data_ptr2 = dmd_trans_data_ptr;
	}

	while (dmd_in_transition)
	{
#if defined(STEP_TRANSITION) && defined(MACHINE_LAUNCH_SWITCH)
		while (!switch_poll (MACHINE_LAUNCH_SWITCH))
			task_sleep (TIME_33MS);
		task_sleep (TIME_100MS);
		while (switch_poll (MACHINE_LAUNCH_SWITCH))
			task_sleep (TIME_33MS);
#else
		task_sleep (dmd_transition->delay);
#endif

		do {
			dmd_composite_page = dmd_alloc ();
		} while ((dmd_composite_page == (new_dark_page & ~1)) ||
			(dmd_composite_page == (new_bright_page & ~1)));

		/* Handle the transition of the dark page first.
		 * Use the lower composite pair page. */
		wpc_dmd_set_high_page (dmd_composite_page);
		dmd_do_transition_cycle (dmd_dark_page, new_dark_page);

		/* Handle the transition of the bright page.
		 * Use the upper composite pair page (+1). */
		{
			U8 *tmp_trans_data_ptr;

			tmp_trans_data_ptr = dmd_trans_data_ptr;
			dmd_trans_data_ptr = dmd_trans_data_ptr2;

			wpc_dmd_set_high_page (dmd_composite_page+1);
			dmd_do_transition_cycle (dmd_bright_page, new_bright_page);

			dmd_trans_data_ptr2 = dmd_trans_data_ptr;
			dmd_trans_data_ptr = tmp_trans_data_ptr;
		}

		/* Make the composite pages visible */
		dmd_dark_page = dmd_composite_page;
		dmd_bright_page = dmd_composite_page+1;
	}

	page_pop ();
	dmd_transition = NULL;
}


/**
 * Apply the contents of a lookaside frame (2 planes)
 * onto the current frame mapped in low/high buffers.
 * The apply operation is performed twice, once per bitplane.
 *
 * An apply function could be an AND or OR operation.
 * Prior to the call, the low page points to the destination
 * buffer and the high page to the source/lookaside buffer.
 */
void dmd_apply_lookaside2 (U8 num, void (*apply)(void))
{
	const U8 low = wpc_dmd_get_low_page ();
	const U8 high = wpc_dmd_get_high_page ();
	const U8 apply_low = dmd_get_lookaside (num);
	const U8 apply_high = apply_low+1;

	/* Note: this currently takes about 18000 cycles, or 9ms.  Each
	 * page AND/OR operation is the majority of the time, each about
	 * 9000 cycles or 4.5ms */
	wpc_dmd_set_high_page (apply_low);
	apply ();
	wpc_dmd_set_low_page (high);
	wpc_dmd_set_high_page (apply_high);
	apply ();
	wpc_dmd_set_low_page (low);
	wpc_dmd_set_high_page (high);
}


/**
 * Schedule a transition.
 *
 * Normally, when dmd_show_low or dmd_show2 is invoked, the new pages
 * take effect immediately (i.e. using a "cut" transition).
 *
 * Calling this function prior to those functions causes them to
 * behave slightly differently.  It causes a transition between the
 * old and new images to be occur.  The show functions use
 * dmd_in_transition to determine whether or not a transition needs
 * to be run.
 *
 * trans is a pointer to a transition object, which identifies the type
 * of transition and some parameters, like its speed.
 */
void dmd_sched_transition (dmd_transition_t *trans)
{
	dmd_transition = trans;
	dmd_in_transition = TRUE;
}


/**
 * Cancel a scheduled transition.  This is needed in case an effect
 * has scheduled a transition, but is stopped or replaced by a higher
 * priority effect.
 */
void dmd_reset_transition (void)
{
	dmd_in_transition = FALSE;
	dmd_transition = NULL;
}

