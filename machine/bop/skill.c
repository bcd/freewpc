/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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

bool skill_kicker_enabled;
bool skillshot_enabled;

void skillshot_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "SKILL SHOT");
	sprintf_score (score_deff_get ());
	seg_write_row_center (1, sprintf_buffer);
	seg_sched_transition (&seg_trans_rtl);
	seg_show ();
	task_sleep_sec (1);
	skill_kicker_enabled = TRUE;
	deff_exit ();
}

static void award_skill_switch (U8 sw)
{
	set_valid_playfield ();
	
	if (skillshot_enabled == FALSE)
		return;
	switch (sw)
	{
		case 0:
			score (SC_50K);
			break;
		case 1:
			score (SC_75K);
			break;
		case 2:
			score (SC_100K);
			break;
		case 3:
			score (SC_200K);
			break;
		case 4:
			score (SC_25K);
			break;
	}
	deff_start (DEFF_SKILLSHOT);
	skillshot_enabled == FALSE;
}

CALLSET_ENTRY (skill, sw_skill_shot_50k)
{
	award_skill_switch (0);	
}

CALLSET_ENTRY (skill, sw_skill_shot_75k)
{
	award_skill_switch (1);	
}

CALLSET_ENTRY (skill, sw_skill_shot_100k)
{
	award_skill_switch (2);	
}

CALLSET_ENTRY (skill, sw_skill_shot_200k)
{
	award_skill_switch (3);	
}

CALLSET_ENTRY (skill, sw_skill_shot_25k)
{
	award_skill_switch (4);	
}

CALLSET_ENTRY (skill, idle_every_second)
{
	if (skill_kicker_enabled == TRUE 
		&& switch_poll (SW_SKILL_SHOT_50K))
	{	
		/* wait for the ball to settle */
		task_sleep (TIME_200MS);
		sol_request (SOL_SSHOT_KICKER);
	}

}

CALLSET_ENTRY (skill, ball_serve)
{
	skillshot_enabled = TRUE;
	skill_kicker_enabled = FALSE;
}

