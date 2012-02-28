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

#include <freewpc.h>
#include <window.h>
#include <test.h>

extern U8 sw_last_scheduled;
extern U8 switch_display_timer;

/* TODO - this is very WPC specific at the moment */
#define MAKE_SWITCH(col,row)	(((col) * 8) + (row) - 1)

void switch_matrix_draw (void)
{
	U8 row, col;

	for (row=0; row < 8; row++)
	{
		for (col=0; col < 8; col++)
		{
			U8 sw = MAKE_SWITCH (col+1,row+1);
#if 0 /* whether or not it is an opto isn't important now */
			bool opto_p = switch_is_opto (sw);
#endif
			bool state_p = switch_poll (sw);
			register U8 *dmd = dmd_low_buffer +
				((U16)row << 6) + (col >> 1);
			U8 mask = (col & 1) ? 0xE0 : 0x0E;

			if (state_p)
			{
				dmd[0 * DMD_BYTE_WIDTH] |= mask;
				dmd[1 * DMD_BYTE_WIDTH] |= mask & ~0x44;
				dmd[2 * DMD_BYTE_WIDTH] |= mask;
			}
			else
			{
				dmd[0 * DMD_BYTE_WIDTH] &= ~mask;
				dmd[1 * DMD_BYTE_WIDTH] |= mask & 0x44;
				dmd[2 * DMD_BYTE_WIDTH] &= ~mask;
			}
			dmd[3 * DMD_BYTE_WIDTH] = 0;
		}
	}
}

void switch_test_display (U8 sw)
{
	sprintf_far_string (names_of_switches + sw_last_scheduled);
	font_render_string_left (&font_var5, 50, 9, sprintf_buffer);
}



void switch_edges_update (void)
{
	extern __test__ void switch_edges_draw (void);

	/* TODO : here's what needs to happen.
	We begin by drawing the switch matrix normally, then we
	take a snapshot of raw switches.  Every 16ms, we do
	a compare of the current raw switches vs. our snapshot.
	If the same, nothing to be done.  If different, save
	current as the new snapshot and redraw the switch matrix.
	Even better, we could only redraw the columns that changed.
	Also show the transition(s) that just occurred.
	(For switch levels, iterate through the active switches
	accounting for backwards optos continuously.) */

	dmd_alloc_low_clean ();

	if (switch_display_timer == 0)
	{
		if (sw_last_scheduled)
		{
			switch_test_display (sw_last_scheduled);
			switch_display_timer = 8;
		}
	}
	else
	{
		switch_display_timer--;
		switch_test_display (sw_last_scheduled);
		if (switch_display_timer == 0)
		{
			sw_last_scheduled = 0;
		}
	}

	switch_edges_draw ();
}

void switch_levels_update (void)
{
	extern __test__ void switch_levels_draw (void);
	dmd_alloc_low_clean ();
	switch_levels_draw ();
}

