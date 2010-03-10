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
/* TODO Use EB button to change music */

/* CALLSET_SECTION (music, __machine2__) */

#include <freewpc.h>

U8 current_music_selection;

struct {
	U8 music_selection;
	const char *music_selection_text;
} music_selections[]= {
	{ MUS_TZ_IN_PLAY, "NORMAL" }, 
	{ MUS_FASTLOCK_EARTHSHAKER, "EARTHSHAKER" },
	{ MUS_FASTLOCK_BANZAI_RUN, "BANZAI RUN" },
	{ MUS_FASTLOCK_WHIRLWIND, "WHIRLWIND" },
	{ MUS_FASTLOCK_FUNHOUSE, "FUNHOUSE" },
	{ MUS_FASTLOCK_ADDAMS_FAMILY, "ADDAMS FAMILY" },
};

void music_button_monitor_task (void)
{
	for (;;)
	{
		if ((switch_poll_logical (SW_LEFT_BUTTON))&& global_flag_test (GLOBAL_FLAG_BALL_AT_PLUNGER))

		{
			bounded_decrement (current_music_selection, 0);
			music_request (music_selections[current_music_selection].music_selection, PRI_SCORES);
		}
		else if ((switch_poll_logical (SW_RIGHT_BUTTON) && global_flag_test (GLOBAL_FLAG_BALL_AT_PLUNGER)))
		{
			bounded_increment (current_music_selection, 5);
			music_request (music_selections[current_music_selection].music_selection, PRI_SCORES);
		}
		task_sleep (TIME_100MS);	
	}
}
//TODO Shooter is an edge switch if this makes any difference
CALLSET_ENTRY (tz_music, init)
{
	
	task_create_gid (GID_MUSIC_BUTTON_MONITOR, music_button_monitor_task);
}
