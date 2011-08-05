/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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
#include <timedmode.h>

/* Shot table */

enum shot_id {
	S_LEFT_LOOP = 0,
	S_LEFT_RAMP,
	S_CENTER_RAMP,
	S_RIGHT_RAMP,
	S_RIGHT_LOOP,
	S_COUNT,
};

/* Jackpots */

__local__ score_t jackpot_value[S_COUNT];

void jackpot_reset (void)
{
	lamplist_apply (LAMPLIST_JACKPOTS, lamp_off);
	lamplist_apply (LAMPLIST_JACKPOTS, lamp_flash_on);
}

void jackpot_shot (enum shot_id id)
{
	U8 lamp = lamplist_index (LAMPLIST_JACKPOTS, id);
	if (lamp_flash_test (lamp))
	{
		deff_start (DEFF_JACKPOT);
		lamp_flash_off (lamp);
		lamp_on (lamp);
	}
}

CALLSET_ENTRY (jackpot, left_loop_shot) { jackpot_shot (S_LEFT_LOOP); }
CALLSET_ENTRY (jackpot, left_ramp_shot) { jackpot_shot (S_LEFT_RAMP); }
CALLSET_ENTRY (jackpot, center_ramp_shot) { jackpot_shot (S_CENTER_RAMP); }
CALLSET_ENTRY (jackpot, right_ramp_shot) { jackpot_shot (S_RIGHT_RAMP); }
CALLSET_ENTRY (jackpot, right_loop_shot) { jackpot_shot (S_RIGHT_LOOP); }

CALLSET_ENTRY (jackpot, start_player)
{
	jackpot_reset ();
}

/* Playfield Multiplier */

U8 shot_multiplier;
U8 ball_count_multiplier;
U8 pf_mult_count[S_COUNT];

U8 pf_mult_timer;

struct timed_mode_ops pf_mult_mode = {
	DEFAULT_MODE,
	.gid = GID_PF_MULT_MODE,
	.timer = &pf_mult_timer,
	.init_timer = 15,
	.grace_timer = 3,
};

const U8 pf_lamplists[S_COUNT] = {
	LAMPLIST_L_LOOP_COUNT,
	LAMPLIST_L_RAMP_COUNT,
	LAMPLIST_LOCKS,
	LAMPLIST_R_RAMP_COUNT,
	LAMPLIST_R_LOOP_COUNT,
};

void pf_mult_update (void)
{
	score_multiplier_set (shot_multiplier * ball_count_multiplier);
}

void pf_mult_shot_reset (enum shot_id id)
{
	pf_mult_count[id] = 0;
	lamplist_apply (pf_lamplists[id], lamp_off);
	lamplist_apply (pf_lamplists[id], lamp_flash_off);
	lamp_flash_on (lamplist_index (pf_lamplists[id], 0));
}

void pf_mult_shot (enum shot_id id)
{
	if (pf_mult_count[id] < 3)
	{
		lamp_tristate_on (lamplist_index (pf_lamplists[id], pf_mult_count[id]));
		pf_mult_count[id]++;
		if (pf_mult_count[id] < 3)
			lamp_flash_on (lamplist_index (pf_lamplists[id], pf_mult_count[id]));
		else
			timed_mode_begin (&pf_mult_mode);
	}
}

CALLSET_ENTRY (pf_mult, left_loop_shot) { pf_mult_shot (S_LEFT_LOOP); }
CALLSET_ENTRY (pf_mult, left_ramp_shot) { pf_mult_shot (S_LEFT_RAMP); }
CALLSET_ENTRY (pf_mult, center_ramp_shot) { pf_mult_shot (S_CENTER_RAMP); }
CALLSET_ENTRY (pf_mult, right_ramp_shot) { pf_mult_shot (S_RIGHT_RAMP); }
CALLSET_ENTRY (pf_mult, right_loop_shot) { pf_mult_shot (S_RIGHT_LOOP); }

CALLSET_ENTRY (pf_mult, start_ball, end_ball)
{
	shot_multiplier = ball_count_multiplier = 1;
	pf_mult_update ();
}

CALLSET_ENTRY (pf_mult, start_player)
{
	memset (pf_mult_count, 0, sizeof (pf_mult_count));
}

CALLSET_ENTRY (pf_mult, ball_count_change)
{
	ball_count_multiplier = live_balls;
	pf_mult_update ();
}

/* Add Time */

__local__ U8 add_time_level;

void add_time_reset (void)
{
	lamplist_apply (LAMPLIST_MARTIANS, lamp_off);
	lamplist_apply (LAMPLIST_MARTIANS, lamp_flash_on);
}

void add_time_award (void)
{
	add_time_level++;
	add_time_reset ();
}

void add_time_martian (U8 id)
{
	callset_invoke (any_martian_target);
	if (lamp_flash_test (id))
	{
		lamp_tristate_on (id);
		if (lamplist_test_all (LAMPLIST_MARTIANS, lamp_test))
		{
			add_time_award ();
			deff_start (DEFF_MARTIAN_SPELLED);
		}
		else
		{
			deff_start (DEFF_MARTIAN_ADVANCE);
		}
	}
}

CALLSET_ENTRY (add_time, sw_martian_1) { add_time_martian (LM_MARTIAN_1); }
CALLSET_ENTRY (add_time, sw_martian_2) { add_time_martian (LM_MARTIAN_2); }
CALLSET_ENTRY (add_time, sw_martian_3) { add_time_martian (LM_MARTIAN_3); }
CALLSET_ENTRY (add_time, sw_martian_4) { add_time_martian (LM_MARTIAN_4); }
CALLSET_ENTRY (add_time, sw_martian_5) { add_time_martian (LM_MARTIAN_5); }
CALLSET_ENTRY (add_time, sw_martian_6) { add_time_martian (LM_MARTIAN_6); }
CALLSET_ENTRY (add_time, sw_martian_7) { add_time_martian (LM_MARTIAN_7); }

CALLSET_ENTRY (add_time, start_player)
{
	add_time_level = 0;
	add_time_reset ();
}

/* Stroke of Luck */

__local__ U8 luck_count_lit;
__local__ U8 luck_count_given;

void lower_lane_score (U8 id)
{
	lamp_on (id);
	if (lamplist_test_all (LAMPLIST_BOTTOM_LANES, lamp_test))
	{
		lamplist_apply (LAMPLIST_BOTTOM_LANES, lamp_off);
		luck_count_lit++;
	}
}

CALLSET_ENTRY (luck, sw_left_outlane) { lower_lane_score (LM_LEFT_OUTLANE); }
CALLSET_ENTRY (luck, sw_left_inlane) { lower_lane_score (LM_LEFT_INLANE); }
CALLSET_ENTRY (luck, sw_right_inlane) { lower_lane_score (LM_RIGHT_INLANE); }
CALLSET_ENTRY (luck, sw_right_outlane) { lower_lane_score (LM_RIGHT_OUTLANE); }
CALLSET_ENTRY (luck, sw_left_button)
{ lamplist_rotate_previous (LAMPLIST_BOTTOM_LANES, matrix_lookup (LMX_DEFAULT)); }
CALLSET_ENTRY (luck, sw_right_button)
{ lamplist_rotate_next (LAMPLIST_BOTTOM_LANES, matrix_lookup (LMX_DEFAULT)); }

CALLSET_ENTRY (luck, right_hole_front_shot)
{
	if (luck_count_lit)
	{
		luck_count_lit--;
		luck_count_given++;
	}
}

CALLSET_ENTRY (luck, start_player)
{
	luck_count_lit = luck_count_given = 0;
}

CALLSET_ENTRY (luck, lamp_update)
{
	lamp_flash_if (LM_STROKE_OF_LUCK, luck_count_lit);
}


/* Multiball */

#define MB_MAX_BALLS 4

__local__ U8 mb_count;
__local__ U8 mb_target_level;
U8 mb_targets_left;
score_t mb_super_value;

struct fixed_ladder mb_super_score_rule = {
	.base = score_table[SC_500K],
	.increment = score_table[SC_25K],
	.max = score_table[SC_1M],
	.current = mb_super_value,
};

bool mb_running_p (void)
{
	return live_balls > 1;
}

void mb_super_reset (void)
{
	mb_target_level = 5;
	mb_targets_left = mb_target_level;
}

void mb_super_award (void)
{
	score_long (mb_super_value);
}

void mb_add_ball (void)
{
	if (live_balls < MB_MAX_BALLS)
		serve_ball_auto ();
}

CALLSET_ENTRY (mb, sw_motor_bank_1, sw_motor_bank_2, sw_motor_bank_3)
{
	if (!mb_running_p ())
	{
		deff_start (DEFF_MB_INCREASE_JACKPOT);
	}
}

CALLSET_ENTRY (mb, sw_left_saucer_target, sw_right_saucer_target)
{
	if (mb_running_p ())
	{
	}
}

CALLSET_ENTRY (mb, drop_target_shot)
{
	if (mb_running_p ())
	{
	}
}

CALLSET_ENTRY (mb, dev_left_hole_enter)
{
	if (mb_running_p ())
	{
		if (mb_targets_left == 0)
			mb_super_award ();
	}
}

CALLSET_ENTRY (mb, single_ball_play)
{
}

CALLSET_ENTRY (mb, start_player)
{
	mb_count = 0;
	mb_super_reset ();
}

CALLSET_ENTRY (mb, display_update)
{
	if (mb_running_p ())
		deff_start_bg (DEFF_MB_RUNNING, 0);
}

/* Martian Attack */

__local__ U8 attack_lit;
__local__ U16 attack_hits;

U8 attack_timer;
struct timed_mode_ops attack_mode = {
	DEFAULT_MODE,
	.gid = GID_ATTACK_MODE,
	.deff_running = DEFF_ATTACK_RUNNING,
	.timer = &attack_timer,
	.init_timer = 20,
	.grace_timer = 4,
};

void attack_light (void)
{
	attack_lit++;
	deff_start (DEFF_ATTACK_LIT);
}

CALLSET_ENTRY (attack, any_martian_target)
{
	if (timed_mode_running_p (&attack_mode))
	{
		deff_start (DEFF_ATTACK_SCORE);
		attack_hits++;
	}
}

CALLSET_ENTRY (attack, right_hole_front_shot)
{
	if (attack_lit)
	{
		attack_lit--;
		timed_mode_begin (&attack_mode);
	}
}

CALLSET_ENTRY (attack, end_ball)
{
	timed_mode_end (&attack_mode);
}

CALLSET_ENTRY (attack, lamp_update)
{
	lamp_flash_if (LM_MARTIAN_ATTACK, attack_lit);
}

CALLSET_ENTRY (attack, display_update)
{
	timed_mode_display_update (&attack_mode);
}

CALLSET_ENTRY (attack, start_player)
{
	attack_lit = 0;
	attack_hits = 0;
}

/* Jet Value */

__local__ U8 jet_count_goal;
__local__ U8 jet_count;
score_t jet_value;

void jet_level_up (void)
{
	jet_count_goal += 25;
	jet_count = 0;
	attack_light ();
}

void upper_lane_score (U8 id)
{
}

CALLSET_ENTRY (jet, any_jet) {}
CALLSET_ENTRY (jet, sw_left_top_lane) { upper_lane_score (LM_L_TOP_LANE); }
CALLSET_ENTRY (jet, sw_right_top_lane) { upper_lane_score (LM_R_TOP_LANE); }
CALLSET_ENTRY (jet, sw_left_button)
{ lamplist_rotate_next (LAMPLIST_TOP_LANES, matrix_lookup (LMX_DEFAULT)); }
CALLSET_ENTRY (jet, sw_right_button)
{ lamplist_rotate_previous (LAMPLIST_TOP_LANES, matrix_lookup (LMX_DEFAULT)); }
CALLSET_ENTRY (jet, start_player)
{
	jet_count_goal = 50;
	jet_count = 0;
	score_copy (jet_value, score_table[SC_5K]);
}

/* Device update rules */

#include <motor_bank.h>
#include <drop_target.h>
#include <divertor.h>

CALLSET_ENTRY (divertor_rule, device_update)
{
	divertor_stop ();
}

CALLSET_ENTRY (drop_rule, drop_target_down)
{
	/* In multiball, the drop should be re-raised until
	the Super Jackpot is qualified. */
	if (mb_targets_left != 0)
		drop_target_reset ();
}

CALLSET_ENTRY (motor_bank_rule, device_update)
{
	if (mb_running_p ())
		motor_bank_move_down ();
	else
		motor_bank_move_up ();
}

