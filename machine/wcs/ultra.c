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

#define NUM_ULTRA_AWARDS 4

U8 ultra_awards_enabled[4];

U8 ultra_award_next;

U8 ultra_awards_finished;


/**
 * The table of ultra modes
 */
static struct ultra_info {
	/* Which lamp indicates the state of the mode */
	U8 lamp;

	/* The number of "shots" required to finish the mode */
	U8 shot_count;

	/* The value of each shot */
	U8 shot_value;

	/* The number of shots that are spotted if the mode is restarted */
	U8 spot;

	/* A pointer to the variable that tracks the number of shots that
	can still be scored.  When zero, the mode is not running */
	U8 *enable;

	/* The speech effect when the mode is started */
	sound_code_t enable_sound;
} ultra_award_table[] = {
	{ LM_ULTRA_RAMPS, 6, SC_250K, 1, &ultra_awards_enabled[0], SPCH_ULTRA_RAMPS },
	{ LM_ULTRA_GOALIE, 6, SC_250K, 1, &ultra_awards_enabled[1], SPCH_ULTRA_GOALIE },
	{ LM_ULTRA_JETS, 50, SC_30K, 10, &ultra_awards_enabled[2], SPCH_ULTRA_JETS },
	{ LM_ULTRA_SPINNER, 30, SC_50K, 5, &ultra_awards_enabled[3], SPCH_ULTRA_SPINNER  }
};


/**
 * Collect an Ultra Mode.  Returns FALSE if the mode is not running and
 * nothing was collected.
 */
bool ultra_collect (struct ultra_info *u)
{
	U8 *enable = u->enable;

	/* Score only when enabled, or when Ultra Mania running */
	if (*enable || flag_test (FLAG_ULTRA_MANIA_RUNNING))
	{
		score (u->shot_value);
	}

	/* Decrement shot counts during normal play */
	if (flag_test (FLAG_ULTRA_MANIA_RUNNING))
	{
	}
	else if (*enable)
	{
		(*enable)--;
		if (*enable == 0)
		{
			lamp_tristate_on (u->lamp);
			ultra_awards_finished++;
			if (ultra_awards_finished == NUM_ULTRA_AWARDS)
			{
				flag_on (FLAG_ULTRA_MANIA_LIT);
				/* TODO - add effects */
			}
		}
		return TRUE;
	}
	else
		return FALSE;
}


/**
 * Score an Ultra Mode.
 */
void ultra_score (struct ultra_info *u)
{
	if (ultra_collect (u))
	{
		music_effect_start (MUS_ULTRA_AWARD, TIME_2S); /* TODO - different sound for spinner/jet */
	}
}

void ultra_add_sound (void)
{
	U16 code = task_get_arg ();
	task_sleep_sec (1);
	speech_start (code, SL_3S);
	task_exit ();
}

/**
 * Start another Ultra Mode.
 *
 * Scan through the list of ultra modes to find a mode that has not been
 * finished yet.  The scan starts from where it left off before, so that
 * all ultra modes will be considered because going back to the beginning
 * again.
 * - If all modes have been completed, ULTRA MANIA is lit and modes cannot
 *   be advanced at all.
 * - If the lamp is SOLID, it has been completed and cannot be awarded
 *   further.
 * - If the lamp is FLASHING, then it is already running.  In this case,
 *   some of the shots are spotted and scored.
 * - Otherwise, if the lamp is OFF, then the mode is started.
 */
void ultra_add_shot (void)
{
	struct ultra_info *u;
	task_pid_t tp;

	if (flag_test (FLAG_ULTRA_MANIA_LIT) || flag_test (FLAG_ULTRA_MANIA_RUNNING))
		return;

	u = ultra_award_table + ultra_award_next;
	while (lamp_test (u->lamp))
	{
		ultra_award_next = (ultra_award_next + 1) % NUM_ULTRA_AWARDS;
		u = ultra_award_table + ultra_award_next;
	}

	if (lamp_flash_test (u->lamp))
	{
		U8 n;
		/* Award a running mode by spotting shots */
		for (n=0; n < u->spot; n++)
			ultra_collect (u);
	}
	else
	{
		/* Award an unlit mode */
		*(u->enable) = u->shot_count;
		lamp_tristate_flash (u->lamp);

		tp = task_create_anon (ultra_add_sound);
		task_set_arg (tp, u->enable_sound);
	}

	ultra_award_next = (ultra_award_next + 1) % NUM_ULTRA_AWARDS;
}


void ultra_mania_start (void)
{
	flag_off (FLAG_ULTRA_MANIA_LIT);
	ultra_awards_finished = 0;
	flag_on (FLAG_ULTRA_MANIA_RUNNING);
	/* effects */
}


CALLSET_ENTRY (ultra, left_ramp_shot, right_ramp_shot)
{
	ultra_score (&ultra_award_table[0]);
}

CALLSET_ENTRY (ultra, sw_goalie_target)
{
	ultra_score (&ultra_award_table[1]);
}

CALLSET_ENTRY (ultra, sw_left_jet, sw_upper_jet, sw_lower_jet)
{
	ultra_score (&ultra_award_table[2]);
}

CALLSET_ENTRY (ultra, sw_spinner_slow)
{
	ultra_score (&ultra_award_table[3]);
}

CALLSET_ENTRY (ultra, left_loop_shot)
{
	if (flag_test (FLAG_ULTRA_MANIA_LIT))
		ultra_mania_start ();
}

CALLSET_ENTRY (ultra, lamp_update)
{
	lamp_on_if (LM_ULTRA_RAMP_COLLECT, lamp_flash_test (LM_ULTRA_RAMPS));
	/* strobe left loop when Ultra Mania lit */
}

CALLSET_ENTRY (ultra, start_player)
{
	ultra_awards_finished = 0;
	lamplist_apply (LAMPLIST_ULTRA_MODES, lamp_off);
}

CALLSET_ENTRY (ultra, start_ball)
{
	lamplist_apply (LAMPLIST_ULTRA_MODES, lamp_flash_off);
	flag_off (FLAG_ULTRA_MANIA_RUNNING);
}

