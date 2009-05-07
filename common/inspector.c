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

/**
 * \file
 * \brief Built-in memory inspection tool for debugging
 *
 * The inspector is a simple memory dump tool to the DMD display.
 * When enabled in the build, it is activated by pressing the Extra Ball Buy-in
 * button.  (For games without a button, need to figure out what to do...
 *
 * Use the flipper buttons to page up/down.  The DMD shows 16 bytes at a time.
 *
 * The inspector can be enabled in attract mode or during a game, but not in
 * test mode.  While active, kickouts are disabled and all game timers
 * should stop running.
 */

#include <freewpc.h>


/** The number of bytes per page to display */
#define INSPECTOR_PAGE_SIZE 16

/** The base address of the memory page being shown now */
U8 *inspector_addr;


/** The inspector uses the display effect module to gain
 * control over the DMD.  It has a higher priority than
 * anything else. */
void inspector_deff (void)
{
	U8 *addr, *p;
	U8 y;

	while (!in_test)
	{
		addr = inspector_addr;
		
		dmd_alloc_low_clean ();
		for (p = addr, y=2; p < addr + 0x10; p += 0x4, y += 7)
		{
			sprintf ("%p   %02X %02X %02X %02X",
				p, p[0], p[1], p[2], p[3]);
			font_render_string_left (&font_mono5, 2, y, sprintf_buffer);
		}
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
	deff_exit ();
}


/** Handles the buyin button for starting/stopping the inspector */
CALLSET_ENTRY (inspector, sw_buyin_button)
{
	if (deff_get_active () == DEFF_INSPECTOR)
	{
		deff_stop (DEFF_INSPECTOR);
		kickout_unlock (KLOCK_DEBUGGER);
	}
	else
	{
		deff_start (DEFF_INSPECTOR);
		kickout_lock (KLOCK_DEBUGGER);
	}
}


/* Switch handlers for changing the current address shown.
 * The flippers can be held down to scroll faster. */

void inspector_fast_page_down (void)
{
	task_sleep (TIME_500MS);
	while (switch_poll_logical (SW_LEFT_BUTTON))
	{
		if (inspector_addr > 0)
			inspector_addr -= INSPECTOR_PAGE_SIZE * 4;
		task_sleep (TIME_33MS);
	}
	task_exit ();
}

CALLSET_ENTRY (inspector, sw_left_button)
{
	if (deff_get_active () == DEFF_INSPECTOR)
	{
		if (inspector_addr > 0)
			inspector_addr -= INSPECTOR_PAGE_SIZE;
		task_recreate_gid (GID_INSPECTOR_FAST_PAGE, inspector_fast_page_down);
	}
}

void inspector_fast_page_up (void)
{
	task_sleep (TIME_500MS);
	while (switch_poll_logical (SW_RIGHT_BUTTON))
	{
		if (inspector_addr < (U8 *)RAM_SIZE-INSPECTOR_PAGE_SIZE)
			inspector_addr += INSPECTOR_PAGE_SIZE * 4;
		task_sleep (TIME_33MS);
	}
	task_exit ();
}


CALLSET_ENTRY (inspector, sw_right_button)
{
	if (deff_get_active () == DEFF_INSPECTOR)
	{
		if (inspector_addr < (U8 *)(RAM_SIZE-INSPECTOR_PAGE_SIZE))
			inspector_addr += INSPECTOR_PAGE_SIZE;
		task_recreate_gid (GID_INSPECTOR_FAST_PAGE, inspector_fast_page_up);
	}
#endif
}

CALLSET_ENTRY (inspector, init)
{
	inspector_addr = 0;
}
