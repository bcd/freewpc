/*
 * Copyright 2010 by Ewan Meadows <sonny_jim@hotmail.com>
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


/* Combos and lucky bounces:
 * Put the event_can_follow where necessary but try and put
 * all the awards in here
 * There's a few in loop.c
 * */

/* CALLSET_SECTION (combo, __machine3__) */


#include <freewpc.h>

U8 two_way_combos;
U8 three_way_combos;
U8 lucky_bounces;

bool stdm_death;
bool unfair_death;

static void lucky_bounce (void)
{
	sound_send (SND_LUCKY);
	score (SC_1M);
	deff_start (DEFF_LUCKY_BOUNCE);
	bounded_increment (lucky_bounces, 99);
}

/* Left ramp, Right ramp, Piano combo and Left ramp, lock, camera combo */
CALLSET_ENTRY (combo, sw_left_ramp_exit)
{
	event_can_follow (left_ramp, right_ramp, TIME_5S);
	timer_restart_free (GID_L_RAMP_TO_LOCK, TIME_5S);
}

CALLSET_ENTRY (combo, dev_lock_enter)
{
	if (task_find_or_kill_gid (GID_L_RAMP_TO_LOCK))
		timer_restart_free (GID_L_RAMP_TO_LOCK_TO_CAMERA, TIME_4S);
}

CALLSET_ENTRY (combo, sw_right_ramp)
{
	if (event_did_follow (left_ramp, right_ramp))
		event_can_follow (left_right_ramp, piano, TIME_4S);
}

/* Jet Death + lucky bounce combo */
CALLSET_ENTRY (combo, sw_jet)
{
	if (in_live_game && !multi_ball_play ())
	{
		/* Jet Death*/
		event_can_follow (jets, either_outlane, TIME_600MS);
		/* Lucky Bounce */
		event_can_follow (jets, slot, TIME_700MS);
	}
}

/* Standup -> Slot lucky bounce combo */
CALLSET_ENTRY (combo, sw_standup_3)
{
	if (in_live_game && !multi_ball_play ())
	{
		event_can_follow (standup_3, slot_or_piano, TIME_700MS);
		event_can_follow (jets, either_outlane, TIME_600MS);
	}
}

CALLSET_ENTRY (combo, sw_standup_2)
{

	if (in_live_game && !multi_ball_play ())
	{
		event_can_follow (jets, either_outlane, TIME_600MS);
	}
}

/* Sling -> Slot Lucky bounce combo */
CALLSET_ENTRY (combo, sw_sling)
{
	if (in_live_game && !multi_ball_play ())
		event_can_follow (left_sling, slot, TIME_700MS);
}

/* Slot standup -> Camera lucky bounce */
CALLSET_ENTRY (combo, sw_standup_7)
{
	if (in_live_game && !multi_ball_play ())
		event_can_follow (slot_standup, camera, TIME_700MS);
}


/* Right Loop, Left+Right ramp -> Piano handlers */
CALLSET_ENTRY (combo, sw_piano)
{
//TODO Hack to remove piano.c
	device_switch_can_follow (piano, slot, TIME_3S + TIME_200MS);

	if (event_did_follow (right_loop, piano))
	{
		sound_send (SND_RUDY_BLEH);
		score (SC_10M);
		deff_start (DEFF_TWO_WAY_COMBO);
		bounded_increment (two_way_combos, 99);
	}
	else if (event_did_follow (left_right_ramp, piano))
	{
		sound_send (SND_THREE_WAY_COMBO);
		score (SC_20M);
		deff_start (DEFF_THREE_WAY_COMBO);
		bounded_increment (three_way_combos, 99);
	}
	else if (event_did_follow (standup_3, slot_or_piano))
		lucky_bounce ();
}

CALLSET_ENTRY (combo, sw_power_payoff)
{
	/* Whoops, missed */
	if (event_did_follow (left_right_ramp, piano))
		sound_send (SND_TOO_HOT_TO_HANDLE);
}

CALLSET_ENTRY (combo, sw_standup_4)
{
	/* Whoops, missed */
	if (event_did_follow (left_right_ramp, piano))
		sound_send (SND_TOO_HOT_TO_HANDLE);
}



/* Left loop, Right loop -> Camera handlers */
CALLSET_ENTRY (combo, award_right_loop)
{
	/* Lucky bounce combo */
	event_can_follow (right_loop, locked_ball, TIME_3S);
	/* 2 way combos */
	event_can_follow (right_loop, piano, TIME_2S);
}

CALLSET_ENTRY (combo, award_left_loop)
{
	event_can_follow (left_loop, hitchhiker, TIME_5S);
	event_can_follow (left_loop, camera, TIME_5S);
}

CALLSET_ENTRY (combo, sw_camera)
{
	if (event_did_follow (gumball_exit, camera))
	{
		return;
	}
	else if (event_did_follow (left_loop, camera))
	{
		score (SC_10M);
		sound_send (SND_LOOK_TO_THE_FUTURE);
		deff_start (DEFF_TWO_WAY_COMBO);
		bounded_increment (two_way_combos, 99);
	}
	else if (event_did_follow (right_loop, camera))
	{
		score (SC_10M);
		sound_send (SND_LOOK_TO_THE_FUTURE);
		deff_start (DEFF_TWO_WAY_COMBO);
		bounded_increment (two_way_combos, 99);
	}
	else if (event_did_follow (slot_standup, camera))
	{
		lucky_bounce ();
	}
		

}

/* Left loop -> Hitchhiker handlers */
CALLSET_ENTRY (combo, sw_hitchhiker)
{
	if (event_did_follow (left_loop, hitchhiker))
	{
		sound_send (SND_GOING_MY_WAY);
		score (SC_10M);
		deff_start (DEFF_TWO_WAY_COMBO);
		bounded_increment (two_way_combos, 99);
	}
}

/* Left ramp -> STDM */
CALLSET_ENTRY (combo, sw_left_ramp_enter)
{
	event_can_follow (stdm, outhole, TIME_2S);
}

/* Clock target -> STDM */
CALLSET_ENTRY (combo, sw_clock_target)
{
	event_can_follow (stdm, outhole, TIME_2S);
}

/* STDM handler */
CALLSET_ENTRY (combo, sw_outhole)
{
	if ((event_did_follow (slot_kick, outhole) || event_did_follow (stdm, outhole))
		&& !ballsave_test_active ())
		stdm_death = TRUE;
}

/* Jet death handler */
CALLSET_ENTRY (combo, sw_right_outlane)
{
	if (event_did_follow (jets, either_outlane) && !ballsave_test_active ())
		unfair_death = TRUE;
}

CALLSET_ENTRY (combo, sw_left_outlane)
{
	if (event_did_follow (jets, either_outlane) && !ballsave_test_active ())
		unfair_death = TRUE;
}

/* Standup3, Jet, Left Sling -> Slot Lucky Bounce handler */
CALLSET_ENTRY (combo, sw_slot)
{
	if (event_did_follow (standup_3, slot_or_piano) || 
		event_did_follow (jets, slot) ||
		event_did_follow (left_sling, slot))
	{
		lucky_bounce ();
	}
}

CALLSET_ENTRY (combo, start_ball)
{
	two_way_combos = 0;
	three_way_combos = 0;
	lucky_bounces = 0;
	stdm_death = FALSE;
	unfair_death = FALSE;
}
