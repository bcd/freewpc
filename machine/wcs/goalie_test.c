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

#include <freewpc.h>
#include <window.h>
#include <test.h>


void wcs_goalie_test_init (void)
{
}

void wcs_goalie_test_draw (void)
{
	extern __machine__ void goalie_status_display (void);
	goalie_status_display ();
}

void wcs_goalie_test_enter (void)
{
	extern __machine__ void goalie_test_toggle (void);
	goalie_test_toggle ();
}

void wcs_goalie_test_up (void)
{
}

void wcs_goalie_test_down (void)
{
}

struct window_ops wcs_goalie_test_window = {
	DEFAULT_WINDOW,
	.init = wcs_goalie_test_init,
	.draw = wcs_goalie_test_draw,
	.enter = wcs_goalie_test_enter,
	.up = wcs_goalie_test_up,
	.down = wcs_goalie_test_down,
};


struct menu wcs_goalie_test_item = {
	.name = "GOALIE TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &wcs_goalie_test_window, NULL } },
};

