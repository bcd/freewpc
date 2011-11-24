/*
 * Copyright 2008-2011 by Brian Dominy <brian@oddchange.com>
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
#include <simulation.h>

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

unsigned int sim_seg_column = 0;

U16 sim_seg_data[SEG_SECTIONS][SEG_SECTION_SIZE] = { { 0, } };


void sim_seg_set_column (unsigned int col)
{
	sim_seg_column = col & 0x0F;
}


void sim_seg_write (unsigned int section, unsigned int subword, unsigned int val)
{
	U16 mask;
	U16 *ptr = &sim_seg_data[section][sim_seg_column];
	char c;

	if (subword == 0)
	{
		/* Uppermost word of 16-bit reg requires shifting value
		up by 8 bits.  Uppermost 8-bits will be cleared first. */
		mask = 0xFF00;
		val <<= 8;
	}
	else
	{
		/* Lower 8-bits need to be cleared first */
		mask = 0x00FF;
	}

	/* The new segment values are set to the previous segments that aren't
	being touched by this update, plus the new segment values excluding
	comma and period. */
	val = (*ptr & ~mask) | (val & ~(SEG_PERIOD | SEG_COMMA));

	/* If the segments didn't change, nothing else to do */
	if (val == *ptr)
		return;
	*ptr = val;

	/* Decode the segments into a character */
	switch (val)
	{
		case 0: c = ' '; break;
		case SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT: c = '0'; break;
		case SEG_RIGHT: c = '1'; break;
		case SEG_TOP+SEG_UPR_RIGHT+SEG_MID+SEG_LWR_LEFT+SEG_BOT: c = '2'; break;
		case SEG_TOP+SEG_MID+SEG_BOT+SEG_RIGHT: c = '3'; break;
		case SEG_UPR_LEFT+SEG_MID+SEG_RIGHT: c = '4'; break;
		case SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT: c = '5'; break;
		case SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID: c = '6'; break;
		case SEG_TOP+SEG_RIGHT: c = '7'; break;
		case SEG_TOP+SEG_MID+SEG_BOT+SEG_LEFT+SEG_RIGHT: c = '8'; break;
		case SEG_TOP+SEG_MID+SEG_BOT+SEG_UPR_LEFT+SEG_RIGHT: c = '9'; break;
		case SEG_LEFT+SEG_TOP+SEG_MID+SEG_RIGHT: c = 'A'; break;
		case SEG_TOP+SEG_BOT+SEG_MID_RIGHT+SEG_VERT+SEG_RIGHT: c = 'B'; break;
	   case SEG_LEFT+SEG_TOP+SEG_BOT: c = 'C'; break;
	   case SEG_TOP+SEG_BOT+SEG_VERT+SEG_RIGHT: c = 'D'; break;
	   case SEG_LEFT+SEG_TOP+SEG_MID+SEG_BOT: c = 'E'; break;
	   case SEG_LEFT+SEG_TOP+SEG_MID: c = 'F'; break;
	   case SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID_RIGHT: c = 'G'; break;
		case SEG_LEFT+SEG_RIGHT+SEG_MID: c = 'H'; break;
		case SEG_TOP+SEG_VERT+SEG_BOT: c = 'I'; break;
		case SEG_RIGHT+SEG_BOT+SEG_LWR_LEFT: c = 'J'; break;
		case SEG_LEFT+SEG_UR_DIAG+SEG_LR_DIAG+SEG_MID_LEFT: c = 'K'; break;
		case SEG_LEFT+SEG_BOT: c = 'L'; break;
		case SEG_LEFT+SEG_UL_DIAG+SEG_UR_DIAG+SEG_RIGHT: c = 'M'; break;
		case SEG_LEFT+SEG_UL_DIAG+SEG_LR_DIAG+SEG_RIGHT: c = 'N'; break;
		case SEG_LEFT+SEG_TOP+SEG_UPR_RIGHT+SEG_MID: c = 'P'; break;
	   case SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT+SEG_LR_DIAG: c = 'Q'; break;
		case SEG_LEFT+SEG_TOP+SEG_UPR_RIGHT+SEG_MID+SEG_LR_DIAG: c = 'R'; break;
		case SEG_TOP+SEG_VERT: c = 'T'; break;
		case SEG_LEFT+SEG_BOT+SEG_RIGHT: c = 'U'; break;
		case SEG_LEFT+SEG_LL_DIAG+SEG_UR_DIAG: c = 'V'; break;
		case SEG_LEFT+SEG_BOT+SEG_VERT_BOT+SEG_RIGHT: c = 'W'; break;
		case SEG_UL_DIAG+SEG_UR_DIAG+SEG_LL_DIAG+SEG_LR_DIAG: c = 'X'; break;
		case SEG_UL_DIAG+SEG_UR_DIAG+SEG_VERT_BOT: c = 'Y'; break;
	   case SEG_TOP+SEG_UR_DIAG+SEG_LL_DIAG+SEG_BOT: c = 'Z'; break;
		case SEG_UPR_LEFT+SEG_UR_DIAG+SEG_LL_DIAG+SEG_LWR_RIGHT: c = '%'; break;
		case SEG_UR_DIAG+SEG_LR_DIAG: c = '('; break;
		case SEG_UL_DIAG+SEG_LL_DIAG: c = ')'; break;
		case SEG_UL_DIAG+SEG_UR_DIAG+SEG_LL_DIAG+SEG_LR_DIAG+SEG_MID+SEG_VERT: c = '*'; break;
		case SEG_VERT+SEG_MID: c = '+'; break;
		case SEG_MID: c = '-'; break;
		case SEG_UR_DIAG+SEG_LL_DIAG: c = '/'; break;
		case SEG_MID+SEG_BOT: c = '='; break;
		case SEG_UL_DIAG+SEG_LR_DIAG: c = '\\'; break;
		default: c = '-'; break;
	}
	ui_refresh_display (sim_seg_column, section, c);
}

