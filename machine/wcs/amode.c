/*
 * Copyright 2008-2011 by Brian Dominy <brian@oddchange.com>
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
#include <amode.h>

static U8 lamplist;

CALLSET_ENTRY (wcs_amode, start_without_credits)
{
	sound_send (SND_WHISTLE);
}


CALLSET_ENTRY (wcs_amode, amode_page)
{
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed6, 84, 9, "STARRING");
	font_render_string_center (&font_fixed6, 84, 21, "STRIKER");
	dmd_text_outline ();

	dmd_alloc_pair ();
	frame_draw (IMG_STRIKER);
	dmd_overlay_outline ();
	dmd_show2 ();
	amode_page_end (5);
}


void amode_pgj_logo (void)
{
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed6, 44, 25, "10");
	font_render_string_center (&font_var5, 64, 27, "YEARS");
	dmd_text_outline ();

	dmd_alloc_pair ();
	frame_draw (IMG_PGJ);
	dmd_overlay_outline ();
	dmd_show2 ();
	task_sleep_sec (15);
	deff_exit ();
}

void amode_leff1 (void)
{
	register U8 my_lamplist = lamplist;
	lamplist_set_apply_delay (TIME_66MS);
	for (;;)
	{
		lamplist_apply (my_lamplist, leff_toggle);
		task_sleep (TIME_100MS);
	}
}


void amode_leff (void)
{
	gi_leff_enable (PINIO_GI_STRINGS);
	leff_quick_free (LM_START_BUTTON);

	for (lamplist = LAMPLIST_GOAL_COUNTS; lamplist <= LAMPLIST_RAMP_TICKETS; lamplist++)
	{
		leff_create_peer (amode_leff1);
		task_sleep (TIME_33MS);
	}

	for (;;)
	{
		leff_toggle (LM_TRAVEL);
		leff_toggle (LM_TACKLE);
		leff_toggle (LM_GOAL_JACKPOT);
		leff_toggle (LM_FREE_KICK);
		leff_toggle (LM_FINAL_DRAW);
		leff_toggle (LM_LIGHT_KICKBACK);
		leff_toggle (LM_ULTRA_RAMP_COLLECT);

		task_sleep (TIME_100MS);

		leff_toggle (LM_WORLD_CUP_FINAL);
		leff_toggle (LM_GOAL);
		leff_toggle (LM_ULTRA_RAMP_COLLECT);
		leff_toggle (LM_TV_AWARD);
		leff_toggle (LM_LIGHT_MAGNA_GOALIE);

		task_sleep (TIME_100MS);
	}
}

