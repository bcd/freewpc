/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

/*
 * This file implements a single 'challenge ruleset' for Attack From Mars.
 * The goal is not a game that you can play all day, but something that will average
 * 3-5 minutes, or maybe 10 minutes if you are really good.
 * The basic rules are as follows:
 *
 * A single shot is lit at any time for 'big points'.  Other shots like targets,
 * bumpers, etc. may score small points but scoring is dominated by the One Lit Shot.
 *
 * The game is timed.  You need to make all 7 main shots once to complete a 'phase', then
 * the timer is reset, but smaller than before, so it gets harder to finish each time.
 *
 * You are allowed to drain the ball 2 times without penalty; on the third drain, the
 * game ends regardless of the timer.
 *
 * When the timer expires, you go into Sudden Death.  Draining now will end the game,
 * but if you have a ball save left, completing a certain number of martian targets
 * will cost you 1 ball save and restore the timer to its original value.
 *
 * The shots to make move from left to right on the first phase, then right to left for
 * the next, etc.
 *
 * Bonus points are awarded at the completion of each phase for any time remaining,
 * comparable to the main scoring so this is desirable.
 *
 * Only 1-player games are supported.
 *
 * TODO:
 * - Award Extra Ball Save after completing Phase 1 & 3.
 * - Add Extra Time rule.  Completing all 7 martian targets (except in Sudden Death) will
     reset the timer to its original value for that round.
 * - Add fast scoring rule.  Value for each shot = base value (100 currently) + bonus for
     how much time is left.  Because timer gets shorter every phase, bonus is
	  (time_left * phase_number).
 * - Pause the timer briefly anytime a "long shot" like a ramp or loop is made, not
     necessarily so long until the player gets the ball back, but just a little time bonus
	  for hitting _something_.
 * - Add Bonus Wave after completion of every other phase.  This is a 30 second timed
     multiball mode where all standups (Martians and Center Bank) score 1 point each, and
	  completing ALL 10 targets scores a Bonus Jackpot equal to the number of points scored
	  already in the round.
 * - Add Saucer rule.  After making the Forcefield shot in each phase, lower the 3-bank to
     expose the Saucer drop.  Hit the drop target N times (increasing for each phase) to
	  spot the next shot.
 * - Add Attract mode rules display explaining how it works.
 */

#include <freewpc.h>

__local__ U8 chal_level;
__local__ U8 chal_shot_lit;
__local__ S8 chal_shot_direction;
__local__ S16 chal_timer;
__local__ U8 chal_timer_reset_value;
__local__ U8 martians_max;
__local__ U8 ball_saves;

score_t chal_shot_score;

U8 martians_needed;


#define TIMER_MULT 8

enum {
	CS_LEFT_LOOP = 0,
	CS_LEFT_RAMP,
	CS_CENTER_RAMP,
	CS_FORCEFIELD,
	CS_RIGHT_HOLE,
	CS_RIGHT_RAMP,
	CS_RIGHT_LOOP,
};

U8 chal_shot_lamplists[] = {
	[CS_LEFT_LOOP] = LAMPLIST_L_LOOP_ALL,
	[CS_LEFT_RAMP] = LAMPLIST_L_RAMP_ALL,
	[CS_CENTER_RAMP] = LAMPLIST_LOCK_LANE,
	[CS_FORCEFIELD] = LAMPLIST_MOTOR_BANK,
	[CS_RIGHT_HOLE] = LAMPLIST_RIGHT_HOLE,
	[CS_RIGHT_RAMP] = LAMPLIST_R_RAMP_ALL,
	[CS_RIGHT_LOOP] = LAMPLIST_R_LOOP_ALL,
};

const char *chal_shot_names[] = {
	[CS_LEFT_LOOP] = "LEFT LOOP",
	[CS_LEFT_RAMP] = "LEFT RAMP",
	[CS_CENTER_RAMP] = "CENTER RAMP",
	[CS_FORCEFIELD] = "CENTER TARGETS",
	[CS_RIGHT_HOLE] = "RIGHT HOLE",
	[CS_RIGHT_RAMP] = "RIGHT RAMP",
	[CS_RIGHT_LOOP] = "RIGHT LOOP",
};

void chal_render_static (void)
{
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_var5, 16, 4, "TIME");
	sprintf ("%d SAVES", ball_saves);
	font_render_string_center (&font_var5, 16, 29, sprintf_buffer);

	sprintf ("LEVEL %d", chal_level+1);
	font_render_string_center (&font_var5, 80, 29, sprintf_buffer);

	sprintf ("SHOOT %s", chal_shot_names[chal_shot_lit]);
	font_render_string_center (&font_var5, 80, 4, sprintf_buffer);
}

void chal_running_deff (void)
{
	chal_render_static ();
	for (;;)
	{
		dmd_alloc_low ();
		pinio_dmd_window_set (PINIO_DMD_WINDOW_1, DMD_OVERLAY_PAGE);
		dmd_copy_page (dmd_low_buffer, dmd_high_buffer);
		pinio_dmd_window_set (PINIO_DMD_WINDOW_1,
			pinio_dmd_window_get (PINIO_DMD_WINDOW_0));

		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 80, 16, sprintf_buffer);

		sprintf ("%ld", chal_timer / 8);
		font_render_string_center (&font_fixed10, 16, 16, sprintf_buffer);

		dmd_show_low ();
		task_sleep (TIME_250MS);
	}
}

void chal_award_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("LEVEL %d AWARD", chal_level);
	font_render_string_center (&font_var5, 64, 8, sprintf_buffer);
	sprintf_score (score_deff_get ());
	font_render_string_center (&font_fixed6, 64, 18, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_1500MS);
	deff_exit ();
}

void chal_ball_drain_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();
	font_render_string_center (&font_fixed6, 64, 8, "BALL SAVED");
	task_sleep (TIME_1500MS);
	deff_exit ();
}

void chal_timeout_endgame_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "OUT OF TIME");
	dmd_show_low ();
	task_sleep (TIME_1500MS);
	deff_exit ();
}

void chal_all_shots_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 9, "ALL SHOTS MADE");
	sprintf ("TIMER AT %d SEC.", chal_timer_reset_value / 8);
	font_render_string_center (&font_fixed6, 64, 22, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_2S);
	deff_exit ();
}

void chal_sudden_death_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_var5, 64, 6, "SUDDEN DEATH");
	font_render_string_center (&font_var5, 64, 27, "TO CONTINUE GAME");
	dmd_copy_low_to_high ();
	sprintf ("SHOOT %d MARTIANS", martians_needed);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	for (;;)
	{
		dmd_show_other ();
		task_sleep (TIME_250MS);
	}
}

void martian_kill_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	sprintf ("%d", martians_needed);
	font_render_string_center (&font_lucida9, 64, 16, sprintf_buffer);
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	dmd_show_low ();
	task_sleep (TIME_200MS);
	dmd_show_other ();
	task_sleep (TIME_500MS);
	deff_exit ();
}

void gi_strobe_leff (void)
{
	U8 i;
	gi_leff_disable (PINIO_GI_STRINGS);
	for (;;)
	{
		for (i=0; i < 3; i++)
		{
			gi_leff_enable (TRIAC_GI_STRING (i));
			task_sleep (TIME_300MS);
			gi_leff_disable (TRIAC_GI_STRING (i));
		}
	}
}

void gi_flash_leff (void)
{
	for (;;)
	{
		gi_leff_enable (PINIO_GI_STRINGS);
		task_sleep (TIME_500MS);
		gi_leff_disable (PINIO_GI_STRINGS);
		task_sleep (TIME_500MS);
	}
}

void chal_level_lamp_set (void)
{
	lamplist_apply (chal_shot_lamplists[chal_shot_lit], lamp_flash_on);
}

void chal_level_lamp_reset (void)
{
	if (martians_needed == 0)
	{
		lamplist_apply (chal_shot_lamplists[chal_shot_lit], lamp_flash_off);
		lamplist_apply (chal_shot_lamplists[chal_shot_lit], lamp_on);
	}
}

void chal_level_lamp_erase (void)
{
	U8 i;
	for (i=0; i < 7; i++)
	{
		lamplist_apply (chal_shot_lamplists[i], lamp_flash_off);
		lamplist_apply (chal_shot_lamplists[i], lamp_off);
	}
}

void chal_endgame (void)
{
	flipper_disable ();
	leff_start (LEFF_TILT);
	in_tilt = TRUE;
	callset_invoke (tilt);
}

void chal_timer_task (void)
{
	leff_stop (LEFF_GI_FLASH);
	leff_start (LEFF_GI_STROBE);
	task_sleep (TIME_500MS);
	while (chal_timer > 0)
	{
		do {	
			task_sleep (TIME_166MS);
		} while (system_timer_pause ());
		chal_timer--;
		task_sleep (TIME_166MS);
		chal_timer--;
		task_sleep (TIME_166MS);
		chal_timer--;
	}
	task_sleep (TIME_1500MS);
	if (ball_saves)
	{
		ball_saves--;
		martians_needed = martians_max;
		if (martians_max < 20)
			martians_max += 5;
		lamplist_apply (LAMPLIST_MARTIANS, lamp_flash_on);
		lamplist_apply (chal_shot_lamplists[chal_shot_lit], lamp_flash_off);
		leff_stop (LEFF_GI_STROBE);
		leff_start (LEFF_GI_FLASH);
	}
	else
	{
		chal_endgame ();
	}
	task_exit ();
}

void chal_timer_reset (void)
{
	chal_timer = chal_timer_reset_value;
	martians_needed = 0;
	task_recreate_gid (GID_CHALLENGE_TIMER, chal_timer_task);
}

void chal_finish_phase (void)
{
	chal_level_lamp_erase ();

	/* Reverse the direction of the shots */
	chal_shot_direction = -chal_shot_direction;

	/* Reset the next shot lit: it is the same as the last one made */
	chal_shot_lit += chal_shot_direction;

	/* Decrease the timer starting value.  The sequence is:
	30, 25, 20, 15 */
	if (chal_timer_reset_value > 15 * TIMER_MULT)
		chal_timer_reset_value -= 5 * TIMER_MULT;

	/* TODO - because the last shot is always a loop shot, catch it
	in the bumpers and hopefully it will fall into the SoL hole so
	that the finish effects can play out.  Maybe disable the jets
	during this time to increase likelihood... */
}

void chal_advance_level (void)
{
	chal_level_lamp_reset ();

	score_copy (chal_shot_score, score_table[SC_100]);
	score_long (chal_shot_score);
	/* TODO : time bonus */

	deff_start (DEFF_CHAL_AWARD);
	chal_level++;
	chal_shot_lit += chal_shot_direction;

	if (chal_shot_lit == 7 || chal_shot_lit == 0xFF)
	{
		deff_start (DEFF_CHAL_PHASE_FINISHED);
		chal_finish_phase ();
	}

	/* Timer resets on every shot made */
	chal_timer_reset ();

	dbprintf ("lit = %d, dir = %d\n", chal_shot_lit, chal_shot_direction);
	chal_level_lamp_set ();
}

void chal_collect (U8 shot)
{
	if (shot == chal_shot_lit && !martians_needed)
	{
		chal_advance_level ();
	}
}

CALLSET_ENTRY (chal, left_loop_shot)
{ chal_collect (CS_LEFT_LOOP); }
CALLSET_ENTRY (chal, left_ramp_shot)
{ chal_collect (CS_LEFT_RAMP); }
CALLSET_ENTRY (chal, center_ramp_shot)
{ chal_collect (CS_CENTER_RAMP); }
CALLSET_ENTRY (chal, any_motor_bank)
{ chal_collect (CS_FORCEFIELD); }
CALLSET_ENTRY (chal, dev_right_hole_enter)
{ chal_collect (CS_RIGHT_HOLE); }
CALLSET_ENTRY (chal, right_ramp_shot)
{ chal_collect (CS_RIGHT_RAMP); }
CALLSET_ENTRY (chal, right_loop_shot)
{ chal_collect (CS_RIGHT_LOOP); }

CALLSET_ENTRY (chal, any_martian)
{
	if (martians_needed)
	{
		deff_start (DEFF_MARTIAN_KILL);
		task_sleep (TIME_33MS);
		martians_needed--;
		if (!martians_needed)
		{
			chal_level_lamp_set ();
			lamplist_apply (LAMPLIST_MARTIANS, lamp_flash_off);
			chal_timer_reset ();
		}
	}
}

CALLSET_ENTRY (chal, device_update)
{
	/* TODO - loop gates should remain open except
	on the final shot of the phase, to divert the ball
	briefly to allow the end phase effects to be seen */
	/* TODO - motor bank should remain up */
	/* TODO - lock should open for first level to make
	the shot easier */
}

CALLSET_ENTRY (chal, display_update)
{
	if (in_live_game && valid_playfield)
	{
		if (martians_needed)
			deff_start_bg (DEFF_CHAL_SUDDEN_DEATH, PRI_GAME_MODE1);
		else
			deff_start_bg (DEFF_CHAL_RUNNING, PRI_GAME_MODE1);
	}
}

CALLSET_ENTRY (chal, start_player)
{
	ball_saves = 2;
	chal_level = 0;
	chal_level_lamp_set ();
	chal_shot_lit = 0;
	chal_shot_direction = 1;
	martians_max = 5;
	chal_timer_reset_value = 30U * TIMER_MULT;
	chal_timer_reset ();
}

CALLSET_ENTRY (chal, start_ball)
{
	task_kill_gid (GID_CHALLENGE_TIMER);
}

CALLSET_ENTRY (chal, valid_playfield)
{
	chal_timer_reset ();
	ball_up = system_config.balls_per_game;
}

CALLSET_BOOL_ENTRY (chal, ball_drain)
{
	if (mb_mode_running_count ())
	{
		dbprintf ("Multiball drain\n");
		return TRUE;
	}
	else if (martians_needed)
	{
		dbprintf ("Sudden death drain\n");
		return TRUE;
	}
	else if (ball_saves > 0)
	{
		dbprintf ("Ball saved\n");
		ball_saves--;
		serve_ball_auto ();
		return FALSE;
	}
	else
	{
		dbprintf ("No ball saves\n");
		return TRUE;
	}
}

