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
#include <bridge_open.h>
/** The number of balls enabled to go to the MPF */
__local__ U8 mpf_enable_count;

#define MAX_LIGHTNINGS 8
#define BITMAP_HEIGHT 15
#define TOTAL_BITMAP_SIZE 17

/** Number of balls currently on the mini-playfield */
U8 mpf_ball_count;
U8 mpf_timer;
U8 mpf_award;
/* How many times the player has hit the flipper buttons */
U8 masher_buttons_pressed;
U8 __local__ mpf_level;
bool mpf_active;

void mpf_mode_init (void);
void mpf_mode_exit (void);
void mpf_lamp_task (void);
//Order is low, high
const U8 lightning_bitmap_2plane[] = {
	8,15,0,0,128,64,32,16,0,128,67,32,16,8,4,2,1,
	8,15,128,192,96,48,24,12,254,127,60,24,12,6,3,1,0,
};

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


struct lightning_state
{
	U8 x;
	U8 y;
	U8 x_speed;
	U8 y_speed;
	bool alive;
	bool left;
} lightning_states[MAX_LIGHTNINGS];

/* Where the powerball is */
extern U8 pb_location;
extern U8 unlit_shot_count;

static void mpf_countdown_score_task (void)
{
	while (mpf_award > 5)
	{
		bounded_decrement (mpf_award, 0);
		task_sleep_sec (1);
	}
	task_exit ();
}

static void randomise_lightning (U8 lightning_number)
{
	struct lightning_state *l = &lightning_states[lightning_number];
	l->x = 54;
	l->x += random_scaled (20);
	l->y = 0;
	l->alive = FALSE;
	l->x_speed = random_scaled(3) + 1;
	l->y_speed = random_scaled(3) + 1;
	l->left = random_scaled(2);
}

static void init_lightning (void)
{
	U8 i;
	for (i = 0; i < MAX_LIGHTNINGS; i++)
	{
		randomise_lightning (i);
	}
}

static void spawn_lightning (bool left)
{
	U8 i;
	/* Find and resurrect the first dead lightning */
	for (i = 0; i < MAX_LIGHTNINGS; i++)
	{
		struct lightning_state *l = &lightning_states[i];
		if (l->alive == FALSE)
		{
			randomise_lightning (i);
			l->alive = TRUE;
			l->left = left;
			return;
		}
	}
}

static void move_lightning (U8 lightning_number)
{
	struct lightning_state *l = &lightning_states[lightning_number];
	if (l->y < 32 - BITMAP_HEIGHT + l->y_speed)
		l->y += l->y_speed;
	else
		l->alive = FALSE;
	
	if (l->x > l->x_speed && l->left)
		l->x -= l->x_speed;
	else if (l->x < 128 - 8 - l->x_speed)
		l->x += l->x_speed;
	else
		l->alive = FALSE;
}

static void draw_lightning (U8 lightning_number)
{
	struct lightning_state *l = &lightning_states[lightning_number];
	if (l->x > 128 || l->y > 32)
		return;
	bitmap_blit (lightning_bitmap_2plane, l->x, l->y);
	dmd_flip_low_high ();
	bitmap_blit (lightning_bitmap_2plane + TOTAL_BITMAP_SIZE, l->x, l->y);
	dmd_flip_low_high ();
}

void lightning_overlay (void)
{
	U8 i;
	for (i = 0; i < MAX_LIGHTNINGS; i++)
	{
		struct lightning_state *l = &lightning_states[i];
		if (l->alive == TRUE)
		{
			draw_lightning (i);
			move_lightning (i);
		}
	}
}

void mpf_mode_deff (void)
{
	//TODO Move this somewhere?
	U16 fno;
	dmd_alloc_pair_clean ();
	for (;;)
	{
		for (fno = IMG_PYRAMIDSPIN_START; fno <= IMG_PYRAMIDSPIN_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_center (&font_var5, 64, 5, "BATTLE THE POWER");
			sprintf ("%d,000,000", (mpf_award * mpf_level));
			font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
			sprintf ("SHOOT TOP HOLE TO COLLECT");
			font_render_string_center (&font_var5, 64, 27, sprintf_buffer);
			sprintf ("%d", mpf_timer);
			font_render_string (&font_var5, 2, 2, sprintf_buffer);
			font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
			dmd_text_outline ();

			dmd_alloc_pair ();
			frame_draw (fno);
			lightning_overlay ();
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
}

void mpf_award_deff (void)
{	
	dmd_alloc_pair_clean ();
	U16 fno;
	sound_send (SND_EXPLOSION_3);
	for (fno = IMG_EXPLODE_START; fno <= IMG_EXPLODE_END; fno += 2)
	{
		dmd_map_overlay ();
		dmd_clean_page_low ();
		
		if (fno > 4)
		{
			sprintf ("%d,000,000", (mpf_award * mpf_level));
			font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
			font_render_string_center (&font_var5, 64, 20, "AND SPOT DOOR PANEL");
		}
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
	dmd_alloc_pair_clean ();
	sprintf ("%d,000,000", (mpf_award * mpf_level));
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	font_render_string_center (&font_var5, 64, 20, "AND SPOT DOOR PANEL");
	dmd_copy_low_to_high ();
	dmd_show2 ();
	task_sleep (TIME_700MS);
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
	/* Start a task to pulse the magnets
	 * if a ball gets stuck */
	task_recreate_gid (GID_MPF_BALLSEARCH, mpf_ballsearch_task);	
	/* Start the hurryup */
	callset_invoke (start_hurryup);
}

void mpf_mode_init (void)
{
	init_lightning ();
}

void mpf_mode_exit (void)
{
	task_kill_gid (GID_MPF_COUNTDOWN);
	task_kill_gid (GID_MPF_COUNTDOWN_SCORE_TASK);
	task_kill_gid (GID_MPF_BALLSEARCH);
	task_kill_gid (GID_MPF_BUTTON_MASHER);
	task_kill_gid (GID_MUSIC_SPEED);
	mpf_active = FALSE;
	leff_stop (LEFF_MPF_ACTIVE);
}

void mpf_countdown_sound_task (void)
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

/* Whether we should allow going into the mpf */
bool mpf_ready_p (void)
{
	if (mpf_enable_count > 0
		/* Don't allow if PB might be on playfield */
		&& !global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY)
		&& !(pb_location & PB_MAYBE_IN_PLAY)
		/* Or during normal multiballs */
		&& !global_flag_test (GLOBAL_FLAG_MULTIBALL_RUNNING)
		&& !global_flag_test (GLOBAL_FLAG_QUICK_MB_RUNNING)
		&& !global_flag_test (GLOBAL_FLAG_CHAOSMB_RUNNING)
		&& !global_flag_test (GLOBAL_FLAG_SSSMB_RUNNING)
		/*  To make it easier to hit the hurryup the ball will be
		 *  dropped back to the upper left flipper */
		&& !hurryup_active ()
		/* TODO Probably want to remove this later on */
		&& !global_flag_test (GLOBAL_FLAG_BTTZ_RUNNING))
		return TRUE;
	else
		return FALSE;

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
		task_create_gid (GID_MPF_COUNTDOWN, mpf_countdown_sound_task);
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

/* Called from shots.c */
CALLSET_ENTRY (mpf, mpf_top_triggered)
{
	if (mpf_timer > 5)
		bounded_increment (mpf_timer, 10);
	spawn_lightning (random_scaled (2));
	leff_restart (LEFF_MPF_HIT);
	sound_send (SND_EXPLOSION_3);
	score (SC_500K);
}

/* Called from shots.c */
CALLSET_ENTRY (mpf, mpf_collected)
{
	/* Inform combo.c that the mpf was collected */
	task_create_anon (award_door_panel_task);
	task_kill_gid (GID_MPF_COUNTDOWN_SCORE_TASK);
	callset_invoke (combo_mpf_collected);
	bounded_decrement (mpf_ball_count, 0);
	/* Safe to here enable as it covers all cases */
	if (!in_bonus)
		flipper_enable ();
	score_multiple(SC_1M, (mpf_award * mpf_level));
	if (mpf_ball_count == 0)
	{
		timed_mode_end (&mpf_mode);
	}
	leff_start (LEFF_FLASHER_HAPPY);
	deff_start (DEFF_MPF_AWARD);
}

/* Called from shots.c */
CALLSET_ENTRY (mpf, mpf_entered)
{
	/* If tripped immediately after the right ramp opto, then a ball
	has truly entered the mini-playfield.  Note this may trip later
	on when a ball is already in play. */
	masher_buttons_pressed = 0;
	mpf_active = TRUE;
	unlit_shot_count = 0;
	mpf_award = 10;
	/* Increment the amount of balls in the mpf */
	bounded_increment (mpf_ball_count, feature_config.installed_balls);
	task_recreate_gid (GID_MPF_COUNTDOWN_SCORE_TASK, mpf_countdown_score_task);
	/* Add on 10 seconds for each extra ball */
//	if (mpf_ball_count > 1)
//		mpf_timer += 10;
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
		}
	}
}

/* Called from shots.c */
CALLSET_ENTRY (mpf, mpf_unexpected_ball)
{
	/* A ball sneaked in during multiball */
	sound_send (SND_WITH_THE_DEVIL);
	score (SC_5M);
	task_recreate_gid (GID_MPF_BALLSEARCH, mpf_ballsearch_task);	
}

/* Called from shots.c */
CALLSET_ENTRY (mpf, mpf_exited)
{
	/* Stop the ball search timer */
	task_kill_gid (GID_MPF_BALLSEARCH);
	task_kill_gid (GID_MPF_COUNTDOWN);
	task_kill_gid (GID_MPF_BUTTON_MASHER);
	bounded_decrement (mpf_ball_count, 0);
	
	if (mpf_ball_count == 0 && timed_mode_running_p (&mpf_mode))
	{
		if (single_ball_play ())
			leff_start (LEFF_FLASH_GI);
		timed_mode_end (&mpf_mode);
		score (SC_1M);
		sound_send (SND_HAHA_POWERFIELD_EXIT);
	}
	else if (mpf_ball_count == 0)
	{
		callset_invoke (start_hurryup);
	}
	flipper_enable ();
}

/* Button handlers */
CALLSET_ENTRY (mpf, sw_mpf_left)
{
	if (mpf_ball_count > 0)
	{
		/* TRUE refers to travelling left */
		spawn_lightning (TRUE);
		if (!task_find_gid (GID_MPF_COUNTDOWN))
			sound_send (SND_POWER_GRUNT_1);
		flasher_pulse (FLASH_JETS);
		score (SC_250K);
	}
}

CALLSET_ENTRY (mpf, sw_mpf_right)
{
	if (mpf_ball_count > 0)
	{
		spawn_lightning (FALSE);
		if (!task_find_gid (GID_MPF_COUNTDOWN))
			sound_send (SND_POWER_GRUNT_2);
		flasher_pulse (FLASH_JETS);
		score (SC_250K);
	}
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
	triac_disable (PINIO_GI_STRINGS);
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
	triac_enable (PINIO_GI_STRINGS);
	lamp_off (LM_MPF_1M);
	lamp_off (LM_MPF_750K);
	lamp_off (LM_MPF_500K);
	task_exit ();
}

/* A friend of mine likes to pummel the flipper buttons rather than actually use
 * skill to get the ball into the top, so I put this code in to stop him 
 * Pressing > 20 buttons in 3 seconds will trigger it
 *
 * */
static void check_button_masher (void)
{
	if (masher_buttons_pressed > 20)
	{
		timed_mode_end (&mpf_mode);
		deff_start (DEFF_BUTTON_MASHER);
		sound_send (SND_HAHA_POWERFIELD_EXIT);
	}
}

static void mpf_button_masher_handler (void)
{
	if (mpf_timer != 0 && !multi_ball_play ())
	{
		if (task_find_gid (GID_MPF_BUTTON_MASHER))
		{
			bounded_increment (masher_buttons_pressed, 254);
			check_button_masher ();
		}
		else 
		{
			masher_buttons_pressed = 0;
			timer_start_free (GID_MPF_BUTTON_MASHER, TIME_3S);
		}

	}
}

CALLSET_ENTRY (mpf, sw_left_button, sw_right_button)
{
	mpf_button_masher_handler ();
}

CALLSET_ENTRY (mpf, stop_game)
{
	mpf_active = FALSE;
}
