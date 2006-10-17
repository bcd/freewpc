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

U8 clock_hits;
U8 clock_multiplier;

static score_id_t clock_score_table[] = {
	SC_10K, SC_20K, SC_30K, SC_40K, SC_50K
};

void clock_multiplier_monitor (void)
{
	while (clock_multiplier > 1)
	{
		task_sleep_sec (5);
		clock_multiplier--;
	}
	task_exit ();
}

void sw_clock_target_handler (void)
{
	U8 n;

	if (!lamp_test (LM_PANEL_CLOCK_MIL) && !lamp_test (LM_PANEL_CLOCK_CHAOS))
	{
		score (SC_5K);
		sound_send (SND_NO_CREDITS);
		return;
	}

	sound_send (SND_CLOCK_BELL);
	leff_start (LEFF_CLOCK_TARGET);
	for (n=0 ; n < clock_multiplier; n++)
		score (clock_score_table[clock_hits]);
	if (clock_hits < 4)
		clock_hits++;

	if (lamp_test (LM_PANEL_CLOCK_MIL))
	{
		clock_multiplier++;
		task_recreate_gid (GID_CLOCK_MIL_MULTIPLIER, clock_multiplier_monitor);
	}
}


CALLSET_ENTRY(clocktarget, start_player)
{
	lamp_tristate_off (LM_CLOCK_MILLIONS);
	clock_hits = 0;
	clock_multiplier = 1;
}


CALLSET_ENTRY (clocktarget, door_panel_awarded)
{
	if (lamp_test (LM_PANEL_CLOCK_MIL))
		lamp_tristate_flash (LM_CLOCK_MILLIONS);
}


DECLARE_SWITCH_DRIVER (sw_clock_target)
{
	.fn = sw_clock_target_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.lamp = LM_CLOCK_MILLIONS,
};

