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
#include <window.h>

/**
 * \file
 * \brief Entry of player's initials
 * This is used by the high score module, and could be used for
 * other initials entry like loop champions, etc.
 */


#define NUM_INITIALS_ALLOWED 3

static const char initial_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ *";


/* The amount of time left to enter initials */
U8 initials_enter_timer;

/* The index of the next initial to be entered */
U8 initials_index;

/* The index of the currently selected letter/symbol */
U8 initials_selection;

/* The array of initials */
U8 initials_data[NUM_INITIALS_ALLOWED];

/* The callback function to invoke when initials are entered OK */
void (*initials_enter_complete) (void);


void initials_enter_init (void)
{
	initials_enter_timer = 30;
	initials_index = 0;
	initials_selection = 0;
}


void initials_enter_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_left (&font_fixed6, 2, 9, "ENTER INITIALS");
	sprintf ("%8s", initial_chars + initials_index);
	font_render_string_left (&font_fixed6, 2, 22, sprintf_buffer);
	dmd_show_low ();
}


void initials_enter_left (void)
{
	--initials_selection;
}


void initials_enter_right (void)
{
	++initials_selection;
}


void initials_enter_start (void)
{
	if (++initials_index == NUM_INITIALS_ALLOWED)
	{
		(*initials_enter_complete) ();
	}
}


struct window_ops initials_enter_window = {
	DEFAULT_WINDOW,
	.escape = null_function,
	.init = initials_enter_init,
	.draw = initials_enter_draw,
	.left = initials_enter_left,
	.right = initials_enter_right,
	.start = initials_enter_start,
};


void initials_enter (void)
{
	initials_enter_timer = 30;
	window_push (&initials_enter_window, NULL);
}

