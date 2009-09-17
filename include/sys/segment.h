/*
 * Copyright 2005-2009 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SYS_SEGMENT_H
#define _SYS_SEGMENT_H

/* Names for each part of a 16-bit segmented character */

typedef U16 segbits_t;

#define SEG_UL_DIAG    0x0001
#define SEG_VERT_TOP   0x0002
#define SEG_UR_DIAG    0x0004
#define SEG_MID_RIGHT  0x0008
#define SEG_LR_DIAG    0x0010
#define SEG_VERT_BOT   0x0020
#define SEG_LL_DIAG    0x0040
#define SEG_COMMA      0x0080
#define SEG_TOP        0x0100
#define SEG_UPR_RIGHT  0x0200
#define SEG_LWR_RIGHT  0x0400
#define SEG_BOT        0x0800
#define SEG_LWR_LEFT   0x1000
#define SEG_UPR_LEFT   0x2000
#define SEG_MID_LEFT   0x4000
#define SEG_PERIOD     0x8000

/* Shorthand notation for some common combinations */

#define SEG_RIGHT      (SEG_UPR_RIGHT+SEG_LWR_RIGHT)
#define SEG_LEFT       (SEG_UPR_LEFT+SEG_LWR_LEFT)
#define SEG_MID        (SEG_MID_LEFT+SEG_MID_RIGHT)
#define SEG_VERT       (SEG_VERT_TOP+SEG_VERT_BOT)

/* Fake characters */

#define SEGCHAR_ALL       0  /* Light up all segments of the display */
#define SEGCHAR_HORIZ     1  /* Light up all horizontal segments */
#define SEGCHAR_VERT      2  /* Light up all vertical segments */
#define SEGCHAR_STROBE    3  /* Light up horizontal segments gradually */
	#define SEGCHAR_STROBE_COUNT 4
   #define SEG_STROBE0       SEG_TOP
	#define SEG_STROBE1       (SEG_STROBE0 + SEG_UPR_LEFT+SEG_UL_DIAG+SEG_VERT_TOP+SEG_UR_DIAG+SEG_UPR_RIGHT)
	#define SEG_STROBE2       (SEG_STROBE1 + SEG_MID)
	#define SEG_STROBE3       (SEG_STROBE2 + SEG_LWR_LEFT+SEG_LL_DIAG+SEG_VERT_BOT+SEG_LR_DIAG+SEG_LWR_RIGHT)


/* The dimensions of the entire display */

#define SEG_SECTIONS       2
#define SEG_SECTION_SIZE   16

/* The number of frames of display data kept in memory.
Alloc pages are handed out at request; fixed pages can
be used whenever */

#define SEG_ALLOC_PAGES    4
#define SEG_FIXED_PAGES    3

typedef segbits_t seg_section_t[SEG_SECTION_SIZE];
typedef seg_section_t seg_page_t[SEG_SECTIONS];

typedef struct
{
	void (*init) (void);
	bool (*update) (seg_page_t *src, U8 iteration);
	U8 delay;
} seg_transition_t;

segbits_t *seg_write_char (segbits_t *sa, char c);
void seg_write_string (U8 row, U8 col, const char *s);
U8 seg_strlen (const char *s);
void seg_erase (void);
void seg_fill (segbits_t segs);
void seg_init (void);
void seg_alloc (void);
void seg_alloc_clean (void);
void seg_show (void);
void dmd_rtt (void);
void seg_sched_transition (seg_transition_t *trans);
void seg_reset_transition (void);

extern seg_transition_t
	seg_trans_center_out, seg_trans_ltr, seg_trans_rtl,
	seg_trans_fast_center_out, seg_trans_fade;

#endif /* _SYS_SEGMENT_H */
