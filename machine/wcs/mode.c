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

#define MODE_STAMINA 0
#define MODE_SKILL 1
#define MODE_SPIRIT 2
#define MODE_SPEED 3
#define MODE_STRENGTH 4
#define NUM_MODES 5

#define MODE_TIMER 20

#define ALL_MODES_RUNNING 0x1F

__local__ U8 dog_targets;

/** The number of modes that have been started */
__local__ U8 modes_started;

__local__ U8 mode_ready;

/** A bitmask of the modes that are currently running */
U8 modes_running;

/** The timer for each of the modes */
U8 mode_timers[NUM_MODES];

/** The mode scoring multiplier */
U8 mode_multiplier;

/** The Super Dog mode timer; when nonzero, the mode is active */
U8 superdog_timer;


/** Awards mode points, possibly multiplied. */
void mode_score (const score_t score)
{
	score_long_multiple (score, mode_multiplier);
}


void mode_stamina_start (void)
{
	/* Requires looping either left/right ramp over and over */
}

void mode_skill_start (void)
{
	/* Hold onto ball at mode start.  Serve new ball to plunger.
	Disable flippers.  Shoot for the flashing skill lane.
	If made, ball drains and is re-served for another shot.
	If missed, flippers are re-enabled and either ramp shot
	will requalify (ball will drain again).  Timed game pauses
	while skill shots are active. */
}

void mode_spirit_start (void)
{
	/* Frenzy mode - all switches score */
}

void mode_speed_start (void)
{
	/* Hurry-Up mode: a countdown value is initialized
	 * and begins dropping.  Each shot made scores the value
	 * and adds to it for the next shot. */
}

void mode_strength_start (void)
{
	/* Shoot goal over and over, with Evil Goalie active if
	 * enabled */
}

bool mode_running_p (const U8 mode)
{
	return modes_running & (1 << mode);
}

typedef struct
{
	const char *name;
	void (*start) (void);
	sound_code_t say_name;
} mode_info_t;


mode_info_t mode_table[] = {
	{ "STAMINA", mode_stamina_start, SPCH_STAMINA },
	{ "SKILL", mode_skill_start, SPCH_SKILL },
	{ "SPIRIT", mode_spirit_start, SPCH_SPIRIT },
	{ "SPEED", mode_speed_start, SPCH_SPEED },
	{ "STRENGTH", mode_strength_start, SPCH_STRENGTH },
};


void mode_draw_target_status (U8 targets)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 48, 12, "D");
	font_render_string_center (&font_fixed10, 64, 12, "O");
	font_render_string_center (&font_fixed10, 80, 12, "G");
	dmd_flip_low_high ();

	dmd_clean_page_low ();
	if (targets & 1)
		font_render_string_center (&font_fixed10, 48, 12, "D");
	if (targets & 2)
		font_render_string_center (&font_fixed10, 64, 12, "O");
	if (targets & 4)
		font_render_string_center (&font_fixed10, 80, 12, "G");

	if (targets < 0x7)
	{
		font_render_string_center (&font_var5, 64, 27, "COMPLETE TO LIGHT MODE");
	}
	else
	{
		font_render_string_center (&font_var5, 64, 27, "MODE START IS LIT");
	}

	dmd_flip_low_high ();
	dmd_show2 ();

}

void mode_target_status_deff (void)
{
	mode_draw_target_status (dog_targets);
	task_sleep_sec (2);
	deff_exit ();
}

void unlit_striker_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 10, "SHOOT 3 DOG TARGETS");
	font_render_string_center (&font_var5, 64, 22, "TO LIGHT MODE START");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void mode_lit_deff (void)
{
	mode_draw_target_status (0x7);
	task_sleep_sec (3);
	deff_exit ();
}

void mode_started_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("MODE %d", modes_started);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	sprintf ("%s", mode_table[mode_ready].name);
	font_render_string_center (&font_fixed10, 64, 22, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void mode_finished_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 16, "MODE FINISHED");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void mode_reset_dog_targets (void)
{
	dog_targets = 0;
	flag_off (FLAG_MODE_LIT);
}

void mode_collect_dog_target (U8 target)
{
	if (superdog_timer)
	{
		return;
	}

	sound_start (ST_SAMPLE, SND_DRIBBLE, SL_500MS, PRI_GAME_QUICK1);
	if (!flag_test (FLAG_MODE_LIT))
	{
		dog_targets |= target;
		if (dog_targets == 0x7)
		{
			flag_on (FLAG_MODE_LIT);
			deff_start (DEFF_MODE_LIT);
			score (SC_100K);
		}
		else
		{
			deff_start (DEFF_MODE_TARGET_STATUS);
			score (SC_25K);
		}
	}
	else
	{
		/* Mode start is already lit; the value of the Dog targets
		depends on the number of modes actually running right now */
		score (SC_10K);
	}
}

static void mode_flash_next (void)
{
	dbprintf ("mode_ready = %d\n", mode_ready);
	lamp_flash_on (lamplist_index (LAMPLIST_BALL_PANELS, mode_ready));
}

void mode_flash_first (void)
{
	mode_ready = 0;
	mode_flash_next ();
}

void mode_rotate_next (void)
{
	U8 lamp;

	lamp_flash_off (lamplist_index (LAMPLIST_BALL_PANELS, mode_ready));
	for (;;)
	{
		mode_ready++;
		lamp = lamplist_index (LAMPLIST_BALL_PANELS, mode_ready);
		if (lamp == LAMP_END)
		{
			mode_ready = 0;
			lamp = lamplist_index (LAMPLIST_BALL_PANELS, mode_ready);
		}
		if (!lamp_test (lamp))
			break;
	}
	mode_flash_next ();
}


bool mode_can_be_started (void)
{
	return TRUE;
}

void mode_start (void)
{
	modes_running |= (1 << mode_ready);
	lamp_tristate_on (lamplist_index (LAMPLIST_BALL_PANELS, mode_ready));
	mode_table[mode_ready].start ();
	mode_timers[mode_ready] = MODE_TIMER;
	if (modes_started < NUM_MODES)
		mode_rotate_next ();
}


void mode_reset (U8 mode)
{
	if (mode_timers[mode] > 0)
	{
		mode_timers[mode] = MODE_TIMER;
	}
}


void superdog_start (void)
{
	superdog_timer = 30;
}


void maybe_mode_start (void)
{
	if (mode_can_be_started () && flag_test (FLAG_MODE_LIT))
	{
		mode_reset_dog_targets ();
		if (modes_started < NUM_MODES)
		{
			score (SC_100K);
			deff_start (DEFF_MODE_STARTED);
			task_sleep (TIME_16MS);
			modes_started++;
			mode_start ();
		}
		else
		{
			superdog_start ();
		}
	}
	else
	{
		score (SC_50K);
	}
}

CALLSET_ENTRY (mode, sw_striker_1)
{
	mode_collect_dog_target (0x1);
}

CALLSET_ENTRY (mode, sw_striker_2)
{
	mode_collect_dog_target (0x2);
}

CALLSET_ENTRY (mode, sw_striker_3)
{
	mode_collect_dog_target (0x4);
}

CALLSET_ENTRY (mode, striker_shot)
{
	maybe_mode_start ();
}

CALLSET_ENTRY (mode, start_player)
{
	flag_off (FLAG_SUPER_DOG_LIT);
	modes_started = 0;
	mode_reset_dog_targets ();
	lamplist_apply (LAMPLIST_BALL_PANELS, lamp_off);
	mode_flash_first ();
}

CALLSET_ENTRY (mode, lamp_update)
{
	lamp_flash_if (LM_STRIKER,
		mode_can_be_started () && flag_test (FLAG_MODE_LIT));
}

CALLSET_ENTRY (mode, sw_right_button)
{
	if (in_live_game)
		mode_rotate_next ();
}

CALLSET_ENTRY (mode, start_ball)
{
	modes_running = 0;
	mode_multiplier = 1;
	superdog_timer = 0;
}

