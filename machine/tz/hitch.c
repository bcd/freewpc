/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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

__local__ U8 hitch_count;


void hitchhiker_deff (void)
{
	dmd_alloc_low ();
	frame_draw (IMG_HITCHER);
	dmd_draw_fif (fif_hitcher);
	psprintf ("%d HITCHHIKER", "%d HITCHHIKERS", hitch_count);
	font_render_string_center (&font_fixed6, 76, 10, sprintf_buffer);
	dmd_sched_transition (&trans_scroll_left);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


CALLSET_ENTRY (hitch, sw_hitchhiker)
{
	(void)event_did_follow (rocket, hitchhiker);
	if (lamp_test (LM_PANEL_HH))
	{
		score (SC_10M);
		sound_send (SND_HITCHHIKER_COUNT);
	}
	else
	{
		score (SC_250K);
		sound_send (SND_HITCHHIKER_DRIVE_BY);
	}
	hitch_count++;
	deff_start (DEFF_HITCHHIKER);
}


CALLSET_ENTRY(hitch, start_player)
{
	hitch_count = 1;
}



