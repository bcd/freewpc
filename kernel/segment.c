/*
 * Copyright 2005-2011 by Brian Dominy <brian@oddchange.com>
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
 * \brief Manage the alphanumeric segment displays.
 *
 */

#include <freewpc.h>

bool seg_in_transition;

seg_transition_t *seg_transition;

U8 *seg_trans_data_ptr;

static void seg_do_transition (void);

/**
 * The segment lookup table.
 * Each entry here gives the segments that should be lit to form a specific
 * character.
 */
const segbits_t seg_table[] = {
	/* Characters in the low end, which are normally not displayable,
	have entries here for miscellaneous graphics characters. */
	[SEGCHAR_ALL] = 0xFFFF & SEG_COMMA & SEG_PERIOD,
	[SEGCHAR_HORIZ] = SEG_TOP+SEG_MID+SEG_BOT,
	[SEGCHAR_VERT] = SEG_LEFT+SEG_VERT+SEG_RIGHT,

	[SEGCHAR_STROBE] = SEG_STROBE0,
		SEG_STROBE1,
		SEG_STROBE2,
		SEG_STROBE3,

	/* The beginning of the ASCII printable characters */
	[' '] = 0,
	['%'] = SEG_UPR_LEFT+SEG_UR_DIAG+SEG_LL_DIAG+SEG_LWR_RIGHT,
	['('] = SEG_UR_DIAG+SEG_LR_DIAG,
	[')'] = SEG_UL_DIAG+SEG_LL_DIAG,
	['*'] = SEG_UL_DIAG+SEG_UR_DIAG+SEG_LL_DIAG+SEG_LR_DIAG+SEG_MID+SEG_VERT,
	['+'] = SEG_VERT+SEG_MID,
	['-'] = SEG_MID,
	['/'] = SEG_UR_DIAG+SEG_LL_DIAG,
	['='] = SEG_MID+SEG_BOT,
	['\\'] = SEG_UL_DIAG+SEG_LR_DIAG,
   ['$'] = SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT+SEG_VERT,
   ['0'] = SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT,
   ['1'] = SEG_RIGHT,
   ['2'] = SEG_TOP+SEG_UPR_RIGHT+SEG_MID+SEG_LWR_LEFT+SEG_BOT,
   ['3'] = SEG_TOP+SEG_MID+SEG_BOT+SEG_RIGHT,
   ['4'] = SEG_UPR_LEFT+SEG_MID+SEG_RIGHT,
   ['5'] = SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT,
   ['6'] = SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID,
   ['7'] = SEG_TOP+SEG_RIGHT,
   ['8'] = SEG_TOP+SEG_MID+SEG_BOT+SEG_LEFT+SEG_RIGHT,
   ['9'] = SEG_TOP+SEG_MID+SEG_BOT+SEG_UPR_LEFT+SEG_RIGHT,
	['.'] = SEG_PERIOD,
	[','] = SEG_PERIOD+SEG_COMMA,
	[':'] = SEG_PERIOD,
	['<'] = SEG_UR_DIAG+SEG_LR_DIAG,
	['>'] = SEG_UL_DIAG+SEG_LL_DIAG,
   ['A'] = SEG_LEFT+SEG_TOP+SEG_MID+SEG_RIGHT,
   ['B'] = SEG_TOP+SEG_BOT+SEG_MID_RIGHT+SEG_VERT+SEG_RIGHT,
   ['C'] = SEG_LEFT+SEG_TOP+SEG_BOT,
   ['D'] = SEG_TOP+SEG_BOT+SEG_VERT+SEG_RIGHT,
   ['E'] = SEG_LEFT+SEG_TOP+SEG_MID+SEG_BOT,
   ['F'] = SEG_LEFT+SEG_TOP+SEG_MID,
   ['G'] = SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID_RIGHT,
	['H'] = SEG_LEFT+SEG_RIGHT+SEG_MID,
	['I'] = SEG_TOP+SEG_VERT+SEG_BOT,
	['J'] = SEG_RIGHT+SEG_BOT+SEG_LWR_LEFT,
	['K'] = SEG_LEFT+SEG_UR_DIAG+SEG_LR_DIAG+SEG_MID_LEFT,
	['L'] = SEG_LEFT+SEG_BOT,
	['M'] = SEG_LEFT+SEG_UL_DIAG+SEG_UR_DIAG+SEG_RIGHT,
	['N'] = SEG_LEFT+SEG_UL_DIAG+SEG_LR_DIAG+SEG_RIGHT,
   ['O'] = SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT,
	['P'] = SEG_LEFT+SEG_TOP+SEG_UPR_RIGHT+SEG_MID,
   ['Q'] = SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT+SEG_LR_DIAG,
	['R'] = SEG_LEFT+SEG_TOP+SEG_UPR_RIGHT+SEG_MID+SEG_LR_DIAG,
   ['S'] = SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT,
	['T'] = SEG_TOP+SEG_VERT,
	['U'] = SEG_LEFT+SEG_BOT+SEG_RIGHT,
	['V'] = SEG_LEFT+SEG_LL_DIAG+SEG_UR_DIAG,
	['W'] = SEG_LEFT+SEG_BOT+SEG_VERT_BOT+SEG_RIGHT,
	['X'] = SEG_UL_DIAG+SEG_UR_DIAG+SEG_LL_DIAG+SEG_LR_DIAG,
	['Y'] = SEG_UL_DIAG+SEG_UR_DIAG+SEG_VERT_BOT,
   ['Z'] = SEG_TOP+SEG_UR_DIAG+SEG_LL_DIAG+SEG_BOT,
	['_'] = SEG_BOT,
	['h'] = SEG_LEFT+SEG_MID+SEG_LWR_RIGHT,
	['m'] = SEG_LWR_LEFT+SEG_VERT_BOT+SEG_LWR_RIGHT+SEG_MID,
	[127] = SEG_MID,
};

/** An array of display page memory */
seg_page_t seg_pages[SEG_ALLOC_PAGES+SEG_FIXED_PAGES];

/** The pointer to the current visible display page */
__fastram__ seg_page_t *seg_visible_page;

/** The pointer to the current writable display page */
__fastram__ seg_page_t *seg_writable_page;

seg_page_t *seg_secondary;

/** The page number for the next page that will be given out */
U8 seg_alloc_pageid;

U8 seg_col;

/**
 * Handle the realtime update of the segment displays.
 *
 * This function is invoked every 1ms to latch a value
 * for one column -- 2 characters, one on top, one on bottom.
 * It thus takes 16ms to strobe the entire display.
 */
void seg_rtt (void)
{
#ifndef __m6809__
	register segbits_t *valp;
#endif

	seg_col = (seg_col + 1) % SEG_SECTION_SIZE;
	writeb (WPC_ALPHA_POS, seg_col);

#ifdef __m6809__
	asm ("ldx\t*_seg_visible_page");
	asm ("abx");
	asm ("abx");
	asm ("ldd\t,x");
	asm ("std\t16364");
	asm ("ldd\t32,x");
	asm ("std\t16366");
#else
	valp = &(*seg_visible_page)[0][seg_col];
	writew (WPC_ALPHA_ROW1, *valp);
	writew (WPC_ALPHA_ROW2, *(valp + SEG_SECTION_SIZE));
#endif
}


/**
 * Allocate a new display page.
 */
void seg_alloc (void)
{
	seg_writable_page = seg_pages + seg_alloc_pageid;
	seg_secondary = seg_writable_page + 1;
	seg_alloc_pageid += 2;
	seg_alloc_pageid &= (SEG_ALLOC_PAGES-1);
}


/**
 * Map a specific display page so that all write commands
 * apply to it.
 */
void seg_map (U8 page)
{
	seg_writable_page = seg_pages + page;
}


/**
 * Flip the current and secondary display pages.
 */
void seg_flip_low_high (void)
{
	seg_page_t *tmp = seg_writable_page;
	seg_writable_page = seg_secondary;
	seg_secondary = tmp;
}


static void seg_show_from (seg_page_t *page_ptr)
{
	if (unlikely (seg_transition))
		seg_do_transition ();
	seg_visible_page = page_ptr;
}


/**
 * Show the current writable page.
 */
void seg_show (void)
{
	seg_show_from (seg_writable_page);
}


/**
 * Show a specific page number.
 */
void seg_show_page (U8 page)
{
	seg_show_from (seg_pages + page);
}


/**
 * Swap the current visible page with the secondary page.
 */
void seg_show_other (void)
{
	if (seg_visible_page == seg_writable_page)
		seg_show_from (seg_writable_page + 1);
	else
		seg_show_from (seg_writable_page);
}


/** Called from a deff when it wants to toggle between two images.
 * COUNT is the number of times to toggle.
 * DELAY is how long to wait between each change. */
void deff_swap_low_high (S8 count, task_ticks_t delay)
{
	dmd_show_low ();
	while (--count >= 0)
	{
		dmd_show_other ();
		task_sleep (delay);
	}
}


/**
 * Copy the contents of the current writable page to the
 * secondary one.
 */
void seg_copy_low_to_high (void)
{
	memcpy (seg_writable_page+1, seg_writable_page, sizeof (seg_page_t));
}


seg_page_t *seg_get_page_pointer (U8 page)
{
	return seg_pages + page;
}


/**
 * Translate an ASCII character into segments.
 *
 * If an invalid, high-end character is given, display it as a
 * dash to avoid table lookup.
 */
static segbits_t seg_translate_char (char c)
{
	if (likely (c < 127))
		return seg_table[(U8)c];
	else
		return SEG_MID;
}


/**
 * Return true if the display slot pointer is valid and can be
 * written.
 */
static bool seg_addr_valid (void *sa)
{
	return ((sa >= (void *)seg_pages) &&
		(sa < (void *)seg_pages + sizeof (seg_pages)));
}


/**
 * Enable one or more segments on a particular display slot.
 */
static void seg_enable_segment (segbits_t *sa, segbits_t seg)
{
	if (seg_addr_valid (sa))
		*sa |= seg;
}


/**
 * Write a single character to one alphanumeric display slot.
 *
 * SA points to the display memory.
 * C is the character to be displayed there.
 */
segbits_t *seg_write_char (segbits_t *sa, char c)
{
	if (c == '\0')
	{
		return sa;
	}
	else if ((c == '.' || c == ':') && (!(sa[-1] & SEG_PERIOD)))
	{
		seg_enable_segment (sa-1, SEG_PERIOD);
		return sa;
	}
	else if (c == ',')
	{
		seg_enable_segment (sa-1, SEG_COMMA+SEG_PERIOD);
		return sa;
	}
	else if (seg_addr_valid (sa))
	{
		*sa = seg_translate_char (c);
		return sa+1;
	}
	else
	{
		return sa;
	}
}


/**
 * Write a string to the alphanumeric display, starting at the
 * given row and column.
 */
void seg_write_string (U8 row, U8 col, const char *s)
{
	segbits_t *addr;

	/* Sanity checking */
	if (row >= SEG_SECTIONS)
		return;
	if (col >= SEG_SECTION_SIZE)
		return;

	/* Get a pointer to the display memory */
	addr = &(*seg_writable_page)[row][col];

	/* For each character in the string, write it and advance the
	cursor. */
	while (*s != '\0')
		addr = seg_write_char (addr, *s++);
}


/**
 * Calculate the length of a string to be written to the
 * display.  This is used to determine positioning for
 * centering and right justification.
 */
U8 seg_strlen (const char *s)
{
	U8 n = 0;
	while (*s++ != '\0')
	{
		if ((*s == '.') && (s[-1] != '.'))
			continue;
		if (*s == ',')
			continue;
		n++;
	}
	return n;
}


void seg_write_row_center (U8 row, const char *s)
{
	seg_write_string (row, 8 - (seg_strlen (s) / 2), s);
}

void seg_write_row_right (U8 row, const char *s)
{
	seg_write_string (row, 16 - seg_strlen (s), s);
}

/**
 * Erase the current page.
 */
void seg_erase (void)
{
	segbits_t *addr = &(*seg_writable_page)[0][0];
	memset (addr, 0, sizeof(segbits_t) * SEG_SECTIONS * SEG_SECTION_SIZE);
}


/**
 * Fill the current page with the same character.
 */
void seg_fill (segbits_t segs)
{
	segbits_t *addr = &(*seg_writable_page)[0][0];
	memset (addr, segs, sizeof(segbits_t) * SEG_SECTIONS * SEG_SECTION_SIZE);
}


/**
 * Allocate a clean page for drawing.
 */
void seg_alloc_clean (void)
{
	seg_alloc ();
	seg_erase ();
}


/**
 * Execute a segment-style transition effect.
 */
static void seg_do_transition (void)
{
	seg_page_t *seg_final_page;
	seg_page_t *tmp;
	U8 iteration;

	/* Save pointer to the final page -- that which will ultimately
	be displayed.  This is the source page for all updates during
	the transition. */
	seg_final_page = seg_writable_page;

	/* Allocate a new page, kept in seg_writable_page, and initialize
	it with the current visible page.  This is the destination page
	for all updates. */
	seg_alloc ();
	memcpy (seg_writable_page, seg_visible_page, sizeof (seg_page_t));

	/* Invoke the constructor */
	if (seg_transition->init)
		seg_transition->init ();

	iteration = 0;
	while (seg_in_transition && iteration < 255)
	{
		/* Delay */
		task_sleep (seg_transition->delay);

		/* Do a partial update */
		seg_in_transition = seg_transition->update (seg_final_page, iteration);
		iteration++;

		/* Make the current destination page visible, and allocate a
		new one for the next iteration.  This can be done by a swap
		of the old page and new page pointers */
		tmp = seg_visible_page;
		seg_visible_page = seg_writable_page;
		seg_writable_page = tmp;
	}
	seg_transition = NULL;
}


/**
 * Schedule a segment display transition.
 */
void seg_sched_transition (seg_transition_t *trans)
{
	seg_transition = trans;
	seg_in_transition = TRUE;
}


/**
 * Clear any segment display transition that has been scheduled,
 * but not executed yet.
 */
void seg_reset_transition (void)
{
	seg_transition = NULL;
	seg_in_transition = FALSE;
}


/**
 * Initialize the segment displays.
 */
void seg_init (void)
{
	memset (seg_pages, 0, sizeof (seg_pages));
	seg_show_page (0);
	seg_alloc_pageid = 0;
	seg_transition = NULL;
}


/************************************************************/

/* Begin examples of transition effects.  TODO : move these
out of the system bank of ROM. */


/**
 * Copy one character from the new display page onto the
 * existing page.
 */
static void seg_fade_in (seg_page_t *src, U8 row, U8 col)
{
	(*seg_writable_page)[row][col] = (*src)[row][col];
}


/**
 * Copy one column of characters
 */
static void seg_fade_in_col (seg_page_t *src, U8 col)
{
	seg_fade_in (src, 0, col);
	seg_fade_in (src, 1, col);
}

static void seg_shift_left (segbits_t *data, U8 count)
{
	while (count > 1)
	{
		data[0] = data[1];
		data++;
		count--;
	}
	data[0] = 0;
}


static void seg_shift_right (segbits_t *data, U8 count)
{
	data += count - 1;
	while (count > 1)
	{
		data[0] = data[-1];
		data--;
		count--;
	}
	data[0] = 0;
}


bool seg_trans_center_out_update (seg_page_t *src, U8 iteration)
{
	if (iteration < (SEG_SECTION_SIZE/2))
	{
		/* Copy from center to outside edges */
		seg_fade_in_col (src, (SEG_SECTION_SIZE/2)-iteration);
		seg_fade_in_col (src, (1+SEG_SECTION_SIZE/2)+iteration);
		return TRUE;
	}
	else
		return FALSE;
}

bool seg_trans_ltr_update (seg_page_t *src, U8 iteration)
{
	if (iteration < SEG_SECTION_SIZE)
	{
		seg_fade_in_col (src, iteration);
		return TRUE;
	}
	else
		return FALSE;
}

bool seg_trans_rtl_update (seg_page_t *src, U8 iteration)
{
	if (iteration < SEG_SECTION_SIZE)
	{
		seg_fade_in_col (src, (SEG_SECTION_SIZE-1)-iteration);
		return TRUE;
	}
	else
		return FALSE;
}

bool seg_trans_push_left_update (seg_page_t *src, U8 iteration)
{
	if (iteration < SEG_SECTION_SIZE)
	{
		segbits_t *dst1 = &(*seg_writable_page)[0][0];
		seg_shift_left (dst1, SEG_SECTION_SIZE);
		seg_shift_left (dst1+SEG_SECTION_SIZE, SEG_SECTION_SIZE);
		(*seg_writable_page)[0][15] = (*src)[0][iteration];
		(*seg_writable_page)[1][15] = (*src)[1][iteration];
		return TRUE;
	}
	else
		return FALSE;
}

bool seg_trans_push_right_update (seg_page_t *src, U8 iteration)
{
	if (iteration < SEG_SECTION_SIZE)
	{
		segbits_t *dst1 = &(*seg_writable_page)[0][0];
		seg_shift_right (dst1, SEG_SECTION_SIZE);
		seg_shift_right (dst1+SEG_SECTION_SIZE, SEG_SECTION_SIZE);
		(*seg_writable_page)[0][0] = (*src)[0][15-iteration];
		(*seg_writable_page)[1][0] = (*src)[1][15-iteration];
		return TRUE;
	}
	else
		return FALSE;
}

segbits_t seg_fade_table[] = {
	SEG_UPR_LEFT,
	SEG_UL_DIAG,
	SEG_TOP,
	SEG_VERT_TOP,
	SEG_UR_DIAG,
	SEG_UPR_RIGHT,
	SEG_MID_RIGHT,
	SEG_LWR_RIGHT,
	SEG_LR_DIAG,
	SEG_VERT_BOT,
	SEG_BOT,
	SEG_LL_DIAG,
	SEG_LWR_LEFT,
	SEG_MID_LEFT,
	SEG_COMMA+SEG_PERIOD,
};

bool seg_trans_fade_update (seg_page_t *src, U8 iteration)
{
	if (iteration < sizeof (seg_fade_table) / sizeof (segbits_t))
	{
		segbits_t mask = seg_fade_table[iteration];
		segbits_t *dst1 = &(*seg_writable_page)[0][0];
		segbits_t *src1 = &(*src)[0][0];
		U8 count;
		for (count = 0;
			count < SEG_SECTIONS*SEG_SECTION_SIZE;
			count++, src1++, dst1++)
		{
			*dst1 = (*dst1 & ~mask) | (*src1 & mask);
		}
		return TRUE;
	}
	else
		return FALSE;
}

seg_transition_t seg_trans_center_out =
{
	.init = NULL,
	.update = seg_trans_center_out_update,
	.delay = TIME_100MS,
};

seg_transition_t seg_trans_ltr =
{
	.init = NULL,
	.update = seg_trans_ltr_update,
	.delay = TIME_50MS,
};

seg_transition_t seg_trans_rtl =
{
	.init = NULL,
	.update = seg_trans_rtl_update,
	.delay = TIME_50MS,
};

seg_transition_t seg_trans_fast_center_out =
{
	.init = NULL,
	.update = seg_trans_center_out_update,
	.delay = TIME_50MS,
};

seg_transition_t seg_trans_fade =
{
	.init = NULL,
	.update = seg_trans_fade_update,
	.delay = TIME_33MS,
};

seg_transition_t seg_trans_push_left =
{
	.init = NULL,
	.update = seg_trans_push_left_update,
	.delay = TIME_50MS,
};

seg_transition_t seg_trans_push_right = {
	.init = NULL,
	.update = seg_trans_push_right_update,
	.delay = TIME_50MS,
};

