/*
 * Copyright 2006, 2007, 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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
//TODO Pressing extra ball during mpf could do something
#include <freewpc.h>
#include <bridge_open.h>
/** The number of balls enabled to go to the MPF */
__local__ U8 mpf_enable_count;

/** Number of balls currently on the mini-playfield */
U8 mpf_ball_count;
U8 mpf_timer;
U8 mpf_award;
U8 mpf_buttons_pressed;
U8 __local__ mpf_level;
bool mpf_active;

void mpf_mode_init (void);
void mpf_mode_exit (void);
void mpf_lamp_task (void);

struct timed_mode_ops mpf_mode = {
	DEFAULT_MODE,
	.init = mpf_mode_init,
	.exit = mpf_mode_exit,
	.gid = GID_MPF_MODE_RUNNING,
	.music = MUS_POWERFIELD,
	.deff_running = DEFF_MPF_MODE,
	.prio = PRI_GAME_MODE8,
	.init_timer = 15,
	.timer = &mpf_timer,
	.grace_timer = 3,
	.pause = null_false_function,
};

/* Where the powerball is */
extern U8 pb_location;
extern U8 unlit_shot_count;
void mpf_mode_deff (void)
{
	mpf_award = 10;
	for (;;)
	{
		
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 5, "BATTLE THE POWER");
		//sprintf_current_score ();
		sprintf ("%d,000,000", (mpf_award * mpf_level));
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		//sprintf ("%d,000,000", (mpf_award * mpf_level));
		sprintf ("SHOOT TOP HOLE TO COLLECT");
		font_render_string_center (&font_var5, 64, 27, sprintf_buffer);
		sprintf ("%d", mpf_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_show_low ();
		task_sleep_sec (1);
		if (mpf_award > 5)
			bounded_decrement (mpf_award, 0);
	}
}

void mpf_award_deff (void)
{
	dmd_alloc_low_clean ();

	sprintf ("%d,000,000", (mpf_award * mpf_level));
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	font_render_string_center (&font_var5, 64, 20, "AND SPOT DOOR PANEL");
	dmd_show_low ();
	sound_send (SND_EXPLOSION_3);
	task_sleep_sec (1);
	deff_exit ();
}

/* Task to pulse the mpf magnets
 * Gets killed when a ball exits */
void mpf_ballsearch_task (void)
{
	U8 i = 0;
	while (mpf_ball_count > 0 && i < 3)
	{
		task_sleep_sec (5);
		sol_request (SOL_MPF_RIGHT_MAGNET);
		task_sleep (TIME_500MS);
		sol_request (SOL_MPF_LEFT_MAGNET);
		task_sleep (TIME_500MS);
		sol_request (SOL_MPF_RIGHT_MAGNET);
		task_sleep (TIME_500MS);
		sol_request (SOL_MPF_LEFT_MAGNET);
		task_sleep (TIME_500MS);
		i++;
	}
	task_exit ();
		
}

void mpf_mode_expire (void)
{
	mpf_active = FALSE;
	leff_stop (LEFF_MPF_ACTIVE);
	/* Start a task to pulse the magnets
	 * if a ball gets stuck */
	task_recreate_gid (GID_MPF_BALLSEARCH, mpf_ballsearch_task);	
}

void mpf_mode_init (void)
{
}

void mpf_mode_exit (void)
{
	mpf_active = FALSE;
	leff_stop (LEFF_MPF_ACTIVE);
}

void mpf_countdown_task (void)
{
	sound_send (SND_FIVE);
	task_sleep_sec (1);
	sound_send (SND_FOUR);
	task_sleep_sec (1);
	sound_send (SND_THREE);
	task_sleep_sec (1);
	sound_send (SND_TWO);
	task_sleep_sec (1);
	sound_send (SND_ONE);
	task_exit ();
}

bool mpf_ready_p (void)
{
	return (mpf_enable_count > 0)
		/* Don't allow if PB might be on playfield */
		&& !flag_test (FLAG_POWERBALL_IN_PLAY)
		&& !(pb_location & PB_MAYBE_IN_PLAY)
		&& !flag_test (FLAG_MULTIBALL_RUNNING)
		&& !flag_test (FLAG_QUICK_MB_RUNNING)
		&& !flag_test (FLAG_BTTZ_RUNNING)
		&& !flag_test (FLAG_CHAOSMB_RUNNING)
		&& !flag_test (FLAG_SSSMB_RUNNING);

}

CALLSET_ENTRY (mpf, display_update)
{
	timed_mode_display_update (&mpf_mode);
}

CALLSET_ENTRY (mpf, music_refresh)
{
	timed_mode_music_refresh (&mpf_mode);
	/* Start a countdown task */	
	if (mpf_timer == 5 && !task_find_gid (GID_MPF_COUNTDOWN))
		task_create_gid (GID_MPF_COUNTDOWN, mpf_countdown_task);
}

CALLSET_ENTRY (mpf, end_ball)
{
}


CALLSET_ENTRY (mpf, lamp_update)
{
	if (mpf_ready_p ())
		lamp_tristate_on (LM_RAMP_BATTLE);
	else
		lamp_tristate_off (LM_RAMP_BATTLE);
}

CALLSET_ENTRY (mpf, door_start_battle_power)
{
	mpf_enable_count++;
}

/* Closing this switch does not imply that the
 * mpf award was collected, but we can tell camera.c
 * to expect a ball coming from the mpf */
CALLSET_ENTRY (mpf, sw_mpf_top)
{
	if (mpf_timer > 5)
		bounded_increment (mpf_timer, 10);
	event_should_follow (mpf_top, camera, TIME_4S);
	leff_restart (LEFF_MPF_HIT);
	sound_send (SND_EXPLOSION_3);
	score (SC_500K);
}

/* Called from camera.c */
CALLSET_ENTRY (mpf, mpf_collected)
{
	/* Inform combo.x that the mpf was collected */
	callset_invoke (combo_mpf_collected);
	bounded_decrement (mpf_ball_count, 0);
	/* Safe to here enable as it covers all cases */
	flipper_enable ();
	leff_start (LEFF_FLASHER_HAPPY);
	score_multiple(SC_1M, (mpf_award * mpf_level));
	if (mpf_ball_count == 0)
	{
		mpf_active = FALSE;
		timed_mode_end (&mpf_mode);
		task_kill_gid (GID_MPF_COUNTDOWN);
		task_kill_gid (GID_MPF_BALLSEARCH);
	}
	deff_start (DEFF_MPF_AWARD);
	callset_invoke (award_door_panel);
}

void pulse_mpf_magnets_task (void)
{
	U8 i = 0;
	while (mpf_ball_count > 0 && i < 10)
	{
		sol_request (SOL_MPF_RIGHT_MAGNET);
		task_sleep (TIME_200MS);
		sol_request (SOL_MPF_LEFT_MAGNET);
		task_sleep (TIME_200MS);
	}
	task_exit ();
}

CALLSET_ENTRY (mpf, sw_mpf_enter)
{
	/* If tripped immediately after the right ramp opto, then a ball
	has truly entered the mini-playfield.  Note this may trip later
	on when a ball is already in play. */
	if (event_did_follow (right_ramp, mpf_enter))
	{
		mpf_buttons_pressed = 0;
		mpf_active = TRUE;
		unlit_shot_count = 0;
		bounded_increment (mpf_ball_count, feature_config.installed_balls);
		/* Add on 10 seconds for each extra ball */
		if (mpf_ball_count > 1)
			mpf_timer += 10;
		bounded_increment (mpf_level, 10);
		bounded_decrement (mpf_enable_count, 0);
		if ((mpf_ball_count = 1))
		{	
			timed_mode_begin (&mpf_mode);
			if (!multi_ball_play ())
			{
				/* Turn off GI and start lamp effect */
				task_create_gid (GID_MPF_LAMP_TASK, mpf_lamp_task);
				flipper_disable ();
				bridge_open_stop ();
			}
		}
	}
	/* A ball sneaked in during multiball */
	else if (multi_ball_play ())
	{
		sound_send (SND_WITH_THE_DEVIL);
		score (SC_5M);
		task_create_gid (GID_PULSE_MPF_MAGNETS, pulse_mpf_magnets_task);
	}
}

CALLSET_ENTRY (mpf, sw_mpf_exit)
{
	/* Stop the ball search timer */
	task_kill_gid (GID_MPF_BALLSEARCH);
	task_kill_gid (GID_MPF_COUNTDOWN);
	if (mpf_ball_count > 0)
		bounded_decrement (mpf_ball_count, 0);
	if (mpf_ball_count == 0)
	{
		mpf_active = FALSE;
		leff_start (LEFF_FLASH_GI);
		timed_mode_end (&mpf_mode);
		score (SC_5M);
		/* This should be fine as we only disable in single ball play */
		flipper_enable ();
	}
	sound_send (SND_HAHA_POWERFIELD_EXIT);
}

CALLSET_ENTRY (mpf, sw_mpf_left)
{
//	magnet_enable_catch_and_throw (MAG_LEFT);
	if (mpf_ball_count > 0)
	{
		if (!task_find_gid (GID_MPF_COUNTDOWN))
			sound_send (SND_POWER_GRUNT_1);
		flasher_pulse (FLASH_JETS);
		score (SC_250K);
	}
}

CALLSET_ENTRY (mpf, sw_mpf_right)
{
	//magnet_enable_catch_and_throw (MAG_RIGHT);
	if (mpf_ball_count > 0)
	{
		if (!task_find_gid (GID_MPF_COUNTDOWN))
			sound_send (SND_POWER_GRUNT_2);
		flasher_pulse (FLASH_JETS);
		score (SC_250K);
	}
}


CALLSET_ENTRY (mpf, sw_right_ramp)
{
	/* If the mini-playfield is enabled, open the ramp
	divertor fully.  The ordinary catch and drop is bypassed.
	We should only allow the follow event if the mpf is ready
	otherwise the powerball can slip through and start the mpf */
	if (mpf_ready_p ())
		event_should_follow (right_ramp, mpf_enter, TIME_3S);
}

CALLSET_ENTRY (mpf, start_player)
{
	mpf_enable_count = 1;
	mpf_level = 1;
	mpf_active = FALSE;
}

CALLSET_ENTRY (mpf, ball_search)
{
	sol_request (SOL_MPF_LEFT_MAGNET);
	task_sleep_sec (1);
	sol_request (SOL_MPF_RIGHT_MAGNET);
}

void mpf_lamp_task (void)
{
	triac_disable (TRIAC_GI_MASK);
	triac_enable (GI_POWERFIELD);
	while (mpf_active)
	{
		lamp_off (LM_MPF_1M);
		lamp_on (LM_MPF_500K);
		task_sleep (TIME_100MS);
		lamp_off (LM_MPF_500K);
		lamp_on (LM_MPF_750K);
		task_sleep (TIME_100MS);
		lamp_off (LM_MPF_750K);
		lamp_on (LM_MPF_1M);
		task_sleep (TIME_100MS);
	}
	triac_enable (TRIAC_GI_MASK);
	lamp_off (LM_MPF_1M);
	lamp_off (LM_MPF_750K);
	lamp_off (LM_MPF_500K);
	task_exit ();
}

void check_button_masher (void)
{
	if (mpf_buttons_pressed > 40)
	{
		mpf_active = FALSE;
		deff_start (DEFF_BUTTON_MASHER);
		sound_send (SND_HAHA_POWERFIELD_EXIT);
	}
}

static mpf_button_masher_handler (void)
{
	if (mpf_timer != 0 && !multi_ball_play ())
	{
		if (task_find_gid (GID_MPF_BUTTON_MASHER))
		{
			bounded_increment (mpf_buttons_pressed, 254);
			check_button_masher ();
		}
		else 
		{
			mpf_buttons_pressed = 0;
			timer_start_free (GID_MPF_BUTTON_MASHER, TIME_4S);
		}

	}
}

CALLSET_ENTRY (mpf, sw_left_button)
{
	mpf_button_masher_handler ();
}

CALLSET_ENTRY (mpf, sw_right_button)
{
	mpf_button_masher_handler ();
}
