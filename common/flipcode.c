/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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
 * \brief The flipper code recognizer
 */

#include <freewpc.h>

/* The next digit to be entered, which is advanced by pressing the
   left button. */
U8 flipcode_digit;

/* The total sequence of digits entered (last 4 only) in BCD */
U16 flipcode_value;


static void flipcode_reset (void)
{
	flipcode_digit = 0;
	flipcode_value = 0;
	task_kill_gid (GID_FLIPCODE_ACTIVE);
}

static void flipcode_active_task (void)
{
	/* Anytime either flipper is pressed, this task is restarted.
	After so many seconds of inactivity, the flipper code will reset
	to zero. */
	task_sleep_sec (5);
	flipcode_reset ();
	task_exit ();
}

static void flipcode_advance_char (void)
{
	flipcode_digit++;
	task_recreate_gid (GID_FLIPCODE_ACTIVE, flipcode_active_task);
}

static void flipcode_lock_char (void)
{
	flipcode_value = (flipcode_value << 4) | (flipcode_digit & 0x0F);
	flipcode_digit = 0;
	if (flipcode_value)
	{
		task_recreate_gid (GID_FLIPCODE_ACTIVE, flipcode_active_task);
		callset_invoke (flipper_code_entered);
	}
}


CALLSET_ENTRY (flipcode, sw_left_button)
{
	if (deff_get_active () != DEFF_AMODE)
		return;
	flipcode_advance_char ();
}

CALLSET_ENTRY (flipcode, sw_right_button)
{
	if (deff_get_active () != DEFF_AMODE)
		return;
	flipcode_lock_char ();
}

CALLSET_ENTRY (flipcode, amode_start)
{
	flipcode_reset ();
}

