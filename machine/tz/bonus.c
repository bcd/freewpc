/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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


void bonus_deff (void)
{
	extern U8 door_panels_started;

	sample_start (MUS_FADE_BONUS, SL_500MS);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 8, "BONUS");

	if (door_panels_started > 0)
	{
		score_multiple (SC_100K, door_panels_started);

		psprintf ("%d DOOR PANEL", "%d DOOR PANELS", door_panels_started);
		font_render_string_center (&font_mono5, 64, 18, sprintf_buffer);
		if (door_panels_started < 10)
			sprintf ("%d00,000", door_panels_started);
		else
			sprintf ("1,%d00,000", door_panels_started - 10);
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
	}

	dmd_sched_transition (&trans_scroll_up);
	dmd_show_low ();
	sound_send (SND_GREED_ROUND_BOOM);
	task_sleep_sec (1);

	dmd_alloc_low_clean ();
	scores_draw ();
	dmd_sched_transition (&trans_scroll_up);
	dmd_show_low ();
	sample_start (MUS_FADE_EXIT, SL_2S);
	task_sleep (TIME_16MS);
	sample_start (SND_GREED_ROUND_BOOM, SL_1S);

	task_sleep_sec (2);
	task_sleep (TIME_500MS);
	deff_exit ();
}
