/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>

/**
 * \file
 * \brief Handle DMD transitions: gradual changes between one display effect and another.
 *
 * Conceptually, there are 3 buffers involved in a transition: the
 * "old" page that is currently visible; the "new" page that will eventually
 * be drawn completely, and a temporary buffer to hold the intermediate
 * frames.  At the time the transition starts, the old and new pages
 * are already rendered; the transition's only job is to render the 
 * intermediate frames.
 *
 * During each "step" of the transition, the intermediate frame is
 * constructed as a function of both the old and new frames.  Two functions
 * are called to do this, "old" and "new".  The high display page
 * is always mapped to the intermediate frame page, while the low
 * display page is mapped to the old/new page (depending on whether
 * the old/new function is being called).
 *
 * Also, the transition structure can define the delay between frames.
 * Multiple transitions can be defined with the same compositing
 * functions but different delays.
 *
 * arg is a pointer/integer union that defines an additional free
 * parameter.  If needed, the 'init' method should initialize
 * dmd_trans_data_ptr with it; the pointer variable is kept
 * between each step of the transition.
 *
 * There are inherently 3 classes of transitions:
 * 1. Old image remains static, new image overlays it.  For this class,
 * the "old" transition function is dmd_copy_low_to_high.  The "new"
 * function copies in the new data one section at a time.
 *
 * 2. Old image moves away, revealing static new image underneath it.
 *
 * 3. Both old image and new image move during the transition (e.g.
 one image "pushes" another one).
 */




/* The scroll_up transition.
 * Existing image data is pushed up off the screen, with new data
 * entered at the bottom.
 *
 * Strategy: the old function will copy the previous page into the
 * composite buffer, but shifting it up by N rows.  The first N
 * rows of the old image are lost.  The last N rows of the
 * composite buffer are uninitialized.
 * The new function will fill in the last N rows with data from
 * the new image.  The first time it is called, it begins from
 * the new image base (dmd_low_buffer), but thereafter begins
 * from higher offsets, using dmd_trans_data_ptr to remember
 * where it left off.  When the last row has been shifted in,
 * the new handler will clear dmd_transition, signalling the
 * end of the transition.
 */

void trans_scroll_up_init (void)
{
	dmd_trans_data_ptr = dmd_low_buffer;
}

void trans_scroll_up_old (void)
{
	/* dmd_low_buffer = old image data */
	/* dmd_high_buffer = composite buffer */
	register U16 arg = dmd_transition->arg.u16;
	
	/* Use __blockcopy16 to do a fast memcpy when size is
	 * guaranteed to be a multiple of 16 bytes and nonzero. */
	__blockcopy16 (dmd_high_buffer, 
		dmd_low_buffer + arg, DMD_PAGE_SIZE - arg);
}

void trans_scroll_up_new (void)
{
	/* dmd_low_buffer = new image data */
	/* dmd_high_buffer = composite buffer */
	register U16 arg = dmd_transition->arg.u16;

	__blockcopy16 (dmd_high_buffer + DMD_PAGE_SIZE - arg,
		dmd_trans_data_ptr, arg);

	dmd_trans_data_ptr += arg;
	if (dmd_trans_data_ptr == (dmd_low_buffer + DMD_PAGE_SIZE))
		dmd_in_transition = FALSE;
}


dmd_transition_t trans_scroll_up = {
	.composite_init = trans_scroll_up_init,
	.composite_old = trans_scroll_up_old,
	.composite_new = trans_scroll_up_new,
	.delay = TIME_33MS,
	.arg = { .u16 = 4 * 16 }, /* 4 lines at a time */
	.count = 8,
};

dmd_transition_t trans_scroll_up_avg = {
	.composite_init = trans_scroll_up_init,
	.composite_old = trans_scroll_up_old,
	.composite_new = trans_scroll_up_new,
	.delay = TIME_66MS,
	.arg = { .u16 = 2 * 16 }, /* 2 lines at a time */
	.count = 16,
};

dmd_transition_t trans_scroll_up_slow = {
	.composite_init = trans_scroll_up_init,
	.composite_old = trans_scroll_up_old,
	.composite_new = trans_scroll_up_new,
	.delay = TIME_100MS,
	.arg = { .u16 = 1 * 16 }, /* 1 line at a time */
	.count = 32,
};

/*********************************************************************/

void trans_scroll_down_init (void)
{
	register U16 arg = dmd_transition->arg.u16;
	dmd_trans_data_ptr = dmd_low_buffer + DMD_PAGE_SIZE - arg;
}

void trans_scroll_down_old (void)
{
	register U16 arg = dmd_transition->arg.u16;
	__blockcopy16 (dmd_high_buffer + arg, 
		dmd_low_buffer, DMD_PAGE_SIZE - arg);
}

void trans_scroll_down_new (void)
{
	register U16 arg = dmd_transition->arg.u16;
	__blockcopy16 (dmd_high_buffer, dmd_trans_data_ptr, arg);
	dmd_trans_data_ptr -= arg;
	if (dmd_trans_data_ptr < dmd_low_buffer)
		dmd_in_transition = FALSE;
}

dmd_transition_t trans_scroll_down = {
	.composite_init = trans_scroll_down_init,
	.composite_old = trans_scroll_down_old,
	.composite_new = trans_scroll_down_new,
	.delay = TIME_33MS,
	.arg = { .u16 = 4 * 16 },
	.count = 8,
};

/*********************************************************************/

void trans_scroll_left_init (void)
{
	dmd_trans_data_ptr = dmd_low_buffer;
}

void trans_scroll_left_old (void)
{
	__blockcopy16 (dmd_high_buffer, dmd_low_buffer + 1, DMD_PAGE_SIZE);
}

void trans_scroll_left_new (void)
{
	U16 i;


	register U8 *src = dmd_trans_data_ptr;
	register U8 *dst = dmd_high_buffer + 15;
	for (i=0; i < 32L * 16; i += 64)
	{
		dst[i] = src[i];
		dst[i+16] = src[i+16];
		dst[i+32] = src[i+32];
		dst[i+48] = src[i+48];
	}

	dmd_trans_data_ptr++;
	if (dmd_trans_data_ptr == dmd_low_buffer + 16)
		dmd_in_transition = FALSE;
}


dmd_transition_t trans_scroll_left = {
	.composite_init = trans_scroll_left_init,
	.composite_old = trans_scroll_left_old,
	.composite_new = trans_scroll_left_new,
	.delay = TIME_33MS,
	.arg = { .u16 = 0 },
	.count = 16,
};


/*********************************************************************/

void trans_scroll_right_init (void)
{
	dmd_trans_data_ptr = dmd_low_buffer+15;
}

void trans_scroll_right_old (void)
{
	__blockcopy16 (dmd_high_buffer+1, dmd_low_buffer, DMD_PAGE_SIZE);
}

void trans_scroll_right_new (void)
{
	U16 i;

	register U8 *src = dmd_trans_data_ptr;
	register U8 *dst = dmd_high_buffer;
	for (i=0; i < 32L * 16; i += 64)
	{
		dst[i] = src[i];
		dst[i+16] = src[i+16];
		dst[i+32] = src[i+32];
		dst[i+48] = src[i+48];
	}

	if (dmd_trans_data_ptr == dmd_low_buffer)
		dmd_in_transition = FALSE;
	else
		dmd_trans_data_ptr--;
}


dmd_transition_t trans_scroll_right = {
	.composite_init = trans_scroll_right_init,
	.composite_old = trans_scroll_right_old,
	.composite_new = trans_scroll_right_new,
	.delay = TIME_33MS,
	.arg = { .u16 = 0 },
	.count = 16,
};

/*********************************************************************/

static U16 sequential_boxfade_offset_table[] = {
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
	256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,
	384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,
};

static U16 random_boxfade_offset_table[] = {
	130, 395, 13, 396, 8, 390, 10, 264, 12,
	135, 15, 265, 384, 2, 131, 393, 5, 397,
	141, 6, 386, 1, 387, 138, 394, 258, 133,
	398, 134, 399, 257, 3, 389, 267, 391, 139,
	9, 140, 270, 262, 11, 142, 269, 143, 4, 136,
	263, 132, 256, 392, 268, 128, 0, 261, 388, 137,
	7, 129, 14, 259, 260, 385, 266, 271,
};

void trans_fade_init (void)
{
	dmd_trans_data_ptr = (U8 *)dmd_transition->arg.ptr;
}

void trans_fade_new (void)
{
	U16 offset;

	offset = *(U16 *)dmd_trans_data_ptr;

	dmd_high_buffer[offset] = dmd_low_buffer[offset];
	dmd_high_buffer[16 + offset] = dmd_low_buffer[16 + offset];
	dmd_high_buffer[32 + offset] = dmd_low_buffer[32 + offset];
	dmd_high_buffer[48 + offset] = dmd_low_buffer[48 + offset];
	dmd_high_buffer[64 + offset] = dmd_low_buffer[64 + offset];
	dmd_high_buffer[80 + offset] = dmd_low_buffer[80 + offset];
	dmd_high_buffer[96 + offset] = dmd_low_buffer[96 + offset];
	dmd_high_buffer[112 + offset] = dmd_low_buffer[112 + offset];

	dmd_trans_data_ptr += sizeof (U16);
	if (dmd_trans_data_ptr == 
		((U8 *)dmd_transition->arg.ptr) + 64 * sizeof (U16))
		dmd_in_transition = FALSE;
}

dmd_transition_t trans_sequential_boxfade = {
	.composite_init = trans_fade_init,
	.composite_old = dmd_copy_low_to_high,
	.composite_new = trans_fade_new,
	.delay = TIME_16MS,
	.arg = { .ptr = sequential_boxfade_offset_table },
	.count = 64,
};

dmd_transition_t trans_random_boxfade = {
	.composite_init = trans_fade_init,
	.composite_old = dmd_copy_low_to_high,
	.composite_new = trans_fade_new,
	.delay = TIME_16MS,
	.arg = { .ptr = random_boxfade_offset_table },
	.count = 64,
};


/*********************************************************************/

void trans_vstripe_init (void)
{
	dmd_trans_data_ptr = (U8 *)dmd_transition->arg.ptr;
}

void trans_vstripe_new (void)
{
	U8 col;
	U8 mask;
	U8 *src, *dst;
	U8 i;

	col = dmd_trans_data_ptr[0];
	mask = dmd_trans_data_ptr[1];
	dst =	dmd_high_buffer + col;
	src = dmd_low_buffer + col;

	for (i=0; i < 8; i++)
	{
		dst[0 * DMD_BYTE_WIDTH] &= ~mask;
		dst[0 * DMD_BYTE_WIDTH] |= src[0 * DMD_BYTE_WIDTH] & mask;

		dst[1 * DMD_BYTE_WIDTH] &= ~mask;
		dst[1 * DMD_BYTE_WIDTH] |= src[1 * DMD_BYTE_WIDTH] & mask;

		dst[2 * DMD_BYTE_WIDTH] &= ~mask;
		dst[2 * DMD_BYTE_WIDTH] |= src[2 * DMD_BYTE_WIDTH] & mask;

		dst[3 * DMD_BYTE_WIDTH] &= ~mask;
		dst[3 * DMD_BYTE_WIDTH] |= src[3 * DMD_BYTE_WIDTH] & mask;

		dst += 4 * DMD_BYTE_WIDTH;
		src += 4 * DMD_BYTE_WIDTH;
	}

	dmd_trans_data_ptr += 2;

	if (dmd_trans_data_ptr[1] == 0)
		dmd_in_transition = FALSE;
}


static U8 vstripe_left2right_data_table[] = {
	0, 0xF, 0, 0xF0,
	1, 0xF, 1, 0xF0,
	2, 0xF, 2, 0xF0,
	3, 0xF, 3, 0xF0,
	4, 0xF, 4, 0xF0,
	5, 0xF, 5, 0xF0,
	6, 0xF, 6, 0xF0,
	7, 0xF, 7, 0xF0,
	8, 0xF, 8, 0xF0,
	9, 0xF, 9, 0xF0,
	10, 0xF, 10, 0xF0,
	11, 0xF, 11, 0xF0,
	12, 0xF, 12, 0xF0,
	13, 0xF, 13, 0xF0,
	14, 0xF, 14, 0xF0,
	15, 0xF, 15, 0xF0,
	0, 0
};

dmd_transition_t trans_vstripe_left2right = {
	.composite_init = trans_vstripe_init,
	.composite_old = dmd_copy_low_to_high,
	.composite_new = trans_vstripe_new,
	.delay = TIME_33MS,
	.arg = { .ptr = vstripe_left2right_data_table },
};


static U8 vstripe_right2left_data_table[] = {
	15, 0xF0, 15, 0xF,
	14, 0xF0, 14, 0xF,
	13, 0xF0, 13, 0xF,
	12, 0xF0, 12, 0xF,
	11, 0xF0, 11, 0xF,
	10, 0xF0, 10, 0xF,
	9, 0xF0, 9, 0xF,
	8, 0xF0, 8, 0xF,
	7, 0xF0, 7, 0xF,
	6, 0xF0, 6, 0xF,
	5, 0xF0, 5, 0xF,
	4, 0xF0, 4, 0xF,
	3, 0xF0, 3, 0xF,
	2, 0xF0, 2, 0xF,
	1, 0xF0, 1, 0xF,
	0, 0xF0, 0, 0xF,
	0, 0
};

dmd_transition_t trans_vstripe_right2left = {
	.composite_init = trans_vstripe_init,
	.composite_old = dmd_copy_low_to_high,
	.composite_new = trans_vstripe_new,
	.delay = TIME_33MS,
	.arg = { .ptr = vstripe_right2left_data_table },
};


/*********************************************************************/

U8 trans_bitfade_mask_table[] = {
	0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF,
	0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF,
	0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF,
	0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF,
};

void trans_bitfade_init (void)
{
	dmd_trans_data_ptr = trans_bitfade_mask_table;
}

void trans_bitfade_old (void)
{
	register U8 *mask = dmd_trans_data_ptr;
	register U8 *src = dmd_low_buffer;
	register U8 *dst = dmd_high_buffer;
	register U16 i;

	if (mask[0] != 0xFF)
	{
		for (i=0; i < 32L * 16; i += 4)
		{
			dst[i] = src[i] & ~mask[0];
			dst[i+1] = src[i+1] & ~mask[1];
			dst[i+2] = src[i+2] & ~mask[2];
			dst[i+3] = src[i+3] & ~mask[3];
		}
	}
}

void trans_bitfade_new (void)
{
	register U8 *mask = dmd_trans_data_ptr++;
	register U8 *src = dmd_low_buffer;
	register U8 *dst = dmd_high_buffer;
	register U16 i;

	if (mask[0] == 0xFF)
	{
		dmd_copy_low_to_high ();
		dmd_in_transition = FALSE;
	}
	else
	{
		for (i=0; i < 32L * 16; i += 4)
		{
			dst[i] |= src[i] & mask[0];
			dst[i+1] |= src[i+1] & mask[1];
			dst[i+2] |= src[i+2] & mask[2];
			dst[i+3] |= src[i+3] & mask[3];
		}
	}
}


dmd_transition_t trans_bitfade_slow = {
	.composite_init = trans_bitfade_init,
	.composite_old = trans_bitfade_old,
	.composite_new = trans_bitfade_new,
	.delay = TIME_66MS,
	.arg = { .u16 = 0 },
	.count = 8,
};


dmd_transition_t trans_bitfade_fast = {
	.composite_init = trans_bitfade_init,
	.composite_old = trans_bitfade_old,
	.composite_new = trans_bitfade_new,
	.delay = TIME_33MS,
	.arg = { .u16 = 0 },
	.count = 8,
};

