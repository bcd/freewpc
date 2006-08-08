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
	const switch_info_t * const swinfo = switch_lookup (sw);

	if (lamp_test (LM_PANEL_GREED))
	{
		score (SC_50K);
		sound_send (SND_GREED_ROUND_BOOM);
		lamp_tristate_flash (swinfo->lamp);
	}
	else if ((greed_set & target) == 0)
	{
		greed_set |= target;

		sound_send (SND_THUNDER1);
		score (SC_25K);
		task_sleep (TIME_500MS);
		lamp_tristate_on (swinfo->lamp);

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


void sw_greed_handler1 (void)
{
	common_greed_handler (0x1);
}

void sw_greed_handler2 (void)
{
	common_greed_handler (0x2);
}

void sw_greed_handler3 (void)
{
	common_greed_handler (0x4);
}

void sw_greed_handler4 (void)
{
	common_greed_handler (0x8);
}

void sw_greed_handler5 (void)
{
	common_greed_handler (0x10);
}

void sw_greed_handler6 (void)
{
	common_greed_handler (0x20);
}

void sw_greed_handler7 (void)
{
	common_greed_handler (0x40);
}



DECLARE_SWITCH_DRIVER (sw_greed1)
{
	.fn = sw_greed_handler1,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.lamp = LM_LL_5M,
};

DECLARE_SWITCH_DRIVER (sw_greed2)
{
	.fn = sw_greed_handler2,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.lamp = LM_ML_5M,
};

DECLARE_SWITCH_DRIVER (sw_greed3)
{
	.fn = sw_greed_handler3,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.lamp = LM_UL_5M,
};

DECLARE_SWITCH_DRIVER (sw_greed4)
{
	.fn = sw_greed_handler4,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.lamp = LM_UR_5M,
};

DECLARE_SWITCH_DRIVER (sw_greed5)
{
	.fn = sw_greed_handler5,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.lamp = LM_MR1_5M,
};

DECLARE_SWITCH_DRIVER (sw_greed6)
{
	.fn = sw_greed_handler6,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.lamp = LM_MR2_5M,
};

DECLARE_SWITCH_DRIVER (sw_greed7)
{
	.fn = sw_greed_handler7,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.lamp = LM_LR_5M,
};

