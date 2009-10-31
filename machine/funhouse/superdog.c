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
	SC_200K, SC_225K, SC_250K, SC_275K, SC_300K
};
const struct generic_ladder superdog_score_rule = {
	5,
	superdog_scores,
	&superdog_level
};

void superdog_running_deff (void)
{
	for (;;)
	{
		seg_alloc_clean ();
		seg_write_string (0, 0, "SUPERDOG");
		sprintf ("%d SEC.", superdog_timer);
		seg_write_row_right (0, sprintf_buffer);
		sprintf_current_score ();
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep (TIME_166MS);
	}
	deff_exit ();
}

void superdog_score_deff (void)
{
	seg_alloc_clean ();
	seg_show ();
	seg_write_row_center (0, "SUPERDOG");
	sprintf_score (score_deff_get ());
	seg_write_row_center (1, sprintf_buffer);
	seg_sched_transition (&seg_trans_rtl);
	seg_show ();
	task_sleep_sec (1);
	deff_exit ();
}

void superdog_running (void)
{
	lamp_tristate_flash (LM_SUPER_DOG);
	while (superdog_timer > 0)
	{
		/* TODO - timer did not pause after ball was locked
		and another served to the plunger.  When it was
		counting down, I saw that the ballsave lamp is on too.
		Also it is counting when ball is in tunnel a bit */
		timer_pause_second ();
		superdog_timer--;
	}
	task_sleep_sec (1);
	lamp_tristate_off (LM_SUPER_DOG);
	effect_update_request ();
	task_sleep (TIME_1500MS); /* grace period */
	task_exit ();
}

void superdog_score (void)
{
	generic_ladder_score_and_advance (&superdog_score_rule);
	sample_start (SND_CHOMP, SL_1S);
	deff_start (DEFF_SUPERDOG_SCORE);
	leff_start (LEFF_SUPERDOG_SCORE);
}

void superdog_start (void)
{
	if (superdog_timer == 0)
	{
		generic_ladder_reset (&superdog_score_rule);
		task_create_gid1 (GID_SUPERDOG_RUNNING, superdog_running);
	}
	superdog_timer += 20;
	if (superdog_timer > 60)
		superdog_timer = 60;
}

CALLSET_ENTRY (superdog, display_update)
{
	if (lamp_flash_test (LM_SUPER_DOG))
	{
		deff_start_bg (DEFF_SUPERDOG_RUNNING, 0);
	}
}

CALLSET_ENTRY (superdog, sw_superdog_low, sw_superdog_center, sw_superdog_high)
{
	if (task_find_gid (GID_SUPERDOG_RUNNING))
	{
		superdog_score ();
	}
}

CALLSET_ENTRY (superdog, start_ball, end_ball)
{
	superdog_timer = 0;
	lamp_tristate_off (LM_SUPER_DOG);
}

