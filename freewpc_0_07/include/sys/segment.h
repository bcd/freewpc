/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

/* This file is not used by FreeWPC.  It was used in the early
 * days of development when the alphanumeric display was still
 * supported.
 */
#define SEG_TOP			0x1
#define SEG_UPR_RIGHT	0x2
#define SEG_LWR_RIGHT	0x4
#define SEG_BOT			0x8
#define SEG_LWR_LEFT		0x10
#define SEG_UPR_LEFT		0x20
#define SEG_MID			0x40
#define SEG_VERT			0x80

#define SEG_RIGHT			(SEG_UPR_RIGHT+SEG_LWR_RIGHT)
#define SEG_LEFT			(SEG_UPR_LEFT+SEG_LWR_LEFT)

#define SEG_ROWS			4
#define SEG_COLS			16
#define SEG_DATA_SIZE	(SEG_ROWS * SEG_COLS)
#define SEG_PAGES			4

#define SEG_ADDR(p,r,c)		((p * 0x40) + (r * 0x10) + c)

#define SEG_PAGE_0 0x0
#define SEG_PAGE_1 0x40
#define SEG_PAGE_2 0x80
#define SEG_PAGE_3 0xC0

#define SEG_ROW_0 0x00
#define SEG_ROW_1 0x10
#define SEG_ROW_2 0x20
#define SEG_ROW_3 0x30

#define SEG_COL_0 0x0
#define SEG_COL_1 0x1
#define SEG_COL_2 0x2
#define SEG_COL_3 0x3
#define SEG_COL_4 0x4
#define SEG_COL_5 0x5
#define SEG_COL_6 0x6
#define SEG_COL_7 0x7
#define SEG_COL_8 0x8
#define SEG_COL_9 0x9
#define SEG_COL_10 0x9
#define SEG_COL_11 0xA
#define SEG_COL_12 0xB
#define SEG_COL_13 0xC
#define SEG_COL_14 0xD
#define SEG_COL_15 0xE

typedef uint8_t segaddr_t;
typedef uint8_t segbits_t;

void seg_set (segaddr_t sa, segbits_t bits);
segbits_t seg_translate_char (char c);
void seg_write_char (segaddr_t sa, char c);
void seg_write_digit (segaddr_t sa, uint8_t digit);
void seg_write_bcd (segaddr_t sa, bcd_t bcd);
void seg_write_uint8 (segaddr_t sa, uint8_t u8);
void seg_write_hex16 (segaddr_t sa, uint16_t u16);
void seg_write_string (segaddr_t sa, const char *s);
void seg_erase (segaddr_t sa, int8_t len);

#endif

