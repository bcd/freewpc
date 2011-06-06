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

U8 sslot_mode_timer;
U8 sslot_award_index;
U8 sslot_award_index_stored;
extern U8 gumball_enable_count;
extern U8 mpf_enable_count;
extern U8 door_panels_started;
extern U8 cameras_lit;
extern bool skill_shot_enabled;

void sslot_mode_init (void);
void sslot_mode_exit (void);

struct timed_mode_ops sslot_mode = {
	DEFAULT_MODE,
	.init = sslot_mode_init,
	.exit = sslot_mode_exit,
	.gid = GID_SSLOT_MODE_RUNNING,
	.music = MUS_SUPER_SLOT,
	.deff_running = DEFF_SSLOT_MODE,
	.prio = PRI_GAME_MODE6,
	.init_timer = 20,
	.timer = &sslot_mode_timer,
	.grace_timer = 3,
	.pause = system_timer_pause,
};

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
	while  (in_live_game)
	{
		sslot_award_index++;
		if (sslot_award_index >= NUM_SSLOT_AWARDS - 1)
			sslot_award_index = 0;
		task_sleep (TIME_400MS);
	}

}

void sslot_mode_deff (void)
{
	U16 fno;
	dmd_alloc_pair_clean ();
	//while (timed_mode_running_p (&sslot_mode))
	for (;;)
	{
		for (fno = IMG_REELSTRIP_START; fno <= IMG_REELSTRIP_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
		
			font_render_string_center (&font_var5, 64, 5, "SHOOT SLOT MACHINE");
			sprintf_current_score ();
			font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
			font_render_string_center (&font_var5, 64, 27, sslot_award_names[sslot_award_index]);
			sprintf ("%d", sslot_mode_timer);
			font_render_string (&font_var5, 8, 16, sprintf_buffer);
			font_render_string_right (&font_var5, 120, 16, sprintf_buffer);
			
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
	deff_exit ();

}

void sslot_award_deff (void)
{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 5, "SLOT MACHINE AWARD");
		font_render_string_center (&font_var5, 64, 20, sslot_award_names[sslot_award_index_stored]);
		dmd_show_low ();
		task_sleep_sec (2);
		deff_exit ();
}

void sslot_mode_init (void)
{
	sslot_award_index = 0;
	task_recreate_gid (GID_SSLOT_AWARD_ROTATE, sslot_award_rotate);
	lamp_tristate_flash (LM_SLOT_MACHINE);
}

void sslot_mode_expire (void)
{
	callset_invoke (start_hurryup);
}

void sslot_mode_exit (void)
{	
	lamp_tristate_off (LM_SLOT_MACHINE);
	task_kill_gid (GID_SSLOT_AWARD_ROTATE);
}

void slot_kick_sound (void)
{
	sound_send (SND_SLOT_KICKOUT_2);
	task_exit ();
}

void sslot_award (void)
{
	sslot_award_index_stored = sslot_award_index;
	task_kill_gid (GID_SSLOT_AWARD_ROTATE);
	
	/* Don't allow awarding the eb if we can't do so */
	if (!can_award_extra_ball ())
	{
		/* Pick another until it's not an EB */
		while (sslot_award_index_stored == 0)
		{
			sslot_award_index_stored = random_scaled(5);
		}
	}

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
			if (!lamp_test (LM_PANEL_10M))
			{
				lamp_on (LM_PANEL_10M);
				door_panels_started++;
			}
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

static void shot_slot_task (void)
{
	callset_invoke (shot_slot_machine);
	task_exit ();
}

CALLSET_ENTRY (slot, slot_shot)
{
	
}
static void shot_sslot (void)
{
	sslot_award ();
	score (SC_10M);
	timed_mode_end (&sslot_mode);
}

static void shot_slot_door (void)
{
	flag_off (FLAG_SLOT_DOOR_LIT);
	flag_on (FLAG_PIANO_DOOR_LIT);
	callset_invoke (select_mode);
	callset_invoke (award_door_panel);
}

static void shot_slot_oddchange (void)
{
	score (SC_5130);
	if (check_relight_slot_or_piano ())
	{
		flag_on (FLAG_SLOT_DOOR_LIT);
		sound_send (SND_FEEL_LUCKY);
	}
	award_unlit_shot (SW_SLOT);
	callset_invoke (oddchange_collected);
}

CALLSET_ENTRY (slot, dev_slot_enter)
{
	if (task_kill_gid (GID_CAMERA_SLOT_PROX_DETECT)
		 || task_kill_gid (GID_PIANO_SLOT_PROX_DETECT))
	{
		/* Proximity sensor did not trip ; must be the powerball */
		pb_detect_event (PF_PB_DETECTED);
		pb_announce ();
	}

	if (!in_live_game)
		return;
	else if (task_find_or_kill_gid (GID_DEADEND_TO_SLOT)
	 	|| task_find_or_kill_gid (GID_GUMBALL_TO_SLOT)
		|| task_find_or_kill_gid (GID_PIANO_TO_SLOT)
		|| task_find_or_kill_gid (GID_CAMERA_TO_SLOT))
	{
		/* dead end was recently hit, so ignore slot */
		/* piano was recently hit, so ignore slot */
		/* camera was recently hit, so ignore slot */
	}
	else if (event_did_follow (skill_shot, slot)
		|| skill_shot_enabled
		|| global_flag_test (GLOBAL_FLAG_SSSMB_RUNNING))
	{
		/* TODO, this may be buggy during sssmb */
		/* skill shot has been missed or ball landed in plunger lane*/
		if (timer_find_gid (GID_SDSS_READY)) 
			deff_start (DEFF_SDSS_READY);
		callset_invoke (skill_missed);
	}
	else if (timed_mode_running_p (&sslot_mode))
	{
		//TODO If shot from lite slot lane, allow player to choose award
		shot_sslot ();
	}
	else if (can_award_door_panel () && flag_test (FLAG_SLOT_DOOR_LIT))
	{
		shot_slot_door ();
	}
	else 
	{
		shot_slot_oddchange ();
	}
	/* Sleep so the deffs can get a chance to start and stop it
	 * kicking out too early */
	task_sleep (TIME_400MS);
	
}

CALLSET_ENTRY (slot, dev_slot_kick_attempt)
{
	if (in_live_game)
	{
		task_sleep (TIME_200MS);
		while (kickout_locks > 0)
			task_sleep (TIME_500MS);
		/* start Slot kick -> STDM timer for combo.c */
		sound_send (SND_SLOT_KICKOUT_1);
		leff_start (LEFF_SLOT_KICKOUT);
		task_sleep (TIME_500MS);
		task_create_gid (0, slot_kick_sound);
		event_can_follow (slot_kick, outhole, TIME_1S);
	}
}

CALLSET_ENTRY (slot, display_update)
{
	timed_mode_display_update (&sslot_mode);
}

CALLSET_ENTRY (slot, music_refresh)
{
	timed_mode_music_refresh (&sslot_mode);
}

CALLSET_ENTRY (slot, door_start_sslot)
{
	timed_mode_begin (&sslot_mode);
}

CALLSET_ENTRY (slot, end_ball)
{
	timed_mode_end (&sslot_mode);
}

CALLSET_ENTRY (slot, start_player)
{
}

CALLSET_ENTRY (slot, ball_search)
{
	sol_request (SOL_SLOT);
}
