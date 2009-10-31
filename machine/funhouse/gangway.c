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

__local__ U8 trap_door_bonuses;

const score_id_t trap_door_bonus_scores[] = {
	SC_250K, SC_500K, SC_750K
};
const struct generic_ladder trap_door_bonus_score_rule = {
	3,
	trap_door_bonus_scores,
	&trap_door_bonuses,
};

__local__ U8 gangway_count;

const score_id_t gangway_scores[] = {
	SC_75K, SC_100K, SC_150K, SC_200K, SC_250K
};
const struct generic_ladder gangway_score_rule = {
	5,
	gangway_scores,
	&gangway_count,
};


void gangway_collect_deff (void)
{
}

bool gangway_available_p (void)
{
	return !multiball_mode_running_p ();
}

void gangway_loop_lit (void)
{
	sample_start (SND_LOOP, SL_1S);
}

void gangway_loop_collected (void)
{
	generic_ladder_score_and_advance (&gangway_score_rule);
	sample_start (SND_WHEEEE, SL_1S);
}


static inline void gangway_shot (task_gid_t gid, task_gid_t other_gid)
{
	score (SC_50K);
	if (gangway_available_p ())
	{
		if (timer_find_gid (gid))
		{
			timer_kill_gid (gid);
			gangway_loop_collected ();
		}
		else
		{
			timer_kill_gid (other_gid);
			timer_start_free (gid, TIME_5S);
			gangway_loop_lit ();
		}
	}
}


static inline void gangway_light (task_gid_t gid, task_gid_t other_gid)
{
	if (gangway_available_p ())
	{
		timer_kill_gid (other_gid);
		timer_restart_free (gid, TIME_5S);
		gangway_loop_lit ();
	}
}


CALLSET_ENTRY (gangway, left_loop_shot)
{
	gangway_shot (GID_LEFT_GANGWAY_LIT, GID_RIGHT_GANGWAY_LIT);
}

CALLSET_ENTRY (gangway, right_loop_shot)
{
	gangway_shot (GID_RIGHT_GANGWAY_LIT, GID_LEFT_GANGWAY_LIT);
}

CALLSET_ENTRY (gangway, sw_left_inlane)
{
	gangway_light (GID_RIGHT_GANGWAY_LIT, GID_LEFT_GANGWAY_LIT);
}

CALLSET_ENTRY (gangway, sw_inner_right_inlane)
{
	gangway_light (GID_LEFT_GANGWAY_LIT, GID_RIGHT_GANGWAY_LIT);
}

CALLSET_ENTRY (gangway, lamp_update)
{
	lamp_on_if (LM_FLIPPER_LANES, gangway_available_p ());
	lamp_flash_if (LM_LEFT_GANGWAY, timer_find_gid (GID_LEFT_GANGWAY_LIT));
	lamp_flash_if (LM_RIGHT_GANGWAY, timer_find_gid (GID_RIGHT_GANGWAY_LIT));
}

CALLSET_ENTRY (gangway, start_player)
{
	lamplist_apply (LAMPLIST_GANGWAYS, lamp_off);
	lamp_tristate_flash (lamplist_index (LAMPLIST_GANGWAYS, 0));
	gangway_count = 0;
	trap_door_bonuses = 0;
}

