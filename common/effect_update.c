/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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
#include <coin.h>

U8 effect_update_counter;

/* CALLSET_SECTION (effect_update, __effect__) */

static void update_complete (void)
{
	effect_update_counter = 5;
}

static void effect_update_task (void)
{
	deff_update ();
	music_refresh ();
	if (in_live_game)
	{
		task_sleep (TIME_33MS);
		callset_invoke (lamp_update);
	}
	if (!in_test)
		lamp_start_update ();
	task_exit ();
}

void effect_update_request (void)
{
	effect_update_counter = 0;
}

CALLSET_ENTRY (effect_update, idle_every_100ms, start_ball, end_ball)
{
	if (!in_test)
	{
		if (in_game)
			callset_invoke (device_update);
		if (effect_update_counter == 0)
		{
			task_recreate_gid_while (GID_EFFECT_UPDATE, effect_update_task,
				TASK_DURATION_INF);
			update_complete ();
		}
		else
		{
			--effect_update_counter;
		}
	}
}

CALLSET_ENTRY (effect_update, init)
{
	update_complete ();
}


