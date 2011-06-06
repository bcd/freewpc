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
#ifdef __m6809__
#include <m6809/math.h>
#endif

U8 balls_served;

bool faster_quote_given;

/** Filename: mach/config.c
 *
 * Machine-specific miscellaneous functions.
 */

static inline U8 decimal_to_bcd_byte (U8 decimal)
{
#ifdef __m6809__
	U8 quot, rem;
	DIV10 (decimal, quot, rem);
	return (quot << 4) + rem;
#else
	return ((decimal / 10) << 4) + (decimal % 10);
#endif
}


void replay_code_to_score (score_t s, U8 val)
{
	
		s[1] = decimal_to_bcd_byte (val * 10);
}


CALLSET_ENTRY (tz, start_ball)
{
	faster_quote_given = 0;
}


CALLSET_ENTRY (tz, add_player)
{
#ifdef CONFIG_TZONE_IP
	if (num_players > 1)
		sound_send (SND_PLAYER_ONE + num_players - 1);
#endif
}


CALLSET_ENTRY (tz, bonus)
{
	deff_start (DEFF_BONUS);
	leff_start (LEFF_BONUS);
	task_sleep_sec (1);
	while (deff_get_active () == DEFF_BONUS)
		task_sleep (TIME_66MS);
	leff_stop (LEFF_BONUS);
}

CALLSET_ENTRY (tz, tilt)
{
	sound_send (SND_TILT);
	task_sleep_sec (3);
	sound_send (SND_OH_NO);
}

CALLSET_ENTRY (tz, tilt_warning)
{
	sound_send (SND_TILT_WARNING);
}

CALLSET_ENTRY (tz, start_without_credits)
{
	if (!timer_find_gid (GID_START_NO_CREDITS_DEBOUNCE))
	{
		timer_restart_free (GID_START_NO_CREDITS_DEBOUNCE, TIME_5S);
		sound_send (SND_GREEEED);
	}
}

CALLSET_ENTRY (tz, timed_game_tick)
{
	if (!in_live_game || in_bonus)
		return;
	switch (timed_game_timer)
	{
		case 10: 
			if (faster_quote_given == 0)
				sound_send (SND_FASTER); 
			faster_quote_given = 1;
			break;
		case 5: sound_send (SND_FIVE); break;
		case 4: sound_send (SND_FOUR); break;
		case 3: sound_send (SND_THREE); break;
		case 2: sound_send (SND_TWO); break;
		case 1: sound_send (SND_ONE); break;
		default: break;
	}
}

/* Hack to disable ballsave after first ball_serve */
CALLSET_ENTRY (config, start_ball)
{
	balls_served = 0;
}

/* Reset the count so we can have a ballsave on the next ball only */
CALLSET_ENTRY (config, mball_start)
{
	balls_served = 1;
}


CALLSET_ENTRY (config, serve_ball)
{
	balls_served++;	
}

