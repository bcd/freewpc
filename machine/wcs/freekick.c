/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

__local__ U8 free_kicks;

U8 free_kick_multiplier;

void free_kick_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%d KICKS", free_kicks);
	font_render_string_center (&font_fixed6, 64, 5, sprintf_buffer);
	sprintf_score (score_deff_get ());
	font_render_string_center (&font_fixed6, 64, 15, sprintf_buffer);
	sprintf ("NEXT SHOT = %dX", free_kick_multiplier);
	font_render_string_center (&font_var5, 64, 25, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void freekick_lamp_update (void)
{
	if (free_kick_multiplier > 1)
	{
		lamp_tristate_flash (LM_FREE_KICK);
	}
	else if (free_kick_multiplier == 1)
	{
		lamp_tristate_on (LM_FREE_KICK);
	}
	else
	{
		lamp_tristate_off (LM_FREE_KICK);
	}
}

void free_kick_reset_multiplier (void)
{
	free_kick_multiplier = 1;
	freekick_lamp_update ();
}

void free_kick_timeout_multiplier (void)
{
	task_sleep_sec (5);
	free_kick_reset_multiplier ();
	task_exit ();
}


void free_kick_award (void)
{
	free_kicks += free_kick_multiplier;
	score_multiple (SC_1M, free_kick_multiplier);
	if (free_kick_multiplier < 5)
	{
		free_kick_multiplier++;
		freekick_lamp_update ();
	}
	task_recreate_gid (GID_FREEKICK_TIMEOUT, free_kick_timeout_multiplier);
	sound_send (SND_KICK);
	deff_restart (DEFF_FREE_KICK);
}


CALLSET_ENTRY (freekick, sw_free_kick_target)
{
	if (free_kick_multiplier)
		free_kick_award ();
}

CALLSET_ENTRY (freekick, start_player)
{
	free_kicks = 0;
}

CALLSET_ENTRY (freekick, start_ball)
{
	free_kick_reset_multiplier ();
}

