/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

static void common_jet_handler (void)
{
	sound_start (ST_SAMPLE, SND_JET_BUMPER, SL_500MS, PRI_GAME_QUICK4);
}

static void common_sling_handler (void)
{
	sound_start (ST_SAMPLE, SND_SLINGSHOT, SL_500MS, PRI_GAME_QUICK2);
}


CALLSET_ENTRY (pity, sw_left_jet)
{
	common_jet_handler ();
}

CALLSET_ENTRY (pity, sw_upper_jet)
{
	common_jet_handler ();
}

CALLSET_ENTRY (pity, sw_lower_jet)
{
	common_jet_handler ();
}

CALLSET_ENTRY (pity, sw_left_slingshot)
{
	common_sling_handler ();
}

CALLSET_ENTRY (pity, sw_right_slingshot)
{
	common_sling_handler ();
}

CALLSET_ENTRY (pity, sw_left_flipper_lane)
{
	sound_start (ST_SAMPLE, SND_INLANE, SL_500MS, PRI_GAME_QUICK2);
}

CALLSET_ENTRY (pity, sw_right_flipper_lane)
{
	sound_start (ST_SAMPLE, SND_INLANE, SL_500MS, PRI_GAME_QUICK2);
}

CALLSET_ENTRY (pity, bonus)
{
	sound_send (MUS_BONUS_BONG);
	task_sleep (TIME_500MS);
	sound_send (MUS_BONUS_BONG);
	task_sleep (TIME_300MS);
	sound_send (MUS_BONUS_BONG);
	task_sleep (TIME_300MS);
}

CALLSET_ENTRY (pity, any_pf_switch)
{
	score (SC_730);
}

CALLSET_ENTRY (pity, sw_spinner_slow)
{
	score (SC_10K);
}


CALLSET_ENTRY (pity, dev_left_eject_kick_attempt)
{
	sound_start (ST_SAMPLE, SND_FREE_KICK_LIT, SL_1S, PRI_GAME_QUICK3);
}

CALLSET_ENTRY (pity, dev_right_eject_kick_attempt)
{
	sound_start (ST_SAMPLE, SND_FREE_KICK_LIT, SL_1S, PRI_GAME_QUICK3);
}

CALLSET_ENTRY (pity, dev_upper_eject_kick_attempt)
{
	sound_start (ST_SAMPLE, SND_FREE_KICK_LIT, SL_1S, PRI_GAME_QUICK3);
}

CALLSET_ENTRY (pity, dev_tv_popper_kick_attempt)
{
	sound_start (ST_SAMPLE, SND_DRIBBLE, SL_1S, PRI_GAME_QUICK3);
}

CALLSET_ENTRY (pity, dev_goal_popper_kick_attempt)
{
	sound_start (ST_SAMPLE, SND_KICK, SL_1S, PRI_GAME_QUICK3);
}


CALLSET_ENTRY (pity, sw_skill_front)
{
}

CALLSET_ENTRY (pity, sw_skill_center)
{
	sound_start (ST_MUSIC, MUS_SKILL_SHOT_CITY, SL_1S, PRI_GAME_QUICK6);
	sound_start (ST_SPEECH, SND_CROWD_CHEER, SL_2S, PRI_GAME_QUICK6);
}

CALLSET_ENTRY (pity, sw_skill_rear)
{
}

CALLSET_ENTRY (pity, sw_rollover_1)
{
	sound_send (SND_DING1);
	callset_invoke (any_rollover);
}

CALLSET_ENTRY (pity, sw_rollover_2)
{
	sound_send (SND_DING2);
	callset_invoke (any_rollover);
}

CALLSET_ENTRY (pity, sw_rollover_3)
{
	sound_send (SND_DING3);
	callset_invoke (any_rollover);
}

CALLSET_ENTRY (pity, sw_rollover_4)
{
	sound_send (SND_DING4);
	callset_invoke (any_rollover);
}

CALLSET_ENTRY (pity, sw_shooter)
{
	/* Sound only on the closed->open transition */
	if (!switch_poll_logical (SW_SHOOTER))
	{
		sound_send (SND_PLUNGE);
	}
}

