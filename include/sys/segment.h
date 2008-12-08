/*
 * Copyright 2005-2008 by Brian Dominy <brian@oddchange.com>
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

#define SEG_RIGHT      (SEG_UPR_RIGHT+SEG_LWR_RIGHT)
#define SEG_LEFT       (SEG_UPR_LEFT+SEG_LWR_LEFT)
#define SEG_MID        (SEG_MID_LEFT+SEG_MID_RIGHT)
#define SEG_VERT       (SEG_VERT_TOP+SEG_VERT_BOT)

#define SEG_SECTIONS       2
#define SEG_SECTION_SIZE   16
#define SEG_PAGES          4

typedef segbits_t seg_section_t[SEG_SECTION_SIZE];
typedef seg_section_t seg_page_t[SEG_SECTIONS];

segbits_t seg_translate_char (char c);
segbits_t *seg_write_char (segbits_t *sa, char c);
void seg_write_string (U8 row, U8 col, const char *s);
U8 seg_strlen (const char *s);
void seg_erase (void);
void seg_init (void);
void seg_alloc (void);
void seg_show (void);

#endif /* _SYS_SEGMENT_H */
