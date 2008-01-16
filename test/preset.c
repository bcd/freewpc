/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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
#include <window.h>
#include <preset.h>

#define PRESET_BEGIN(name) \
struct preset_component preset_ ## name ## _comps[] = {

#define PRESET_END(N, string) \
	{ NULL, 0 }, \
}; \
struct preset preset_ ## N = {  \
	.name = string,  \
	.comps = preset_ ## N ## _comps \
};

extern struct adjustment standard_adjustments[];
extern struct adjustment feature_adjustments[];
extern struct adjustment pricing_adjustments[];


PRESET_BEGIN (3ball)
	{ standard_adjustments, &system_config.balls_per_game, 3 },
PRESET_END (3ball, "3-BALL")


PRESET_BEGIN (5ball)
	{ standard_adjustments, &system_config.balls_per_game, 5 },
PRESET_END (5ball, "5-BALL")


PRESET_BEGIN (tournament)
	{ standard_adjustments, &system_config.balls_per_game, 3 },
	{ standard_adjustments, &system_config.replay_award, FREE_AWARD_OFF },
	{ standard_adjustments, &system_config.special_award, FREE_AWARD_OFF },
	{ pricing_adjustments, &price_config.free_play, YES },
	{ standard_adjustments, &system_config.game_restart, GAME_RESTART_NEVER },
	{ standard_adjustments, &system_config.max_ebs, 0 },
	{ standard_adjustments, &system_config.match_feature, OFF },
	{ standard_adjustments, &system_config.tournament_mode, ON },
	{ standard_adjustments, &system_config.no_bonus_flips, NO },
	{ pricing_adjustments, &price_config.one_coin_buyin, OFF },
	{ feature_adjustments, &system_config.buy_extra_ball, NO },
PRESET_END (tournament, "TOURNAMENT")


PRESET_BEGIN (show)
	{ pricing_adjustments, &price_config.free_play, YES },
	{ standard_adjustments, &system_config.replay_award, FREE_AWARD_OFF },
	{ standard_adjustments, &system_config.special_award, FREE_AWARD_OFF },
	{ standard_adjustments, &system_config.match_feature, OFF },
	{ pricing_adjustments, &price_config.one_coin_buyin, OFF },
PRESET_END (show, "SHOW")


PRESET_BEGIN (timed_game)
	{ standard_adjustments, &system_config.max_players, 1 },
PRESET_END (timed_game, "TIMED GAME")


PRESET_BEGIN (american)
	{ standard_adjustments, &system_config.euro_digit_sep, NO },
	{ standard_adjustments, &system_config.date_style, 0 },
PRESET_END (american, "AMERICAN")


PRESET_BEGIN (french)
	{ standard_adjustments, &system_config.euro_digit_sep, YES },
	{ standard_adjustments, &system_config.date_style, 1 },
PRESET_END (french, "FRENCH")


PRESET_BEGIN (german)
	{ standard_adjustments, &system_config.euro_digit_sep, YES },
	{ standard_adjustments, &system_config.date_style, 1 },
PRESET_END (german, "GERMAN")


struct preset *preset_table[] = {
	/* Easy-Hard */
	/* Add-a-Ball */
	/* Ticket */
	/* Novelty */
	/* Serial Capture */
	/* German 1-6 */
	/* French 1-6 */
	&preset_3ball,
	&preset_5ball,
	&preset_tournament,
	&preset_show,
	&preset_timed_game,
	&preset_french,
	&preset_german,
};


U8 preset_count (void)
{
	return sizeof (preset_table) / sizeof (struct preset *);
}

void preset_install (struct preset_component *comps)
{
	/* Modify all of the adjustments affected by the preset */
	wpc_nvram_get ();
	while (comps->nvram != NULL)
	{
		*(comps->nvram) = comps->value;
		comps++;
	}
	wpc_nvram_put ();

	/* Update the checksums to match the new values */
	adj_modified ();
}


void preset_install_from_test (void)
{
	struct preset *pre = preset_table[menu_selection];
	preset_install (pre->comps);
}


/** Install the presets for a particular country.
 * The code is determined from the DIP switches. */
void preset_install_country_code (U8 code)
{
	dbprintf ("Installing preset for country code %d\n", code);
	switch (code)
	{
		case WPC_JUMPER_USA_CANADA:
		case WPC_JUMPER_USA_CANADA2:
			preset_install (preset_american_comps);
			break;

		case WPC_JUMPER_FRANCE:
		case WPC_JUMPER_FRANCE2:
		case WPC_JUMPER_FRANCE3:
		case WPC_JUMPER_FRANCE4:
			preset_install (preset_french_comps);
			break;

		case WPC_JUMPER_GERMANY:
			preset_install (preset_german_comps);
			break;

		case WPC_JUMPER_EXPORT_ENGLISH:
		case WPC_JUMPER_EXPORT:
		case WPC_JUMPER_UK:
		case WPC_JUMPER_EUROPE:
			break;

		case WPC_JUMPER_SPAIN:
			break;
	}
}


void preset_render_name (U8 index)
{
	sprintf ("%s", preset_table[index]->name);
}


bool preset_installed_p (U8 index)
{
	struct preset *pre = preset_table[index];
	struct preset_component *comps = pre->comps;

	while (comps->nvram != NULL)
	{
		if (*(comps->nvram) != comps->value)
			return FALSE;
		comps++;
	}
	return TRUE;
}


void preset_show_components (void)
{
	struct preset *pre = preset_table[menu_selection];
	struct preset_component *comps = pre->comps;

	dmd_alloc_low_clean ();
	dmd_sched_transition (&trans_scroll_left);
	font_render_string_center (&font_mono5, 64, 5, pre->name);
	dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 11);
	sound_send (SND_TEST_SCROLL);
	dmd_show_low ();
	task_sleep (TIME_1S + TIME_500MS);

	while (comps->nvram != NULL)
	{
		struct adjustment *info = comps->info;
		dmd_alloc_low_clean ();
		font_render_string_center (&font_mono5, 64, 5, pre->name);
		dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 11);

		if (info == NULL)
		{
			sprintf ("SET %p TO %02X", comps->nvram, comps->value);
			font_render_string_center (&font_mono5, 64, 16, sprintf_buffer);
		}
		else
		{
			union dmd_coordinate coord;
			/* WARNING - you cannot make a farcall when some of the args
			 * are on the stack!  The called function sees the saved
			 * value of the page instead of the args.  This has to be
			 * fixed in the compiler itself.
			 * So for now, we split into two function calls... */
			adj_prepare_lookup (info);
			adj_name_for_preset (comps->nvram, comps->value);
			coord.x = 24;
			coord.y = 24;
			if (*comps->nvram == comps->value)
			{
				bitmap_draw (coord, BM_X5);
			}
			else
			{
				bitmap_draw (coord, BM_BOX5);
			}
		}

		dmd_show_low ();
		sound_send (SND_TEST_CONFIRM);
		task_sleep (TIME_1S + TIME_500MS);
		comps++;
	}
}

