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

/**
 * \file
 * \brief Transition effects for segment displays
 *
 */

#include <freewpc.h>

#ifdef __m6809__
#define __register__ register
#define __ureg__ asm ("u")
#define __yreg__ asm ("y")
#else
#define __register__
#define __ureg__
#define __yreg__
#endif

__register__ segbits_t *src __ureg__;

segbits_t *src2;

__register__ segbits_t *dst __yreg__;

segbits_t seg_overlay_mask;

segbits_t seg_overlay_data;

void_function seg_apply;

U8 len;


void seg_fade1 (void)
{
	*dst = (*src & seg_overlay_mask)
		| (*src2 & ~seg_overlay_mask);
	src2++;
}

void seg_overlay1 (void)
{
	*dst = (*src & seg_overlay_mask) | seg_overlay_data;
}


void seg_apply_init (segbits_t *_dst, segbits_t *_src, U8 _len)
{
	dst = _dst;
	src = _src;
	len = _len;
}

void seg_apply_loop (void)
{
	U8 n = len;
	while (n != 0)
	{
		seg_apply ();
		src++;
		dst++;
		n--;
	}
}


void seg_effect_demo (void)
{
	extern segbits_t *seg_writable_page, *seg_visible_page;
	segbits_t *src, *dst;
	U8 bit0 = 0;
	
	seg_alloc ();
	seg_apply = seg_overlay1;
	seg_overlay_data = 0;
	seg_overlay_mask = 0x5555;
	src = seg_visible_page;
	dst = seg_writable_page;
	for (;;)
	{
		dbprintf ("Mask = %04lX\n", seg_overlay_mask);
		dbprintf ("dst = %p  src = %p\n", dst, seg_visible_page);
		seg_apply_init (dst, src, sizeof (seg_page_t));
		seg_apply_loop ();

		seg_writable_page = seg_visible_page;

		seg_overlay_mask <<= 1;
		seg_overlay_mask |= bit0;
		bit0 ^= 1;
		task_sleep (TIME_100MS);
	}
}


