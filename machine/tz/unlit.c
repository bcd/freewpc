/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com
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

/* Backdoor Award Rules:
 * Award the player a door for getting a certain amount of unlit shots 
 * Currently deadend, rightramp, slot, piano, HH, lock are awarded when 'unlit'
 * Collecting mpf, multiball, door panel, gumball or lock resets count
 * Only works once per game, 20M bonus on each ball */

/* CALLSET_SECTION (unlit, __machine2__) */
#include <freewpc.h>

__local__ U8 unlit_shot_count;
__local__ bool backdoor_award_collected;
/* last switch that was collected 'unlit; */
__local__ U8 unlit_called_from_stored;

void backdoor_award_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("BACKDOOR AWARD");
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (1);
	deff_exit ();
}

CALLSET_ENTRY (unlit, reset_unlit_shots)
{
	unlit_shot_count = 0;
	unlit_called_from_stored = 0;
}

static bool can_award_unlit_shot (U8 unlit_called_from)
{
	/* Don't allow during multiball or if previously collected */
	if (multi_ball_play ())
		return FALSE;
	else if (backdoor_award_collected)
		return FALSE;
	/* Don't allow spamming the same unlit shot */
	else if (unlit_called_from == unlit_called_from_stored)
		return FALSE;
	else
		return TRUE;
}

void award_unlit_shot (U8 unlit_called_from)
{
	if (can_award_unlit_shot (unlit_called_from))
	{
		unlit_shot_count++;
		/* Don't allow collecting from Hitchhiker */
		if (unlit_shot_count > 4 && (unlit_called_from != (SW_HITCHHIKER)))
		{
			sound_send (SND_JUST_TAKEN_A_DETOUR);
			deff_start (DEFF_BACKDOOR_AWARD);
			//task_sleep_sec (1);
			task_kill_gid (GID_DOOR_AWARD_ROTATE);
			door_award_if_possible ();
			backdoor_award_collected = TRUE;
			callset_invoke (reset_unlit_shots);
		}
		/* Reset if the player hits the same unlit shot twice */
		if (unlit_called_from == unlit_called_from_stored)
			callset_invoke (reset_unlit_shots);
		if (unlit_shot_count == 4)
		{
			/* Hint to the player that backdoor award is ready */
			sound_send (SND_TWILIGHT_ZONE_SHORT_SOUND);
			//TODO Use leff
			/*lamplist_apply (LAMPLIST_UNLIT_SHOTS, lamp_flash_on);
			task_sleep_sec (3);
			lamplist_apply (LAMPLIST_UNLIT_SHOTS, lamp_flash_off);*/
		
		}
		/* Store where we were called from */
		unlit_called_from_stored = unlit_called_from;
	}
}

CALLSET_ENTRY (unlit, start_ball)
{
	/* Maybe clear backdoor_award_collected on each ball? */
	callset_invoke (reset_unlit_shots);
}

CALLSET_ENTRY (unlit, start_player)
{
	backdoor_award_collected = FALSE;
//	reset_unlit_shots ();
}
