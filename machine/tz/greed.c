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
/* TODO Change so hitting standups increases jackpot
 * jackpot is collected by hitting slot machine 
 * Suggested by litz 
 *
 * Hitting green clock target shifts the lit lamps one step to the left and wraps 
 * */

/* CALLSET_SECTION (greed, __machine3__) */
#include <freewpc.h>

void greed_mode_init (void);
void greed_mode_exit (void);

/** Bitmask referring to all 7 standup targets */
#define NO_TARGETS 0x0
#define ALL_TARGETS 0x7f

U8 greed_sound_index;

score_t greed_mode_total;

/** Which default standups are lit */
__local__ U8 default_set;

/** Which standups are lit for the GREED mode */
__local__ U8 greed_set;


U8 greed_sounds[] = { 
	SND_GREED_DEFAULT_1,
	SND_GREED_DEFAULT_2,
	SND_GREED_DEFAULT_3,
	SND_GREED_DEFAULT_4,
};

U8 greed_mode_timer;

struct timed_mode_ops greed_mode = {
	DEFAULT_MODE,
	.init = greed_mode_init,
	.exit = greed_mode_exit,
	.gid = GID_GREED_MODE_RUNNING,
	.music = MUS_GREED_MODE,
	.deff_running = DEFF_GREED_MODE,
	.deff_ending = DEFF_GREED_MODE_TOTAL,
	.prio = PRI_GAME_MODE2,
	.init_timer = 40,
	.timer = &greed_mode_timer,
	.grace_timer = 3,
	.pause = system_timer_pause,
};

/* Note:  The oddchange_deff handles the hits during greed mode */
void greed_mode_deff (void)
{
	dmd_alloc_pair_clean ();
	U16 fno;
	for (;;)
	{
		U8 i = 0;
		for (fno = IMG_GREED_START; fno <= IMG_GREED_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_center (&font_fixed6, 64, 5, "GREED");
			i++;
			if (i > 1)
			{
				sprintf_score (greed_mode_total);
				font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
			}
			
			if (i > 2)
			{
				i = 0;
			}
			
			font_render_string_center (&font_var5, 64, 27, "SHOOT FLASHING STANDUPS");
			sprintf ("%d", greed_mode_timer);
			font_render_string (&font_var5, 2, 2, sprintf_buffer);
			font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			callset_invoke (score_overlay);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
}

void greed_mode_total_deff (void)
{
	sound_send (SND_SEE_WHAT_GREED);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 5, "GREED OVER");
	sprintf_score (greed_mode_total);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	font_render_string_center (&font_var5, 64, 27, "POINTS EARNED FROM MODE");
	dmd_show_low ();
	task_sleep_sec (4);
	deff_exit ();
}

/* Award 500K for a clock hit during Greed and cycle lamps */

static inline void rotate_greed_lamps (void)
{
	greed_set = (greed_set >> 1) | (greed_set << (7 - 1));
}

CALLSET_ENTRY (greed, sw_clock_target)
{
	if (timed_mode_running_p (&greed_mode))
	{
		score_add (greed_mode_total, score_table[SC_500K]);
		rotate_greed_lamps ();
	}
}

void standup_lamp_update1 (U8 mask, U8 lamp)
{
	if (timed_mode_running_p (&greed_mode))
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
	/* Increase the odcchange pot, which also shows the deff */
	if (in_live_game)
		callset_invoke (grow_oddchange);

	const U8 sw = task_get_arg ();
	const U8 lamp = switch_lookup_lamp (sw);

	if (timed_mode_running_p(&greed_mode) && (greed_set & target))
	{
		greed_set &= ~target;
		score (SC_5M);
		score_add (greed_mode_total, score_table[SC_5M]);
		sound_send (SND_GREED_MODE_BOOM);
	}
	else if ((default_set & target) == 0)
	{
		default_set &= ~target;
		score (SC_1M);
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
		score (SC_25K);
	}

	standup_lamp_update1 (target, lamp);
}


void greed_mode_init (void)
{
	greed_set = ALL_TARGETS;
	standup_lamp_update ();
	deff_start (DEFF_GREED_MODE);
	score_zero (greed_mode_total);
}

void greed_mode_expire (void)
{
	if (score_compare (score_table[SC_10M], greed_mode_total) == 1)
		callset_invoke (start_hurryup);
}

void greed_mode_exit (void)
{
	greed_set = NO_TARGETS;
	standup_lamp_update ();
}

CALLSET_ENTRY (greed, display_update)
{
	timed_mode_display_update (&greed_mode);
}

CALLSET_ENTRY (greed, music_refresh)
{
	timed_mode_music_refresh (&greed_mode);
}

CALLSET_ENTRY (greed, end_ball)
{
	timed_mode_end (&greed_mode);
}


CALLSET_ENTRY (greed, door_start_greed)
{
	timed_mode_begin (&greed_mode);
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
