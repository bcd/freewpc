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

U8 superdog_timer;

U8 superdog_level;

const score_id_t superdog_scores[] = {
	SC_200K, SC_225K, SC_250K, SC_275K, SC_300K,
};
const struct generic_ladder superdog_score_rule = {
	5,
	superdog_scores,
	&superdog_level
};

void superdog_running_deff (void)
{
	deff_exit ();
}

void superdog_score_deff (void)
{
	deff_exit ();
}

void superdog_running (void)
{
	lamp_tristate_flash (LM_SUPER_DOG);
	while (superdog_timer > 0)
	{
		while (system_timer_pause ())
			task_sleep (TIME_250MS);
		task_sleep_sec (1);
		superdog_timer--;
	}
	lamp_tristate_off (LM_SUPER_DOG);
	task_exit ();
}

void superdog_score (void)
{
	generic_ladder_score_and_advance (&superdog_score_rule);
}

void superdog_start (void)
{
	if (superdog_timer == 0)
	{
		task_create_gid1 (GID_SUPERDOG_RUNNING, superdog_running);
	}
	superdog_timer += 20;
	if (superdog_timer > 60)
		superdog_timer = 60;
}

CALLSET_ENTRY (superdog, sw_superdog_low, sw_superdog_center, sw_superdog_high)
{
	if (lamp_flash_test (LM_SUPER_DOG))
	{
		superdog_score ();
	}
}

CALLSET_ENTRY (superdog, start_ball)
{
	superdog_timer = 0;
	lamp_tristate_off (LM_SUPER_DOG);
}

