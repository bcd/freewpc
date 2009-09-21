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

/** Bitmask referring to all 7 standup targets */
#define NO_TARGETS 0x0
#define ALL_TARGETS 0x7f

U8 greed_sound_index;

/** Which default standups are lit */
__local__ U8 default_set;

/** Which standups are lit for the GREED round */
__local__ U8 greed_set;


U8 greed_sounds[] = { 
	SND_GREED_DEFAULT_1,
	SND_GREED_DEFAULT_2,
	SND_GREED_DEFAULT_3,
	SND_GREED_DEFAULT_4,
};

U8 greed_round_timer;


void greed_round_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed6, 64, 5, "GREED");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "SHOOT FLASHING STANDUPS");
		sprintf ("%d", greed_round_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
}


void standup_lamp_update1 (U8 mask, U8 lamp)
{
	if (lamp_test (LM_PANEL_GREED))
	{
		if (greed_set & mask)
		{
			lamp_tristate_flash (lamp);
		}
		else
		{
			lamp_tristate_off (lamp);
		}
	}
	else if (default_set & mask)
	{
		lamp_tristate_off (lamp);
	}
	else
	{
		lamp_tristate_on (lamp);
	}
}

CALLSET_ENTRY (standup, lamp_update)
{
	standup_lamp_update1 (0x1, LM_LL_5M);
	standup_lamp_update1 (0x2, LM_ML_5M);
	standup_lamp_update1 (0x4, LM_UL_5M);
	standup_lamp_update1 (0x8, LM_UR_5M);
	standup_lamp_update1 (0x10, LM_MR1_5M);
	standup_lamp_update1 (0x20, LM_MR2_5M);
	standup_lamp_update1 (0x40, LM_LR_5M);
}


/** target is given as a bitmask */
void common_greed_handler (U8 target)
{
	const U8 sw = task_get_arg ();
	const U8 lamp = switch_lookup_lamp (sw);

	if (lamp_test (LM_PANEL_GREED) && (greed_set & target))
	{
		greed_set &= ~target;
		score (SC_50K);
		sound_send (SND_GREED_ROUND_BOOM);
	}
	else if ((default_set & target) == 0)
	{
		default_set &= ~target;
		score (SC_25K);
		sound_send (SND_THUNDER1);

		if (default_set == NO_TARGETS)
		{
		}
	}
	else
	{
		greed_sound_index++;
		if (greed_sound_index >= 4)
			greed_sound_index = 0;
		sound_send (greed_sounds[greed_sound_index]);
		score (SC_5K);
	}

	standup_lamp_update1 (target, lamp);
}


void greed_round_begin (void)
{
	greed_set = ALL_TARGETS;
	standup_lamp_update ();
	deff_start (DEFF_GREED_ROUND);
}

void greed_round_expire (void)
{
	deff_stop (DEFF_GREED_ROUND);
}

void greed_round_end (void)
{
	greed_set = NO_TARGETS;
	standup_lamp_update ();
}

void greed_round_task (void)
{
	timed_mode_task (greed_round_begin, greed_round_expire, greed_round_end,
		&greed_round_timer, 20, 3);
}

CALLSET_ENTRY (greed, display_update)
{
	if (timed_mode_timer_running_p (GID_GREED_ROUND_RUNNING,
		&greed_round_timer))
		deff_start_bg (DEFF_GREED_ROUND, 0);
}

CALLSET_ENTRY (greed, music_refresh)
{
	if (timed_mode_timer_running_p (GID_GREED_ROUND_RUNNING,
		&greed_round_timer))
		music_request (MUS_GREED_ROUND, PRI_GAME_MODE1);
}

CALLSET_ENTRY (greed, door_start_greed)
{
	timed_mode_start (GID_GREED_ROUND_RUNNING, greed_round_task);
}

CALLSET_ENTRY (greed, end_ball)
{
	timed_mode_stop (&greed_round_timer);
}

CALLSET_ENTRY (greed, start_player)
{
	/* Light all 7 'default' lamps */
	default_set = ALL_TARGETS;
	greed_set = NO_TARGETS;
	standup_lamp_update ();
}


CALLSET_ENTRY (greed, sw_standup_1)
{
	common_greed_handler (0x1);
}

CALLSET_ENTRY (greed, sw_standup_2)
{
	common_greed_handler (0x2);
}

CALLSET_ENTRY (greed, sw_standup_3)
{
	common_greed_handler (0x4);
}

CALLSET_ENTRY (greed, sw_standup_4)
{
	common_greed_handler (0x8);
}

CALLSET_ENTRY (greed, sw_standup_5)
{
	common_greed_handler (0x10);
}

CALLSET_ENTRY (greed, sw_standup_6)
{
	common_greed_handler (0x20);
}

CALLSET_ENTRY (greed, sw_standup_7)
{
	common_greed_handler (0x40);
}


