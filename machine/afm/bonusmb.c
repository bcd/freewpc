/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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

/* An example of how to use the multiball mode module.
	For this simple 2-ball multiball, shoot the center 3-bank
	10 times to start, then continue to hit to score jackpots. */

enum mb_mode_state bonus_mb_state;

U8 mb_total_shots_needed;

U8 mb_shots_to_start;

U8 mb_jackpot_count;

static void bonus_mb_jackpot (void)
{
	mb_jackpot_count++;
	score (SC_10);
}

static void afm_bonus_mb_update (enum mb_mode_state state)
{
	switch (state)
	{
		case MB_INACTIVE:
			mb_jackpot_count = 0;
			/* fallthru for effects */

		case MB_IN_GRACE:
			lamp_tristate_off (LM_LOCK_JACKPOT);
			break;

		case MB_ACTIVE:
			lamp_tristate_on (LM_LOCK_JACKPOT);
			set_ball_count (2);
			break;
	}
}

struct mb_mode_ops afm_bonus_mb_mode =
{
	DEFAULT_MBMODE,
	.state = &bonus_mb_state,
	.update = afm_bonus_mb_update,
	.deff_running = DEFF_BONUSMB_RUNNING,
	.deff_ending = DEFF_BONUSMB_ENDING,
	.prio = PRI_GAME_MODE3,
	.gid_running = GID_BONUS_MB,
	.gid_in_grace = GID_BONUS_MB_GRACE,
	.grace_period = TIME_3S,
};

void bonusmb_running_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "BONUS MB");
	dmd_show_low ();
	for (;;)
		task_sleep (TIME_4S);
}

void bonusmb_ending_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "BONUS MB OVER");
	dmd_show_low ();
	task_sleep (TIME_2S);
	deff_exit ();
}

CALLSET_ENTRY (bonusmb, start_player)
{
	mb_total_shots_needed = 10;
	mb_shots_to_start = 10;
}

CALLSET_ENTRY (bonusmb, any_motor_bank)
{
	if (mb_mode_running_p (&afm_bonus_mb_mode))
	{
		bonus_mb_jackpot ();
	}
	else
	{
		mb_shots_to_start--;
		if (mb_shots_to_start == 0)
		{
			mb_mode_start (&afm_bonus_mb_mode);
			if (mb_total_shots_needed < 50)
				mb_total_shots_needed += 10;
			mb_shots_to_start = mb_total_shots_needed;
		}
	}
}

CALLSET_ENTRY (bonusmb, start_ball)
{
	bonus_mb_state = MB_INACTIVE;
}

CALLSET_ENTRY (bonusmb, single_ball_play)
{
	mb_mode_single_ball (&afm_bonus_mb_mode);
}

CALLSET_ENTRY (bonusmb, end_ball)
{
	mb_mode_end_ball (&afm_bonus_mb_mode);
}

CALLSET_ENTRY (bonusmb, display_update)
{
	mb_mode_display_update (&afm_bonus_mb_mode);
}

CALLSET_ENTRY (bonusmb, music_refresh)
{
	mb_mode_music_refresh (&afm_bonus_mb_mode);
}

#if 1
CALLSET_ENTRY (bonusmb, center_ramp_shot)
{
	mb_mode_start (&afm_bonus_mb_mode);
}
#endif
