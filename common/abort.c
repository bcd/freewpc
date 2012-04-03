/*
 * Copyright 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Generate the abort event, when both flippers are pressed simultaneously.
 */

void abort_monitor_task (void)
{
	U8 count = 3;
	while (count > 0)
	{
		task_sleep (TIME_66MS);
		if (!switch_poll_logical (SW_LEFT_BUTTON)
			|| !switch_poll_logical (SW_RIGHT_BUTTON))
		{
			task_exit ();
		}
		count--;
	}
	callset_invoke (flipper_abort);
	task_exit ();
}


CALLSET_ENTRY (abort, sw_left_button, sw_right_button)
{
	task_recreate_gid (GID_FLIPPER_ABORT_MONITOR, abort_monitor_task);
}

