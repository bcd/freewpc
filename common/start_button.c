/*
 * Copyright 2008-2010 by Brian Dominy <brian@oddchange.com>
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

extern U8 initials_enter_timer;
extern U8 pin_enter_timer;

extern U8 switch_stress_enable;

__test2__ void switch_stress_drain (void);

/**
 * Handle the start button.  There should be only one callset entry for
 * this, because only one of the cases should be handled at any time.
 * Modules that want to hook the start button should declare a
 * start_button_handler and modify the logic here.
 */
CALLSET_ENTRY (start_button, sw_start_button)
{
	/* if (free_timer_test (TMR_IGNORE_START_BUTTON))
	{
	}
	else */ if (deff_get_active () == DEFF_SYSTEM_RESET)
	{
	}
#ifdef CONFIG_BUYIN
	else if (deff_get_active () == DEFF_BUYIN_OFFER)
	{
		SECTION_VOIDCALL (__common__, buyin_start_button_handler);
	}
#endif
	else if (switch_stress_enable && in_live_game && valid_playfield)
	{
		switch_stress_drain ();
	}
	else if (initials_enter_timer)
	{
		SECTION_VOIDCALL (__common__, initials_start_button_handler);
	}
	else if (pin_enter_timer)
	{
		SECTION_VOIDCALL (__common__, pin_start_button_handler);
	}
#ifdef CONFIG_TEST
	else if (in_test)
	{
		SECTION_VOIDCALL (__test__, test_mode_start_button_handler);
	}
#endif
	else
	{
		VOIDCALL (game_start_button_handler);
	}
}

