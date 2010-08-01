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

/* Strange order as 25K is treated as last */
const sound_code_t skill_sounds[] = {
	SND_SKILL_SHOT_2, SND_SKILL_SHOT_3, 
	SND_SKILL_SHOT_4, SND_SKILL_SHOT_5, SND_SKILL_SHOT_1
};

void skillshot_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "SKILL SHOT");
	sprintf_score (score_deff_get ());
	seg_write_row_center (1, sprintf_buffer);
	seg_sched_transition (&seg_trans_rtl);
	seg_show ();
	task_sleep_sec (3);
	kickout_unlock (KLOCK_DEFF);
	deff_exit ();
}

CALLSET_ENTRY (skill, enable_skill_shot)
{
	flag_on (FLAG_SKILLSHOT_ENABLED);
	//leff_start (LEFF_SKILLSHOT);
}

static void award_skill_switch (U8 sw)
{
	set_valid_playfield ();
	if (task_find_gid (GID_SKILL_DEBOUNCE)
		|| !flag_test (FLAG_SKILLSHOT_ENABLED))
		return;
	
	timer_restart_free (GID_SKILL_DEBOUNCE, TIME_10S);
	kickout_lock (KLOCK_DEFF);
	deff_start (DEFF_SKILLSHOT);

	sound_send (skill_sounds[sw]);
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
			sound_send (SND_YAHOO);
			break;
		case 4:
			score (SC_25K);
			break;
	}
	flag_off (FLAG_SKILLSHOT_ENABLED);
	//sol_request (SOL_SSHOT_KICKER);
}

CALLSET_ENTRY (skill, ball_search)
{
	if (switch_poll_logical (SW_SKILL_SHOT_50K))
		sol_request (SOL_SSHOT_KICKER);
}

/* TODO Bug workaround */
CALLSET_ENTRY (skill, start_ball)
{
	flag_on (FLAG_SKILLSHOT_ENABLED);
	task_kill_gid (GID_SKILL_DEBOUNCE);
}

CALLSET_ENTRY (skill, serve_ball)
{
	flag_on (FLAG_SKILLSHOT_ENABLED);
	task_kill_gid (GID_SKILL_DEBOUNCE);
}

CALLSET_ENTRY (skill, end_ball)
{
	flag_off (FLAG_SKILLSHOT_ENABLED);
}

CALLSET_ENTRY (skill, sw_shooter)
{
	if (flag_test (FLAG_SKILLSHOT_ENABLED))
		sound_send (SND_SKILL_SHOT_LAUNCH);
}

/* Switch handlers */
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
