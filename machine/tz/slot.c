/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
#include <eb.h>

U8 sslot_round_timer;
U8 sslot_award_index;
U8 sslot_award_index_stored;
extern U8 gumball_enable_count;
extern U8 mpf_enable_count;
extern U8 cameras_lit;

const char *sslot_award_names[] = {
	"EXTRA BALL",
	"LIGHT GUMBALL",
	"LIGHT POWER",
	"LIGHT CAMERA",
	"10 MILLION",
	"5 MILLION"
};

#define NUM_SSLOT_AWARDS 6

void sslot_award_rotate (void)
{
	for (;;)
	{
		if (in_live_game)
		{
		sslot_award_index++;
			if (sslot_award_index >= NUM_SSLOT_AWARDS - 1)
				sslot_award_index = 0;
		}
		task_sleep (TIME_700MS);
	}

}

void sslot_round_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 5, "SHOOT SLOT MACHINE");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, sslot_award_names[sslot_award_index]);
		sprintf ("%d", sslot_round_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
}

void sslot_award_deff (void)
{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 5, "SLOT MACHINE AWARD");
		font_render_string_center (&font_var5, 64, 27, sslot_award_names[sslot_award_index_stored]);
		dmd_show_low ();
		task_sleep_sec (2);
		deff_exit ();
}


void sslot_round_begin (void)
{
	sslot_award_index = 0;
	task_recreate_gid (GID_SSLOT_AWARD_ROTATE, sslot_award_rotate);
	lamp_tristate_flash (LM_SLOT_MACHINE);
}

void sslot_round_expire (void)
{
	deff_stop (DEFF_SSLOT_ROUND);
	task_kill_gid (GID_SSLOT_AWARD_ROTATE);
	lamp_tristate_off (LM_SLOT_MACHINE);
}

void sslot_round_end (void)
{	
	deff_stop (DEFF_SSLOT_ROUND);
	task_kill_gid (GID_SSLOT_AWARD_ROTATE);
	lamp_tristate_off (LM_SLOT_MACHINE);
}

void sslot_round_task (void)
{
	timed_mode_task (sslot_round_begin, sslot_round_expire, sslot_round_end,
		&sslot_round_timer, 20, 3);
}

void slot_kick_sound (void)
{
	sound_send (SND_SLOT_KICKOUT_2);
	task_exit ();
}

void sslot_award (void)
{
	sslot_award_index_stored = sslot_award_index;
	/* Stop round */
	task_kill_gid (GID_SSLOT_AWARD_ROTATE);
	
	switch (sslot_award_index_stored)
	{
		case 0:
			sound_send (SND_GET_THE_EXTRA_BALL);
			light_easy_extra_ball ();
			break;
		case 1:
			sound_send (SND_THIS_IS_NO_ORDINARY_GUMBALL);
			gumball_enable_count++;
			break;
		case 2:
			sound_send (SND_ARE_YOU_READY_TO_BATTLE);
			mpf_enable_count++;
			break;
		case 3:
			sound_send (SND_MOST_UNUSUAL_CAMERA);
			cameras_lit++;
			break;
		case 4:
			sound_send (SND_TEN_MILLION_POINTS);
			score (SC_10M);
			break;
		case 5:
			sound_send (SND_SEE_WHAT_GREED);
			score (SC_5M);
			break;
		default:
			score (SC_5M);
	}

	deff_start (DEFF_SSLOT_AWARD);
	sound_send (SND_SLOT_PAYOUT);
	lamp_tristate_off (LM_SLOT_MACHINE);
}

CALLSET_ENTRY (slot, dev_slot_enter)
{
	task_kill_gid (GID_SKILL_SWITCH_TRIGGER);
	set_valid_playfield ();
	score (SC_1K);

	if (event_did_follow (dead_end, slot))
	{
		/* dead end was recently hit, so ignore slot */
	}
	if (event_did_follow (gumball_exit, slot))
	{
		/* dead end was recently hit, so ignore slot */
	}
	else if (event_did_follow (piano, slot))
	{
		/* piano was recently hit, so ignore slot */
	}
	else if (event_did_follow (camera, slot))
	{
		/* camera was recently hit, so ignore slot */
	}
	else if (event_did_follow (skill_shot, slot))
	{
		/* skill switch was recently hit, so ignore slot */
		callset_invoke (skill_missed);
		deff_stop (DEFF_SKILL_SHOT_READY);
	}
	else if (timed_mode_timer_running_p (GID_SSLOT_ROUND_RUNNING, &sslot_round_timer))
	{
		sslot_award ();
		score (SC_10M);
		deff_stop (DEFF_SSLOT_ROUND);
		timed_mode_stop (&sslot_round_timer);
	}
	else
	{
		score (SC_50K);

		/* Tell door.c that the slot machine was hit */
		callset_invoke (shot_slot_machine);
	}
}


CALLSET_ENTRY (slot, dev_slot_kick_attempt)
{
	if (in_live_game)
	{
		sound_send (SND_SLOT_KICKOUT_1);
		leff_start (LEFF_SLOT_KICKOUT);
		task_sleep (TIME_100MS * 5);
		task_create_gid (0, slot_kick_sound);
		if (!multi_ball_play ())
			event_can_follow (slot_kick, outhole, TIME_1S + TIME_600MS);
	}
}

CALLSET_ENTRY (slot, display_update)
{
	if (timed_mode_timer_running_p (GID_SSLOT_ROUND_RUNNING,
		&sslot_round_timer))
		deff_start_bg (DEFF_SSLOT_ROUND, 0);
}

CALLSET_ENTRY (slot, music_refresh)
{
	if (timed_mode_timer_running_p (GID_SSLOT_ROUND_RUNNING,
		&sslot_round_timer))
		music_request (MUS_SUPER_SLOT, PRI_GAME_MODE1);
}

CALLSET_ENTRY (slot, door_start_sslot)
{
	timed_mode_start (GID_SSLOT_ROUND_RUNNING, sslot_round_task);
}

CALLSET_ENTRY (slot, end_ball)
{
	timed_mode_stop (&sslot_round_timer);
}

CALLSET_ENTRY (slot, start_player)
{
}

CALLSET_ENTRY (slot, ball_search)
{
	sol_request (SOL_SLOT);
}
