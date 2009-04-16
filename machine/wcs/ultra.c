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

#define NUM_ULTRA_AWARDS 4

U8 ultra_awards_enabled[4];

U8 ultra_award_next;

U8 ultra_awards_finished;


static struct ultra_info {
	U8 lamp;
	U8 shot_count;
	U8 shot_value;
	U8 spot;
	U8 *enable;
} ultra_award_table[] = {
	{ LM_ULTRA_RAMPS, 6, SC_250K, 1, &ultra_awards_enabled[0] },
	{ LM_ULTRA_GOALIE, 6, SC_250K, 1, &ultra_awards_enabled[1] },
	{ LM_ULTRA_JETS, 50, SC_30K, 10, &ultra_awards_enabled[2] },
	{ LM_ULTRA_SPINNER, 30, SC_50K, 5, &ultra_awards_enabled[3]  }
};



void ultra_collect (struct ultra_info *u)
{
	U8 *enable = u->enable;
	if (*enable)
	{
		(*enable)--;
		if (*enable == 0)
		{
			lamp_tristate_on (u->lamp);
			ultra_awards_finished++;
			if (ultra_awards_finished == NUM_ULTRA_AWARDS)
			{
				flag_on (FLAG_ULTRA_MANIA_LIT);
			}
		}
	}
}


void ultra_score (struct ultra_info *u)
{
	ultra_collect (u);
	score (u->shot_value);
}


void ultra_add_shot (void)
{
	struct ultra_info *u;

	if (flag_test (FLAG_ULTRA_MANIA_LIT))
		return;

	u = ultra_award_table + ultra_award_next;
	while (lamp_test (u->lamp))
	{
		ultra_award_next = (ultra_award_next + 1) % NUM_ULTRA_AWARDS;
		u = ultra_award_table + ultra_award_next;
	}

	if (lamp_flash_test (u->lamp))
	{
		U8 n;
		for (n=0; n < u->spot; n++)
			ultra_collect (u);
	}
	else
	{
		*(u->enable) = u->shot_count;
		lamp_tristate_flash (u->lamp);
	}

	ultra_award_next = (ultra_award_next + 1) % NUM_ULTRA_AWARDS;
}


CALLSET_ENTRY (ultra, left_ramp_shot, right_ramp_shot)
{
	ultra_collect (&ultra_award_table[0]);
}

CALLSET_ENTRY (ultra, sw_goalie_target)
{
	ultra_collect (&ultra_award_table[1]);
}

CALLSET_ENTRY (ultra, sw_left_jet, sw_upper_jet, sw_lower_jet)
{
	ultra_collect (&ultra_award_table[2]);
}

CALLSET_ENTRY (ultra, sw_spinner_slow)
{
	ultra_collect (&ultra_award_table[3]);
}

CALLSET_ENTRY (ultra, lamp_update)
{
	lamp_on_if (LM_ULTRA_RAMP_COLLECT, lamp_flash_test (LM_ULTRA_RAMPS));
}

CALLSET_ENTRY (ultra, start_player)
{
	lamplist_apply (LAMPLIST_ULTRA_MODES, lamp_off);
}

CALLSET_ENTRY (ultra, start_ball)
{
	lamplist_apply (LAMPLIST_ULTRA_MODES, lamp_flash_off);
}
