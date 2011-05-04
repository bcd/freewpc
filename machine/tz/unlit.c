/*
 * Copyright 2010 by Ewan Meadows <sonny_jim@hotmail.com>
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
 * Award the player a door for collecting 5 unlit shots
 *  
 * Targets: rightramp, slot, piano, HH, lock 
 * Unlit shot awarded when above targets collected 'unlit'
 * 
 * A jet level up awards an unlit shot.
 *
 * Unlit shots are not awarded from the same shot hit twice consectively
 *
 * Backdoor Award cannot be collected by Hitchhiker or Jets, it must be collected
 * by one of the other unlit targets
 * 
 * Collecting mpf, multiball, door panel, gumball or lock 
 * resets unlit shot count
 * 
 * Only works once per game, 20M bonus on each ball bonus thereafter */

/* CALLSET_SECTION (unlit, __machine2__) */
#include <freewpc.h>

U8 unlit_shot_count;
bool backdoor_award_collected;
/* last switch that was collected unlit */
U8 unlit_called_from_stored;

void backdoor_award_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("BACKDOOR AWARD");
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

inline static bool can_award_unlit_shot (U8 unlit_called_from)
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

bool unlit_shots_lit (void)
{
	/*TODO Use a big if to check whether its actually possible to get
	 * another unlit shot
	 */
	return TRUE;
}

void award_unlit_shot (U8 unlit_called_from)
{
	if (can_award_unlit_shot (unlit_called_from))
	{
		unlit_shot_count++;
		/* Don't allow collecting from Hitchhiker or jets */
		if (unlit_shot_count > 4 && unlit_called_from != (SW_HITCHHIKER)
			&& unlit_called_from != (SW_BOTTOM_JET))
		{
			sound_send (SND_JUST_TAKEN_A_DETOUR);
			deff_start (DEFF_BACKDOOR_AWARD);
			backdoor_award_collected = TRUE;
			unlit_shot_count = 0;
			callset_invoke (award_door_panel);	
		}
		/* Reset if the player hits the same unlit shot twice */
		if (unlit_called_from == unlit_called_from_stored)
			unlit_shot_count = 0;	
		if (unlit_shot_count == 4 )
		{
			//TODO Check for unlit shots
			/* Hint to the player that backdoor award is ready */
			sound_send (SND_TWILIGHT_ZONE_SHORT_SOUND);
		}
		/* Store where we were called from */
		unlit_called_from_stored = unlit_called_from;
	}
}

CALLSET_ENTRY (unlit, serve_ball)
{
	unlit_shot_count = 0;
	unlit_called_from_stored = 0;

}

CALLSET_ENTRY (unlit, start_game)
{
	unlit_shot_count = 0;
	unlit_called_from_stored = 0;
	backdoor_award_collected = FALSE;
}
