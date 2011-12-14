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
#include <eb.h>
#include <trunk_divert.h>

/* Miscellaneous */

U8 lamp_to_be_flickered;

void lamp_flicker (void)
{
}

void global_multiplier_update (void)
{
	extern U8 double_scoring_multiplier;
	extern U8 combo_multiplier;
	extern U8 trickmb_multiplier;
#if 0
	dbprintf ("mult %d %d %d\n",
		double_scoring_multiplier, combo_multiplier, trickmb_multiplier);
#endif
	score_multiplier_set (
		double_scoring_multiplier * combo_multiplier * trickmb_multiplier);
}

CALLSET_ENTRY (global_mult, idle_every_100ms)
{
	if (in_live_game)
		global_multiplier_update ();
}

/* Multiball conflict */

bool major_mb_running (void)
{
	extern bool trickmb_is_running (void);
	extern bool main_mb_is_running (void);
	extern bool midnight_mb_is_running (void);
	return trickmb_is_running () || main_mb_is_running () ||
		midnight_mb_is_running ();
}

bool any_mb_running (void)
{
	extern bool quickmb_is_running (void);
	return major_mb_running () || quickmb_is_running ();
}


/* Multiball ball saver */

U8 mbsave_timer;

void mbsave_task (void)
{
	/* Note: maximum of 30sec. due to multiplication below */
	mbsave_timer *= 8;
	while (mbsave_timer > 0)
	{
		do {
			task_sleep (TIME_133MS);
		} while (system_timer_pause ());
		mbsave_timer--;
	}
	task_sleep_sec (2);
	task_exit ();
}


void mbsave_enable (U8 secs)
{
	mbsave_timer = secs;
	task_recreate_gid (GID_MBSAVE, mbsave_task);
}

void mbsave_restart (void)
{
	task_kill_gid (GID_MBSAVE);
	add_ball_count (1);
}

CALLSET_ENTRY (mbsave, start_ball)
{
}

CALLSET_ENTRY (mbsave, display_update)
{
}

CALLSET_ENTRY (mbsave, trunk_hole_shot)
{
	if (task_find_gid (GID_MBSAVE))
	{
		mbsave_restart ();
	}
}

/* Grand Finale */

__local__ U8 finale_level;

bool finale_qualified (void)
{
	return flag_test (FLAG_FINALE_LIT);
}

bool finale_can_be_started (void)
{
	return finale_qualified () && !any_mb_running ();
}

bool finale_is_running (void)
{
	return finale_level > 0;
}

bool finale_level_running (U8 level)
{
	return finale_level == level;
}

void finale_light (void)
{
	flag_on (FLAG_FINALE_LIT);
}

void finale_start (void)
{
	if (finale_can_be_started ())
	{
		flag_off (FLAG_FINALE_LIT);
		lamp_tristate_flash (LM_GRAND_FINALE);
		lamplist_apply (LAMPLIST_MODES, lamp_flash_on);
		finale_level = 1;
	}
}

void finale_end (void)
{
	extern void badge_reset (void);
	lamp_tristate_off (LM_GRAND_FINALE);
	badge_reset ();
}

CALLSET_ENTRY (finale, lamp_update)
{
	lamp_flash_if (LM_START_FINALE, finale_can_be_started ());
}

CALLSET_ENTRY (finale, start_player)
{
	finale_level = 0;
}

CALLSET_ENTRY (finale, left_orbit_shot)
{
}

CALLSET_ENTRY (finale, captive_ball_shot)
{
}

CALLSET_ENTRY (finale, left_loop_shot)
{
}

CALLSET_ENTRY (finale, trunk_wall_shot)
{
}

CALLSET_ENTRY (finale, trunk_hole_shot)
{
}

CALLSET_ENTRY (finale, trunk_back_shot)
{
}

CALLSET_ENTRY (finale, center_ramp_shot)
{
}

CALLSET_ENTRY (finale, right_loop_shot)
{
}

CALLSET_ENTRY (finale, right_ramp_shot)
{
}

CALLSET_ENTRY (finale, right_orbit_shot)
{
}


/* Badges */

void badges_complete (void)
{
	finale_light ();
}

void badge_award (U8 lamp)
{
	if (!finale_is_running () && !finale_qualified ())
	{
		lamp_tristate_on (lamp);
		if (lamplist_test_all (LAMPLIST_MODES, lamp_test))
		{
			badges_complete ();
		}
	}
}

void badge_reset (void)
{
	lamplist_apply (LAMPLIST_MODES, lamp_off);
	lamplist_apply (LAMPLIST_MODES, lamp_flash_off);
}

CALLSET_ENTRY (badge, start_player)
{
	badge_reset ();
}


/* Illusions */
#define NUM_TRICKS 8
#define TR_CAPTIVE_BALL 0
#define TR_CENTER_RAMP 1
#define TR_TRUNK_WALL 2
#define TR_RIGHT_LOOP 3
#define TR_LEFT_LOOP 4
#define TR_RIGHT_RAMP 5
#define TR_RIGHT_ORBIT 6
#define TR_TRUNK_HOLE 7
#define TR_BIT(t) (1 << t)

const struct trick_info
{
	const char *mode_name;
	const char *shot_name;
	const char *shots_name;
	U8 mode_lamp;
	U8 shot_lamp;
	U8 goal_mult;
} trick_info_table[NUM_TRICKS] = {
	{ "TIGER SAW", "HIT", "HITS", LM_TIGER_SAW, LM_TIGER_SAW_AWARD, 1 },
	{ "LEVITATING WOMAN", "RAMP", "RAMPS", LM_LEVITATE_WOMAN, LM_LEVITATE_AWARD, 1 },
	{ "TRUNK ESCAPE", "HIT", "HITS", LM_TRUNK_ESCAPE, LM_TRUNK_LAMP, 1 },
	{ "SPIRIT CARDS", "SPIN", "SPINS", LM_SPIRIT_CARDS, LM_SPIRIT_AWARD, 10 },
	{ "SAFE ESCAPE", "LOOP", "LOOPS", LM_SAFE_ESCAPE, LM_SAFE_AWARD, 1 },
	{ "METAMORPHOSIS", "RAMP", "RAMPS", LM_METAMORPHOSIS, LM_METAMORPHOSIS_AWARD, 1 },
	{ "STRAIT JACKET", "ORBIT", "ORBITS", LM_STRAIT_JACKET, LM_JACKET_AWARD, 1 },
	{ "HAT MAGIC", "SHOT", "SHOTS", LM_HAT_MAGIC, LM_HAT_TRICK_AWARD, 1 },
};

__local__ struct trick
{
	U8 goal;
	U8 count;
	/* TBD - score level */
} trick_table[NUM_TRICKS];

__local__ U8 trick_level;
__local__ U8 tricks_finished;
__local__ U8 tricks_finished_this_ball;
__local__ U8 trick_shots_this_ball;

void trick_update_lamp (U8 n)
{
	const struct trick_info *trinfo = trick_info_table + n;
	U8 bit = TR_BIT(n);

	/* For each shot, there are two lamps updated: the
	'mode' lamp in the 4x2 grid at the bottom, and the
	'shot' lamp which is near the shot itself. */

	/* Logic for the shot lamp:
		1. If skill shot is active, then all remaining skill shots will
			be flashing.  Other lamps will be off.
		2. Else, during Trick Multiball, these lamps continuously flash
			for Jackpots.
		3. Else, during normal play when Tricks are available, they show
			which shots have been made this ball.
		4. Else, if Tricks are not available, then they will all be off. */
	if (trick_shots_this_ball & bit)
		lamp_tristate_on (trinfo->shot_lamp);
	else
		lamp_tristate_flash (trinfo->shot_lamp);

	/* Logic for the mode shots:
		(Skill shot has no effect on these.)
		1. If Trick MB running, shots which have not been collected will
			flash, others will be lit.
		2. Else, they show status towards next Trick MB:
			- If a shot has been finished this ball, it flashes
			- If finished earlier, it is solid
			- Else, it is off.
			This state is drawn even when Tricks are not available.
		*/
	if (tricks_finished_this_ball & bit)
		lamp_tristate_flash (trinfo->mode_lamp);
	else if (tricks_finished & bit)
		lamp_tristate_on (trinfo->mode_lamp);
	else
		lamp_tristate_off (trinfo->mode_lamp);
}

void trick_update_all_lamps (void)
{
	U8 n;
	for (n=0; n < NUM_TRICKS; n++) {
		trick_update_lamp (n);
	}
}

void trick_init_level (void)
{
	U8 n;
	for (n=0; n < NUM_TRICKS; n++) {
		trick_table[n].count = 0;
		trick_table[n].goal = trick_level * trick_info_table[n].goal_mult;
	}
	trick_update_all_lamps ();
}

void trick_advance_level (void)
{
	if (trick_level < 5)
	{
		trick_level++;
		trick_init_level ();
	}
}

void trick_advance_shot (struct trick *tr, U8 bit)
{
	if (tr->count < tr->goal)
	{
		tr->count++;
		if (tr->count == tr->goal)
		{
			tricks_finished |= bit;
			tricks_finished_this_ball |= bit;
		}
	}
}

void trick_award_shot (U8 n)
{
	struct trick *tr = trick_table + n;
	U8 bit = TR_BIT(n);
	extern void skill_shot_collect (U8 shot);

	if (global_flag_test (GLOBAL_FLAG_SKILL_SHOT_LIT))
	{
		skill_shot_collect (n);
		trick_advance_shot (tr, bit);
	}

	trick_advance_shot (tr, bit);
	trick_shots_this_ball |= bit;
	trick_update_lamp (n);
}

void trick_spot_shot (void)
{
	U8 n;
	struct trick *tr;
	for (n=0, tr = trick_table; n < NUM_TRICKS; n++, tr++) {
		if (tr->count < tr->goal)
		{
			trick_advance_shot (tr, TR_BIT(n));
			return;
		}
	}
}

CALLSET_ENTRY (trick, start_player)
{
	trick_level = 3;
	tricks_finished = 0;
	trick_init_level ();
}

CALLSET_ENTRY (trick, start_ball)
{
	tricks_finished_this_ball = 0;
	trick_shots_this_ball = 0;
	trick_update_all_lamps ();
}

CALLSET_ENTRY (trick, lamp_update)
{
}

CALLSET_ENTRY (trick, captive_ball_shot)
{ trick_award_shot (TR_CAPTIVE_BALL); }
CALLSET_ENTRY (trick, left_loop_shot)
{ trick_award_shot (TR_LEFT_LOOP); }
CALLSET_ENTRY (trick, trunk_wall_shot)
{ trick_award_shot (TR_TRUNK_WALL); }
CALLSET_ENTRY (trick, trunk_hole_shot)
{ trick_award_shot (TR_TRUNK_HOLE); }
CALLSET_ENTRY (trick, center_ramp_shot)
{ trick_award_shot (TR_CENTER_RAMP); }
CALLSET_ENTRY (trick, sw_spinner_slow)
{ trick_award_shot (TR_RIGHT_LOOP); }
CALLSET_ENTRY (trick, right_ramp_shot)
{ trick_award_shot (TR_RIGHT_RAMP); }
CALLSET_ENTRY (trick, right_orbit_shot)
{ trick_award_shot (TR_RIGHT_ORBIT); }

/* Trick Multiball */

__local__ U8 trickmb_level;

U8 trickmb_multiplier;

void trickmb_update (enum mb_mode_state state)
{
	if (state == MB_ACTIVE)
	{
		add_ball_count (1);
	}
	else if (state == MB_INACTIVE)
	{
		trickmb_multiplier = 1;
		global_multiplier_update ();
	}
}

struct mb_mode_ops trickmb_mode = {
	DEFAULT_MBMODE,
	.update = trickmb_update,
	.deff_running = DEFF_TRICK_MB,
	.gid_running = GID_TRICK_MB_RUNNING,
	.gid_in_grace = GID_TRICK_MB_GRACE,
};

bool trickmb_can_be_started (void)
{
	return flag_test (FLAG_TRICK_MB_LIT) &&
		!any_mb_running ();
}

bool trickmb_is_running (void)
{
	return mb_mode_running_p (&trickmb_mode);
}

void super_trick_enable (void)
{
	trickmb_multiplier = 2;
	global_multiplier_update ();
}

void trickmb_start (void)
{
	if (0)
		super_trick_enable ();
	mb_mode_start (&trickmb_mode);
	flag_off (FLAG_TRICK_MB_LIT);
}


CALLSET_ENTRY (trickmb, start_player)
{
	flag_off (FLAG_TRICK_MB_LIT);
	trickmb_level = 0;
}

CALLSET_ENTRY (trickmb, display_update)
{
	mb_mode_display_update (&trickmb_mode);
}

CALLSET_ENTRY (trickmb, music_refresh)
{
	mb_mode_music_refresh (&trickmb_mode);
}

CALLSET_ENTRY (trickmb, single_ball_play)
{
	mb_mode_single_ball (&trickmb_mode);
}

CALLSET_ENTRY (trickmb, trunk_hole_shot)
{
	if (trickmb_can_be_started ())
		trickmb_start ();
}

CALLSET_ENTRY (trickmb, start_game, start_ball)
{
	trickmb_multiplier = 1;
}

/* Rollovers - Bonus X */

U8 bonusx;

void bonusx_add (U8 count)
{
	bonusx += count;
	if (bonusx > 99)
		bonusx = 99;
}

void bonusx_lane (U8 lamp)
{
	if (lamp_test (lamp))
	{
		score (SC_10K);
	}
	else
	{
		score (SC_50K);
		lamp_on (lamp);

		if (lamp_test (LM_ROLLOVER_1) && lamp_test (LM_ROLLOVER_2))
		{
			bonusx_add (1);
			lamplist_apply (LAMPLIST_ROLLOVERS, lamp_off);
			sound_send (SND_TADA);
			deff_data_init ();
			deff_data_push (bonusx);
			deff_start (DEFF_BONUSX);
			leff_start (LEFF_BONUSX);
		}
	}
}

CALLSET_ENTRY (bonusx, sw_top_lane_1)
{
	bonusx_lane (LM_ROLLOVER_1);
}

CALLSET_ENTRY (bonusx, sw_top_lane_2)
{
	bonusx_lane (LM_ROLLOVER_2);
}

CALLSET_ENTRY (bonusx, sw_left_button, sw_right_button)
{
	lamplist_rotate_next (LAMPLIST_ROLLOVERS, matrix_lookup (LMX_DEFAULT));
}

CALLSET_ENTRY (bonusx, start_ball)
{
	bonusx = 1;
	lamp_off (LM_ROLLOVER_1);
	lamp_off (LM_ROLLOVER_2);
}

/* Hurryup */

U8 hurryup_timer;

U8 hurryup_multiplier;

score_t hurryup_value;

free_timer_id_t hurryup_pause_timer;

bool hurryup_timer_pause (void)
{
	if (free_timer_test (hurryup_pause_timer))
		return TRUE;
	return system_timer_pause ();
}

void hurryup_exit (void)
{
	extern void theatre_spell_reset (void);
	theatre_spell_reset ();
}

void hurryup_update (void)
{
	score_sub (hurryup_value, score_table[SC_17530]);
	if (score_compare (hurryup_value, score_table[SC_250K]) == -1)
		score_copy (hurryup_value, score_table[SC_250K]);
}

struct timed_mode_ops hurryup_mode = {
	DEFAULT_MODE,
	.exit = hurryup_exit,
	.gid = GID_HURRYUP_RUNNING,
	.update = hurryup_update,
	.init_timer = 20,
	.deff_running = DEFF_HURRYUP,
	.timer = &hurryup_timer,
	.grace_timer = 2,
	.pause = hurryup_timer_pause,
};

struct fixed_ladder hurryup_scoring = {
	.base = score_table[SC_500K],
	.increment = score_table[SC_250K],
	.max = score_table[SC_1M],
	.current = hurryup_value,
};

bool hurryup_is_running (void)
{
	return timed_mode_running_p (&hurryup_mode);
}

bool hurryup_can_be_started (void)
{
	return TRUE;
}

void hurryup_start (void)
{
	hurryup_multiplier = 3;
	fixed_ladder_reset (&hurryup_scoring);
	timed_mode_begin (&hurryup_mode);
}

CALLSET_ENTRY (hurryup, lamp_update)
{
	lamp_flash_if (LM_HURRY_UP, timed_mode_effect_running_p (&hurryup_mode));
}

CALLSET_ENTRY (hurryup, trunk_hole_shot)
{
	if (hurryup_is_running ())
	{
		score_long (hurryup_value);
		fixed_ladder_advance (&hurryup_scoring);
	}
}

CALLSET_ENTRY (hurryup, display_update)
{
	timed_mode_display_update (&hurryup_mode);
}

CALLSET_ENTRY (hurryup, music_refresh)
{
	timed_mode_music_refresh (&hurryup_mode);
}

CALLSET_ENTRY (hurryup, end_ball)
{
	timed_mode_end (&hurryup_mode);
}

/* Theatre Letters */

#define THEATRE_LEN 7

__local__ U8 theatre_spell_count;

U8 theatre_spelled_this_ball;

score_t theatre_letter_value;

struct fixed_ladder theatre_letter_scoring = {
	.base = score_table[SC_100K],
	.increment = score_table[SC_10K],
	.max = score_table[SC_200K],
	.current = theatre_letter_value
};

bool theatre_can_be_spelled (void)
{
	return !any_mb_running () && !hurryup_is_running();
}

void theatre_spell_reset (void)
{
	theatre_spell_count = 0;
	lamplist_apply (LAMPLIST_THEATRE_SPELL, lamp_off);
}

void theatre_spell_complete (void)
{
	sound_send (SND_KABOOM);
	lamp_flash_on (LM_HURRY_UP);
	hurryup_start ();
}

void theatre_spell_advance (void)
{
	if (theatre_spell_count >= THEATRE_LEN)
		return;
	fixed_ladder_score_and_advance (&theatre_letter_scoring);
	theatre_spell_count++;
	lamplist_build_increment (LAMPLIST_THEATRE_SPELL, matrix_lookup (LMX_DEFAULT));

	deff_data_init ();
	deff_data_push (theatre_spell_count);
	deff_start (DEFF_THEATRE_LETTER);

	if (theatre_spell_count == THEATRE_LEN)
		theatre_spell_complete ();
}

CALLSET_ENTRY (theatre_spell, lamp_update)
{
	lamp_on_if (LM_SPELL_THEATRE, theatre_can_be_spelled ());
}

CALLSET_ENTRY (theatre_spell, left_orbit_shot)
{
	if (theatre_can_be_spelled ())
	{
		theatre_spell_advance ();
	}
}

CALLSET_ENTRY (theatre_spell, start_player)
{
	theatre_spell_reset ();
}

CALLSET_ENTRY (theatre_spell, start_ball)
{
	theatre_spelled_this_ball = 0;
	lamp_off (LM_HURRY_UP);
	fixed_ladder_reset (&theatre_letter_scoring);
}

/* Magic Letters */

__local__ U8 magic_spell_count;
__local__ U8 magic_relight_ramp_count;
__local__ U8 magic_relight_ramp_init;

bool magic_can_be_spelled (void)
{
	return TRUE;
}

void magic_spell_reset (void)
{
	magic_spell_count = 0;
	lamplist_apply (LAMPLIST_MAGIC_SPELL, lamp_off);
}

void magic_spell_complete (void)
{
	extern void ball_lock_enable (void);
	sound_send (SND_MAGIC_SPELLED);
	ball_lock_enable ();
}

void magic_spell_advance (void)
{
	if (magic_can_be_spelled () && magic_spell_count < 5)
	{
		magic_spell_count++;
		score (SC_50K);
		lamplist_build_increment (LAMPLIST_MAGIC_SPELL,
			matrix_lookup (LMX_DEFAULT));

		deff_data_init ();
		deff_data_push (magic_spell_count);
		deff_start (DEFF_MAGIC_LETTER);

		if (magic_spell_count == 5)
			magic_spell_complete ();
	}
}


CALLSET_ENTRY (magic_spell, start_player)
{
	magic_spell_reset ();
	magic_relight_ramp_init = 0;
	magic_relight_ramp_count = 0;
	lamp_tristate_on (LM_CENTER_MAGIC);
	lamp_tristate_on (LM_RIGHT_MAGIC);
}

CALLSET_ENTRY (magic_spell, center_ramp_shot)
{
	if (lamp_test (LM_CENTER_MAGIC))
	{
		magic_spell_advance ();
	}
}

CALLSET_ENTRY (magic_spell, right_ramp_shot)
{
	if (lamp_test (LM_RIGHT_MAGIC))
	{
		magic_spell_advance ();
	}
}

CALLSET_ENTRY (magic_spell, sw_ramp_targets)
{
	score (SC_5K);
	if (magic_relight_ramp_count > 0)
	{
		if (--magic_relight_ramp_count == 0)
		{
			if (!lamp_test (LM_CENTER_MAGIC))
				lamp_on (LM_CENTER_MAGIC);
			else if (!lamp_test (LM_RIGHT_MAGIC))
				lamp_on (LM_RIGHT_MAGIC);
		}
	}
}

CALLSET_ENTRY (magic_spell, start_ball)
{
	magic_relight_ramp_count = magic_relight_ramp_init;
}

/* Main Multiball Ball Lock */

__local__ U8 locks_lit;
__local__ U8 balls_locked;
__local__ U8 locks_lit_per_spell;

bool ball_lock_can_be_collected (void)
{
	return locks_lit && !any_mb_running ();
}

void ball_lock_enable (void)
{
	locks_lit += locks_lit_per_spell;
}

void ball_lock_reset (void)
{
	locks_lit = 0;
	balls_locked = 0;
}

void ball_lock_award (void)
{
	extern void main_mb_start (void);

	balls_locked++;
	locks_lit--;
	if (balls_locked == 3)
	{
		ball_lock_reset ();
		main_mb_start ();
		if (locks_lit_per_spell > 1)
			locks_lit_per_spell--;
	}
	else if (locks_lit == 0)
	{
		magic_spell_reset ();
	}
}

CALLSET_ENTRY (ball_lock, lamp_update)
{
	lamp_flash_if (LM_LOCK_BALL, ball_lock_can_be_collected ());
}

CALLSET_ENTRY (ball_lock, start_ball)
{
	locks_lit_per_spell = 3;
	ball_lock_reset ();
}

CALLSET_ENTRY (ball_lock, left_loop_shot, right_loop_shot)
{
	if (ball_lock_can_be_collected () && balls_locked < 2)
	{
		trunk_divert_start ();
	}
}

CALLSET_ENTRY (ball_lock, trunk_back_shot)
{
	if (ball_lock_can_be_collected () && balls_locked < 2)
	{
		device_t *dev = device_entry (DEVNO_SUBWAY);
		if (device_recount (dev) < 2)
		{
			device_lock_ball (dev);
		}
		else
		{
			/* start kickout warning effect */
		}
		ball_lock_award ();
	}
}

CALLSET_ENTRY (ball_lock, trunk_hole_shot)
{
	if (ball_lock_can_be_collected () && balls_locked >= 2)
	{
		ball_lock_award ();
	}
}

/* Main Multiball */

#define MAINMB_SHOT_CENTER_RAMP 1
#define MAINMB_SHOT_LEFT_LOOP 2
#define MAINMB_SHOT_RIGHT_RAMP 3
#define MAINMB_SHOT_RIGHT_ORBIT 4
#define MAINMB_SHOT_TRUNK 5

__local__ U8 main_mb_level;

__local__ U8 main_mb_shot;

score_t main_mb_trunk_value;

struct fixed_ladder main_mb_trunk_scoring = {
	.base = score_table[SC_200K],
	.increment = score_table[SC_100K],
	.max = score_table[SC_600K],
	.current = main_mb_trunk_value
};

score_t main_mb_jackpot_value;

struct fixed_ladder main_mb_jackpot_scoring = {
	.base = score_table[SC_2M],
	.increment = score_table[SC_2M],
	.max = score_table[SC_10M],
	.current = main_mb_trunk_value
};

U8 main_mb_shots_left;

void main_mb_update (enum mb_mode_state state)
{
	if (state == MB_ACTIVE)
	{
		effect_update_request ();
		set_ball_count (3);
	}
	else if (state == MB_INACTIVE)
	{
		main_mb_level++;
	}
}

struct mb_mode_ops main_mb_mode = {
	DEFAULT_MBMODE,
	.update = main_mb_update,
	.deff_running = DEFF_MAIN_MB,
	.gid_running = GID_MAIN_MB_RUNNING,
	.gid_in_grace = GID_MAIN_MB_GRACE,
};

bool main_mb_can_be_started (void)
{
	return !any_mb_running ();
}

bool main_mb_is_running (void)
{
	return mb_mode_running_p (&main_mb_mode);
}

bool main_mb_shot_lit (U8 shot)
{
	return main_mb_is_running () && main_mb_shot == shot &&
		main_mb_shots_left == 0;
}

void main_mb_set_shot_count (void)
{
	main_mb_shots_left = main_mb_shot + 2;
	if (main_mb_shots_left > 7)
		main_mb_shots_left = 7;
}

void main_mb_award_jackpot (void)
{
	fixed_ladder_score_and_advance (&main_mb_jackpot_scoring);
	fixed_ladder_advance (&main_mb_trunk_scoring);
	if (main_mb_shot < MAINMB_SHOT_TRUNK)
		main_mb_shot++;
	main_mb_set_shot_count ();
}

void main_mb_start (void)
{
	main_mb_set_shot_count ();
	mbsave_enable (20);
	mb_mode_start (&main_mb_mode);
}

CALLSET_ENTRY (main_mb, single_ball_play)
{
	mb_mode_single_ball (&main_mb_mode);
}

CALLSET_ENTRY (main_mb, display_update)
{
	mb_mode_display_update (&main_mb_mode);
}

CALLSET_ENTRY (main_mb, start_player)
{
	main_mb_level = 0;
	main_mb_shot = 0;
}

CALLSET_ENTRY (main_mb, center_ramp_shot)
{
	if (main_mb_shot_lit (MAINMB_SHOT_CENTER_RAMP))
		main_mb_award_jackpot ();
}

CALLSET_ENTRY (main_mb, left_loop_shot)
{
	if (main_mb_shot_lit (MAINMB_SHOT_LEFT_LOOP))
		main_mb_award_jackpot ();
}

CALLSET_ENTRY (main_mb, right_ramp_shot)
{
	if (main_mb_shot_lit (MAINMB_SHOT_RIGHT_RAMP))
		main_mb_award_jackpot ();
}

CALLSET_ENTRY (main_mb, right_orbit_shot)
{
	if (main_mb_shot_lit (MAINMB_SHOT_RIGHT_ORBIT))
		main_mb_award_jackpot ();
}

CALLSET_ENTRY (main_mb, trunk_hole_shot)
{
	if (main_mb_shot_lit (MAINMB_SHOT_TRUNK))
		main_mb_award_jackpot ();
}

CALLSET_ENTRY (main_mb, trunk_wall_shot)
{
	if (main_mb_is_running () && main_mb_shots_left)
	{
		fixed_ladder_score (&main_mb_trunk_scoring);
		main_mb_shots_left--;
		if (main_mb_shots_left == 0)
		{
		}
	}
}

/* Midnight Multiball */

score_t midnight_mb_sw_value;
struct fixed_ladder midnight_mb_sw_scoring = {
	.base = score_table[SC_25K],
	.increment = score_table[SC_5K],
	.max = score_table[SC_50K],
	.current = midnight_mb_sw_value,
};

score_t midnight_mb_jackpot_value;
struct fixed_ladder midnight_mb_jackpot_scoring = {
	.base = score_table[SC_500K],
	.increment = score_table[SC_100K],
	.max = score_table[SC_1M],
	.current = midnight_mb_jackpot_value,
};

U8 midnight_mb_trunk_count;

void midnight_mb_update (enum mb_mode_state state)
{
	if (state == MB_ACTIVE)
	{
		add_ball_count (1);
	}
}

struct mb_mode_ops midnight_mb_mode = {
	DEFAULT_MBMODE,
	.update = midnight_mb_update,
	.deff_running = DEFF_MIDNIGHT_MB,
	.gid_running = GID_MIDNIGHT_MB_RUNNING,
	.gid_in_grace = GID_MIDNIGHT_MB_GRACE,
};

bool midnight_mb_is_running (void)
{
	return mb_mode_running_p (&midnight_mb_mode);
}

bool midnight_mb_qualified (void)
{
	return flag_test (FLAG_MIDNIGHT_MB_LIT);
}

bool midnight_mb_can_be_started (void)
{
	return midnight_mb_qualified () &&
		!any_mb_running ();
}

void midnight_mb_light (void)
{
	flag_on (FLAG_MIDNIGHT_MB_LIT);
}

void midnight_mb_update_lamps (void)
{
	if (midnight_mb_trunk_count == 0)
	{
		lamp_tristate_flash (LM_TRUNK_1);
		lamp_tristate_off (LM_TRUNK_2);
		lamp_tristate_off (LM_TRUNK_3);
	}
	else if (midnight_mb_trunk_count == 1)
	{
		lamp_tristate_on (LM_TRUNK_1);
		lamp_tristate_flash (LM_TRUNK_2);
		lamp_tristate_off (LM_TRUNK_3);
	}
	else if (midnight_mb_trunk_count == 2)
	{
		lamp_tristate_on (LM_TRUNK_1);
		lamp_tristate_on (LM_TRUNK_2);
		lamp_tristate_flash (LM_TRUNK_3);
	}
	else if (midnight_mb_trunk_count == 3)
	{
		lamp_tristate_on (LM_TRUNK_1);
		lamp_tristate_on (LM_TRUNK_2);
		lamp_tristate_on (LM_TRUNK_3);
	}
}

void midnight_mb_set_trunk_count (U8 count)
{
	midnight_mb_trunk_count = count;
	midnight_mb_update_lamps ();
}

void midnight_mb_start (void)
{
	fixed_ladder_reset (&midnight_mb_sw_scoring);
	fixed_ladder_reset (&midnight_mb_jackpot_scoring);
	midnight_mb_set_trunk_count (0);
	mb_mode_start (&midnight_mb_mode);
}

CALLSET_ENTRY (midnight_mb, trunk_wall_shot)
{
	if (mb_mode_running_p (&midnight_mb_mode))
	{
		if (midnight_mb_trunk_count < 3)
		{
			midnight_mb_set_trunk_count (midnight_mb_trunk_count+1);
			if (midnight_mb_trunk_count == 3)
			{
				fixed_ladder_advance (&midnight_mb_sw_scoring);
			}
		}
	}
}

CALLSET_ENTRY (midnight_mb, right_orbit_shot)
{
	if (mb_mode_running_p (&midnight_mb_mode))
	{
		midnight_mb_set_trunk_count (0);
		fixed_ladder_score_and_advance (&midnight_mb_jackpot_scoring);
	}
}

CALLSET_ENTRY (midnight_mb, any_pf_switch)
{
	if (mb_mode_running_p (&midnight_mb_mode))
	{
		fixed_ladder_score (&midnight_mb_sw_scoring);
	}
}

CALLSET_ENTRY (midnight_mb, display_update)
{
	mb_mode_display_update (&midnight_mb_mode);
}

CALLSET_ENTRY (midnight_mb, music_refresh)
{
	mb_mode_music_refresh (&midnight_mb_mode);
}

CALLSET_ENTRY (midnight_mb, lamp_update)
{
	midnight_mb_update_lamps ();
}

CALLSET_ENTRY (midnight_mb, single_ball_play)
{
	mb_mode_single_ball (&midnight_mb_mode);
}

/* Clock advance to Midnight */

__local__ U8 midnight_level;
__local__ U8 clock_hour;
__local__ U8 clock_min;
U8 clock_jet_min;
U8 clock_orbit_min;

#define CLK_STEP_MIN 10
#define CLK_STEPS_PER_HR 6
#define CLK_MIDNIGHT_HOUR 12
#define CLK_MAX_STEPS (CLK_HOURS * CLK_STEPS_PER_HR)

void clock_display_task (void)
{
	task_sleep_sec (3);
	if (clock_hour <= 6)
		sound_send (SPCH_1PM + clock_hour - 1);
	else
		; /* sound_send (SPCH_7PM + clock_hour - 1); */
	deff_start (DEFF_CLOCK_UPDATE);
	task_exit ();
}

bool clock_can_be_awarded (void)
{
	return ((clock_hour < CLK_MIDNIGHT_HOUR) && !midnight_mb_qualified ());
}

void clock_add_min (U8 min)
{
	if (clock_can_be_awarded ())
	{
		clock_min += min;
		while (clock_min >= 60)
		{
			clock_min -= 60;
			clock_hour++;
			task_recreate_gid (GID_CLOCK_DISPLAY_UPDATE, clock_display_task);
		}
		if (clock_hour >= CLK_MIDNIGHT_HOUR)
		{
			clock_hour = 12;
			clock_min = 0;
			midnight_mb_light ();
		}
	}
}

CALLSET_ENTRY (clock, lamp_update)
{
	lamp_on_if (LM_ADVANCE_CLOCK, clock_can_be_awarded ());
}

CALLSET_ENTRY (clock, sw_top_jet, sw_bottom_jet, sw_middle_jet)
{
	if (clock_can_be_awarded ())
	{
		score (SC_10K);
		clock_add_min (clock_jet_min);
	}
}

CALLSET_ENTRY (clock, right_orbit_shot)
{
	if (clock_can_be_awarded ())
	{
		score (SC_50K);
		clock_add_min (clock_orbit_min);
		if (clock_orbit_min < 60)
			clock_orbit_min += 10;
	}
}

CALLSET_ENTRY (clock, start_player)
{
	midnight_level = 0;
	clock_hour = clock_min = 0;
}

CALLSET_ENTRY (clock, start_ball)
{
	clock_jet_min = 5;
	clock_orbit_min = 30;
}

CALLSET_ENTRY (clock, sw_captive_ball_top)
{
	if (clock_jet_min < 15)
	{
		/* TBD effect */
		clock_jet_min += 5;
	}
}

/* Newton Ball countup rules */

__local__ U8 newton_count;

__local__ U8 newton_next_level;

score_t newton_value;

struct fixed_ladder newton_scoring = {
	.base = score_table[SC_50K],
	.increment = score_table[SC_10K],
	.max = score_table[SC_100K],
	.current = newton_value
};


bool newton_can_be_awarded (void)
{
	return (newton_count < 250);
}

void newton_bonus_award (void)
{
	extern void quickmb_light (void);
	newton_next_level += 10;
	deff_start (DEFF_NEWTON_BONUS);
	quickmb_light ();
}

CALLSET_ENTRY (newton, lamp_update)
{
	// turn on Light Vanish if next level ready
	//lamp_on_if (LM_TIGER_SAW_AWARD, newton_can_be_awarded ());
}

CALLSET_ENTRY (newton, captive_ball_shot)
{
	if (newton_can_be_awarded ())
	{
		fixed_ladder_score_and_advance (&newton_scoring);
		deff_start (DEFF_NEWTON_HIT);
		newton_count++;
		sound_send (SND_TIGER);
		if (newton_count == newton_next_level)
		{
			newton_bonus_award ();
		}
	}
}

CALLSET_ENTRY (newton, start_player)
{
	newton_count = 0;
	newton_next_level = 5;
}

CALLSET_ENTRY (newton, start_ball)
{
	fixed_ladder_reset (&newton_scoring);
}

/* Quick Multiball rules - use timed mode */

void quick_mb_update (enum mb_mode_state state)
{
	if (state == MB_ACTIVE)
	{
		add_ball_count (1);
	}
}

struct mb_mode_ops quick_mb_mode = {
	DEFAULT_MBMODE,
	.update = quick_mb_update,
	.deff_running = DEFF_QUICK_MB,
	.gid_running = GID_QUICK_MB_RUNNING,
	.gid_in_grace = GID_QUICK_MB_GRACE,
	.grace_period = TIME_2S,
};

bool quickmb_is_qualified (void)
{
	return flag_test (FLAG_QUICK_MB_LIT);
}

bool quickmb_can_be_started (void)
{
	return quickmb_is_qualified ();
}

bool quickmb_is_running (void)
{
	return mb_mode_running_p (&quick_mb_mode);
}

void quickmb_light (void)
{
	flag_on (FLAG_QUICK_MB_LIT);
}

void quickmb_start (void)
{
	if (quickmb_can_be_started ())
	{
		flag_off (FLAG_QUICK_MB_LIT);
		mb_mode_start (&quick_mb_mode);
	}
}

CALLSET_ENTRY (quickmb, display_update)
{
	mb_mode_display_update (&quick_mb_mode);
}

CALLSET_ENTRY (quickmb, music_refresh)
{
	mb_mode_music_refresh (&quick_mb_mode);
}

CALLSET_ENTRY (quickmb, single_ball_play)
{
	mb_mode_single_ball (&quick_mb_mode);
}

CALLSET_ENTRY (quickmb, dev_vanish_enter)
{
}

/* Double Scoring */

U8 double_score_timer;

U8 double_scoring_multiplier;

void double_score_init (void)
{
	double_scoring_multiplier = 2;
	global_multiplier_update ();
}

void double_score_exit (void)
{
	double_scoring_multiplier = 1;
	global_multiplier_update ();
}

struct timed_mode_ops double_score_mode = {
	DEFAULT_MODE,
	.init = double_score_init,
	.gid = GID_DOUBLE_SCORE_RUNNING,
	.deff_running = DEFF_DOUBLE_SCORING,
	.init_timer = 15,
	.timer = &double_score_timer,
	.grace_timer = 2,
	.pause = system_timer_pause,
};

bool double_score_is_running (void)
{
	return timed_mode_running_p (&double_score_mode);
}

void double_score_start (void)
{
	if (timed_mode_running_p (&double_score_mode))
	{
		timed_mode_add (&double_score_mode, 15);
	}
	else
	{
		timed_mode_begin (&double_score_mode);
	}
}

CALLSET_ENTRY (double_score, display_update)
{
	timed_mode_display_update (&double_score_mode);
}

CALLSET_ENTRY (double_score, music_refresh)
{
	timed_mode_music_refresh (&double_score_mode);
}

CALLSET_ENTRY (double_score, start_game)
{
	double_scoring_multiplier = 1;
}

CALLSET_ENTRY (double_score, start_ball)
{
	double_score_exit ();
}

CALLSET_ENTRY (double_score, end_ball)
{
	timed_mode_end (&double_score_mode);
}


/* Spirit Ring */

score_t spirit_ring_value;

struct fixed_ladder spirit_ring_scoring = {
	.base = score_table[SC_500K],
	.increment = score_table[SC_100K],
	.max = score_table[SC_1M],
	.current = spirit_ring_value,
};

bool spirit_ring_qualified (void)
{
	return flag_test (FLAG_SPIRIT_RING_LIT);
}

bool spirit_ring_can_be_awarded (void)
{
	return spirit_ring_qualified ();
}

void spirit_ring_light (void)
{
	flag_on (FLAG_SPIRIT_RING_LIT);
	deff_start (DEFF_SPIRIT_RING_LIT);
}

void spirit_ring_collect (void)
{
	flag_off (FLAG_SPIRIT_RING_LIT);
	fixed_ladder_score_and_advance (&spirit_ring_scoring);
}

CALLSET_ENTRY (spirit_ring, lamp_update)
{
	lamp_on_if (LM_SPIRIT_RING, spirit_ring_can_be_awarded ());
}

CALLSET_ENTRY (spirit_ring, right_lower_target)
{
	if (!spirit_ring_qualified ())
	{
		spirit_ring_light ();
	}
	score (SC_25K);
}

CALLSET_ENTRY (spirit_ring, sw_magnet_button)
{
}

CALLSET_ENTRY (spirit_ring, right_ramp_shot)
{
	if (spirit_ring_can_be_awarded ())
	{
		spirit_ring_collect ();
	}
}

CALLSET_ENTRY (spirit_ring, start_ball)
{
	fixed_ladder_reset (&spirit_ring_scoring);
}

/* Hocus Pocus */

__local__ U8 hocus_pocus_count;

__local__ U8 hocus_pocus_goal;


bool hocus_pocus_qualified (void)
{
	return flag_test (FLAG_HOCUS_POCUS_LIT);
}

bool hocus_pocus_can_be_started (void)
{
	return hocus_pocus_qualified ();
}

CALLSET_ENTRY (hocus_pocus, sw_left_bank)
{
	if (hocus_pocus_can_be_started ())
		return;
	if (++hocus_pocus_count == hocus_pocus_goal)
	{
		flag_on (FLAG_HOCUS_POCUS_LIT);
		bounded_increment (hocus_pocus_goal, 5);
	}
}

CALLSET_ENTRY (hocus_pocus, lamp_update)
{
	if (hocus_pocus_can_be_started ())
	{
		lamp_tristate_on (LM_HOCUS_POCUS);
	}
	else if (hocus_pocus_goal - hocus_pocus_count == 1)
	{
		lamp_tristate_flash (LM_HOCUS_POCUS);
	}
	else
	{
		lamp_tristate_off (LM_HOCUS_POCUS);
	}
}

CALLSET_ENTRY (hocus_pocus, sw_left_drain_eddy, sw_right_drain_eddy)
{
	if (hocus_pocus_can_be_started ())
	{
		flag_off (FLAG_HOCUS_POCUS_LIT);
		hocus_pocus_count = 0;
	}
}

CALLSET_ENTRY (hocus_pocus, start_ball)
{
	flag_off (FLAG_HOCUS_POCUS_LIT);
}

CALLSET_ENTRY (hocus_pocus, start_player)
{
	hocus_pocus_goal = 2;
	hocus_pocus_count = 0;
}


/* Basement - Mystery Award */

__local__ U8 rnd_count;

__local__ U8 rnd_prev_award;

#define RND_EB 0
#define RND_LIGHT_LOCK 1
#define RND_ADVANCE_CLOCK 2
#define RND_ADVANCE_THEATRE 3
#define RND_SPOT_ILLUSION 4
#define RND_LIGHT_HOCUS 5
#define RND_ADD_BONUS_X 6
#define RND_START_DOUBLE 7
#define RND_BIG_POINTS 8

U8 rnd_eb_prob (void)
{
	if (!can_award_extra_ball ())
		return 0;
	if (flag_test (FLAG_RANDOM_EB_GIVEN))
		return 0;
	return 10;
}

void rnd_eb_award (void)
{
	flag_on (FLAG_RANDOM_EB_GIVEN);
	light_easy_extra_ball ();
}

U8 rnd_light_lock_prob (void)
{
	if (ball_lock_can_be_collected ())
		return 0;
	return 15;
}

void rnd_light_lock (void)
{
	ball_lock_award ();
}

U8 rnd_advance_clock_prob (void)
{
	return 15;
}

void rnd_advance_clock (void)
{
	clock_add_min (120);
}

void rnd_spot_illusion (void)
{
	trick_spot_shot ();
}

void rnd_light_hocus (void)
{
	flag_on (FLAG_HOCUS_POCUS_LIT);
}

U8 rnd_add_bonusx_prob (void)
{
	if (bonusx < 5)
		return 25;
	else if (bonusx < 10)
		return 10;
	else
		return 5;
}

void rnd_add_bonusx (void)
{
	bonusx_add (3);
}

U8 rnd_start_double_prob (void)
{
	if (double_score_is_running ())
		return 0;
	return 20;
}

void rnd_start_double (void)
{
	double_score_start ();
}

U8 rnd_advance_theatre_prob (void)
{
	if (!theatre_can_be_spelled ())
		return 0;
	return 25;
}

void rnd_advance_theatre (void)
{
	theatre_spell_advance ();
	theatre_spell_advance ();
}

U8 rnd_big_points_prob (void)
{
	if (rnd_count <= 3)
		return 10;
	else if (rnd_count <= 6)
		return 30;
	else
		return 50;
}

void rnd_big_points (void)
{
	score (SC_1M);
}

U8 rnd_default_prob (void)
{
	return 20;
}

const char *rnd_choice;

__local__ U8 rnd_last;

struct rnd_award
{
	const char *name;
	U8 (*get_prob) (void);
	void (*exec) (void);
} random_award_table[] = {
	[RND_EB] = { "LIGHT EXTRA BALL", rnd_eb_prob, rnd_eb_award },
	[RND_LIGHT_LOCK] = { "LIGHT LOCK", rnd_light_lock_prob, rnd_light_lock },
	[RND_ADVANCE_CLOCK] = { "ADVANCE CLOCK", rnd_advance_clock_prob, rnd_advance_clock },
	[RND_SPOT_ILLUSION] = { "SPOT ILLUSION", rnd_default_prob, rnd_spot_illusion },
	[RND_LIGHT_HOCUS] = { "HOCUS POCUS", rnd_default_prob, rnd_light_hocus },
	[RND_ADD_BONUS_X] = { "ADD 3X BONUS", rnd_add_bonusx_prob, rnd_add_bonusx },
	[RND_START_DOUBLE] = { "DOUBLE SCORING", rnd_start_double_prob, rnd_start_double },
	[RND_ADVANCE_THEATRE] = { "2 THEATRE LETTERS", rnd_advance_theatre_prob, rnd_advance_theatre },
	[RND_BIG_POINTS] = { "1,000,000", rnd_big_points_prob, rnd_big_points },
};
#define RND_COUNT (sizeof(random_award_table) / sizeof(random_award_table[0]))


void mystery_award (void)
{
	struct rnd_award *rnd;
	U8 count;
	U8 range;
	U8 rno;

	flag_off (FLAG_BASEMENT_LIT);
	rnd_count++;

	range = 0;
	for (count=0, rnd=random_award_table; count < RND_COUNT; count++, rnd++)
	{
		if (count != rnd_last)
			range += rnd->get_prob ();
	}

	rno = random_scaled (range);

	range = 0;
	for (count=0, rnd=random_award_table; count < RND_COUNT; count++, rnd++)
	{
		if (count != rnd_last)
		{
			range += rnd->get_prob ();
			if (rno < range)
			{
award:
				rnd_choice = rnd->name;
				rnd->exec ();
				deff_start (DEFF_RANDOM_AWARD);
				return;
			}
		}
	}
	dbprintf ("did not find random award");
	rnd = random_award_table + RND_BIG_POINTS;
	goto award;
}

void mystery_light (void)
{
	flag_on (FLAG_BASEMENT_LIT);
}

bool mystery_qualified (void)
{
	return flag_test (FLAG_BASEMENT_LIT);
}

bool basement_can_be_awarded (void)
{
	return mystery_qualified () && !any_mb_running ();
}

CALLSET_ENTRY (basement, sw_left_inlane)
{
	if (basement_can_be_awarded ())
	{
	}
}

CALLSET_ENTRY (basement, init)
{
	rnd_choice = random_award_table[0].name;
}

CALLSET_ENTRY (basement, lamp_update)
{
	lamp_flash_if (LM_BASEMENT, basement_can_be_awarded ());
}

CALLSET_ENTRY (basement, basement_shot)
{
	if (basement_can_be_awarded ())
	{
		mystery_award ();
	}
}

CALLSET_ENTRY (basement, device_update)
{
	/* open the trap door when the shot is lit */
}

CALLSET_ENTRY (basement, start_player)
{
	rnd_count = 0;
	rnd_last = RND_BIG_POINTS;
	rnd_prev_award = RND_EB;
}


/* Combos */

U8 last_combo_shot;

U8 combo_count;

U8 combo_multiplier;

free_timer_id_t combo_timer;

void combo_set_multiplier (U8 mult)
{
	combo_multiplier = mult;
	global_multiplier_update ();
}

void combo_reset (void)
{
	combo_count = 0;
	combo_set_multiplier (1);
}

void combo_exit (void)
{
	combo_reset ();
	task_exit ();
}

void combo_detect (U8 id)
{
	if (id == last_combo_shot)
		return;
	if (!free_timer_test (combo_timer))
		combo_reset ();

	free_timer_restart (combo_timer, TIME_4S);
	last_combo_shot = id;
	if (++combo_count == 3)
	{
		combo_set_multiplier (2);
		task_create_gid1 (GID_COMBO_EXIT, combo_exit);
	}
}

void combo_stop (void)
{
	last_combo_shot = 0xFF;
	free_timer_stop (combo_timer);
}

CALLSET_ENTRY (combo, sw_left_sling, sw_right_sling)
{
	combo_stop ();
}

CALLSET_ENTRY (combo, start_game)
{
	combo_set_multiplier (1);
}

CALLSET_ENTRY (combo, start_ball)
{
	last_combo_shot = 0xFF;
	combo_reset ();
}

/* Extra Ball */

CALLSET_ENTRY (eb_award, left_loop_shot)
{
	if (decrement_extra_balls ())
	{
	}
}

/* Special */

CALLSET_ENTRY (special_award, sw_left_outlane, sw_right_outlane)
{
	if (lamp_test (LM_SPECIAL))
	{
		lamp_off (LM_SPECIAL);
		special_award ();
	}
}

CALLSET_ENTRY (special_award, start_ball)
{
	lamp_off (LM_SPECIAL);
}

/* Skill Shot */

__local__ U8 skill_shots_collected;
__local__ U8 skill_shot_count;

void skill_shot_enable (void)
{
	global_flag_on (GLOBAL_FLAG_SKILL_SHOT_LIT);
}

void skill_shot_disable (void)
{
	global_flag_off (GLOBAL_FLAG_SKILL_SHOT_LIT);
}

void skill_shot_collect (U8 shot)
{
	if (global_flag_test (GLOBAL_FLAG_SKILL_SHOT_LIT))
	{
		U8 bit = TR_BIT(shot);
		if (!(skill_shots_collected & bit))
		{
			skill_shot_count++;
			/* TBD - points */
			skill_shots_collected |= bit;
		}
		skill_shot_disable ();
	}
}

CALLSET_ENTRY (skill_shot, start_player)
{
	skill_shots_collected = 0;
	skill_shot_count = 0;
}

CALLSET_ENTRY (skill_shot, start_ball)
{
	skill_shot_enable ();
}

CALLSET_ENTRY (skill_shot, valid_playfield)
{
	skill_shot_disable ();
}

