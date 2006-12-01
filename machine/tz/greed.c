/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

U8 greed_sound_index;

__local__ U8 greed_set;


U8 greed_sounds[] = { 
	SND_GREED_DEFAULT_1,
	SND_GREED_DEFAULT_2,
	SND_GREED_DEFAULT_3,
	SND_GREED_DEFAULT_4,
};


void common_greed_handler (U8 target)
{
	const U8 sw = task_get_arg ();
	const U8 lamp = switch_lookup_lamp (sw);

	if (lamp_test (LM_PANEL_GREED))
	{
		score (SC_50K);
		sound_send (SND_GREED_ROUND_BOOM);
		lamp_tristate_flash (lamp);
	}
	else if ((greed_set & target) == 0)
	{
		greed_set |= target;

		sound_send (SND_THUNDER1);
		score (SC_25K);
		task_sleep (TIME_500MS);
		lamp_tristate_on (lamp);

		if (greed_set == 0x7F)
		{
		}
	}
	else
	{
		greed_sound_index++;
		if (greed_sound_index >= 4)
			greed_sound_index = 0;
		sound_send (greed_sounds[greed_sound_index]);
		score (SC_5K);
	}
}


CALLSET_ENTRY (greed, door_panel_awarded)
{
	if (lamp_test (LM_PANEL_GREED))
	{
		lamp_tristate_flash (LM_LL_5M);
		lamp_tristate_flash (LM_ML_5M);
		lamp_tristate_flash (LM_UL_5M);
		lamp_tristate_flash (LM_UR_5M);
		lamp_tristate_flash (LM_MR1_5M);
		lamp_tristate_flash (LM_MR2_5M);
		lamp_tristate_flash (LM_LR_5M);
	}
}


CALLSET_ENTRY (greed, start_player)
{
	greed_set = 0;
}


CALLSET_ENTRY (greed, sw_greed_handler1)
{
	common_greed_handler (0x1);
}

CALLSET_ENTRY (greed, sw_greed_handler2)
{
	common_greed_handler (0x2);
}

CALLSET_ENTRY (greed, sw_greed_handler3)
{
	common_greed_handler (0x4);
}

CALLSET_ENTRY (greed, sw_greed_handler4)
{
	common_greed_handler (0x8);
}

CALLSET_ENTRY (greed, sw_greed_handler5)
{
	common_greed_handler (0x10);
}

CALLSET_ENTRY (greed, sw_greed_handler6)
{
	common_greed_handler (0x20);
}

CALLSET_ENTRY (greed, sw_greed_handler7)
{
	common_greed_handler (0x40);
}


