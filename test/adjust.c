/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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
#include <test.h>
#include <format.h>
#include <text.h>

#if (MACHINE_DMD == 1)
extern __test__ void print_row_center (const font_t *f, U8 row);
#else
extern __test__ void print_row_center1 (U8 row);
#define print_row_center(f, row) print_row_center1 (row)
#endif

struct adjustment current_adjustment;

struct adjustment_value current_adjustment_value;

struct adjustment *current_adjustment_set;

struct adjustment_value integer_value = { 0, 0xFF, 1, decimal_render };
struct adjustment_value hex_integer_value = { 0, 0xFF, 1, hexadecimal_render };
struct adjustment_value credit_count_value = { 0, 4, 1, decimal_render };
struct adjustment_value nonzero_integer_value = { 1, 0xFF, 1, decimal_render };
struct adjustment_value balls_per_game_value = { 1, 10, 1, decimal_render };
struct adjustment_value players_per_game_value = { 1, MAX_PLAYERS, 1, decimal_render };
struct adjustment_value max_eb_value = { 0, 10, 1, decimal_render };
struct adjustment_value on_off_value = { 0, 1, 1, on_off_render };
struct adjustment_value yes_no_value = { 0, 1, 1, yes_no_render };
struct adjustment_value game_restart_value = { 0, 2, 1, game_restart_render };
struct adjustment_value max_credits_value = { 5, 99, 1, decimal_render };
struct adjustment_value hs_reset_value = { 0, 80, 1, hs_reset_render };
struct adjustment_value clock_style_value = { 0, 1, CLOCK_STYLE_AMPM,
	clock_style_render };
struct adjustment_value date_style_value = { 0, 1, DATE_STYLE_US,
	date_style_render };
struct adjustment_value score_value = { 0, 250, 10, decimal_render };
struct adjustment_value lang_value = { 0, 0, 0, lang_render };
struct adjustment_value replay_system_value = { REPLAY_FIXED, REPLAY_AUTO, 1, replay_system_render };
struct adjustment_value free_award_value = { 0, 4, 1, free_award_render };
struct adjustment_value percent_value = { 0, 100, 1, percent_render };
struct adjustment_value collection_text_value = { 0, NUM_CURRENCY_TYPES-1, 1,
	collection_text_render };
struct adjustment_value printer_type_value = { 0, 4, 1, printer_type_render };
struct adjustment_value baud_rate_value = { 0, 5, 1, baud_rate_render };
struct adjustment_value lines_per_page_value = { 22, 80, 1, decimal_render };
struct adjustment_value pricing_mode_value = { 0, NUM_PRICING_MODES-1, 1, pricing_mode_render };
struct adjustment_value coin_door_type_value = { 0, NUM_COIN_DOOR_TYPES-1, 1, coin_door_render };

#ifndef MACHINE_REPLAY_SCORE_CHOICES
#define MACHINE_REPLAY_SCORE_CHOICES 250
#endif
#ifndef MACHINE_REPLAY_START_CHOICE
#define MACHINE_REPLAY_START_CHOICE 0
#endif
struct adjustment_value replay_score_value = {
	0, MACHINE_REPLAY_SCORE_CHOICES-1, 1, replay_score_render
};

struct adjustment_value max_tickets_value = { 0, 100, 1, decimal_render };
struct adjustment_value gi_power_saver_value = { 0, 60, 1, minutes_render };
struct adjustment_value power_saver_level_value = { 4, 7, 1, brightness_render };

struct adjustment standard_adjustments[] = {
	{ "BALLS PER GAME", &balls_per_game_value, 3, &system_config.balls_per_game },
	{ "MAX PLAYERS", &players_per_game_value, MAX_PLAYERS, &system_config.max_players },
	{ "TILT WARNINGS", &balls_per_game_value, 3, &system_config.tilt_warnings },
	{ "MAX E.B.", &max_eb_value, 5, &system_config.max_ebs },
	{ "MAX E.B. PER B.I.P.", &max_eb_value, 4, &system_config.max_ebs_per_bip },
	{ "REPLAY SYSTEM", &replay_system_value, REPLAY_FIXED, &system_config.replay_system },
	{ "REPLAY PERCENT", &percent_value, 7, &system_config.replay_percent },
	{ "REPLAY START", &replay_score_value, MACHINE_REPLAY_START_CHOICE, &system_config.replay_start },
	{ "REPLAY LEVELS", &integer_value, 1, &system_config.replay_levels },
	{ "REPLAY 1 LEVEL", &replay_score_value, MACHINE_REPLAY_START_CHOICE, &system_config.replay_level[0] },
	{ "REPLAY 2 LEVEL", &replay_score_value, 0, &system_config.replay_level[1] },
	{ "REPLAY 3 LEVEL", &replay_score_value, 0, &system_config.replay_level[2] },
	{ "REPLAY 4 LEVEL", &replay_score_value, 0, &system_config.replay_level[3] },
	{ "REPLAY BOOST", &yes_no_value, NO, &system_config.replay_boost },
	{ "REPLAY AWARD", &free_award_value, FREE_AWARD_CREDIT, &system_config.replay_award },
	{ "SPECIAL AWARD", &free_award_value, FREE_AWARD_CREDIT, &system_config.special_award },
	{ "MATCH AWARD", &free_award_value, FREE_AWARD_CREDIT, &system_config.match_award },
	{ "EX. BALL TICKET", &yes_no_value, NO, &system_config.extra_ball_ticket },
	{ "MAX. TICKET/PLAYER" ,&max_tickets_value, 25, &system_config.max_tickets_per_player },
	{ "MATCH FEATURE", &percent_value, 7, &system_config.match_feature },
	{ "CUSTOM MESSAGE", &on_off_value, OFF, &system_config.custom_message },
	{ "LANGUAGE", &lang_value, 0, &system_config.language },
	{ "CLOCK STYLE", &clock_style_value, 0, &system_config.clock_style },
	{ "DATE STYLE", &date_style_value, 0, &system_config.date_style },
	{ "SHOW DATE/TIME", &yes_no_value, YES, &system_config.show_date_and_time },
	{ "ALLOW DIM ALLUM.", &yes_no_value, YES, &system_config.allow_dim_illum },
	{ "TOURNAMENT MODE", &yes_no_value, NO, &system_config.tournament_mode },
	{ "EURO. DIG. SEP.", &yes_no_value, NO, &system_config.euro_digit_sep },
	{ "MIN. VOL. OVERRIDE", &integer_value, 0, &system_config.min_volume_control },
	{ "G.I. POWER SAVER", &gi_power_saver_value, 15, &system_config.gi_power_saver },
	{ "POWER SAVER LEVEL", &power_saver_level_value, 7, &system_config.power_saver_level },
	{ "TICKET EXP. BOARD", &yes_no_value, NO, &system_config.ticket_board },
	{ "NO BONUS FLIPS", &yes_no_value, YES, &system_config.no_bonus_flips },
	{ "GAME RESTART", &game_restart_value, GAME_RESTART_SLOW, &system_config.game_restart },
	{ "ALLOW CHASE BALL", &yes_no_value, YES, &system_config.allow_chase_ball },
	{ NULL, NULL, 0, NULL },
};


/* TODO - these really belong in feature_config.  They are not being
verified/initialized correctly because of this */
struct adjustment feature_adjustments[] = {
	/* The first few feature adjustments are provided by the core
	system but only enabled if the game supports it. */

	{ "BUY EXTRA BALL", &yes_no_value, NO, &system_config.buy_extra_ball },

#ifdef MACHINE_LAUNCH_SWITCH
	{ "TIMED PLUNGER", &on_off_value, OFF, &system_config.timed_plunger },
	{ "FLIPPER PLUNGER", &on_off_value, OFF, &system_config.flipper_plunger },
#endif

	{ "FAMILY MODE", &yes_no_value, NO, &system_config.family_mode },

#ifdef MACHINE_HAS_NOVICE_MODE
	{ "NOVICE MODE", &yes_no_value, NO, &system_config.novice_mode },
#endif

	{ "GAME MUSIC", &on_off_value, ON, &system_config.game_music },

#ifdef CONFIG_TIMED_GAME
	{ "TIMED GAME", &yes_no_value, YES, &system_config.timed_game },
#endif

	/* The game-specific feature adjustments go here. */
#ifdef MACHINE_FEATURE_ADJUSTMENTS
	MACHINE_FEATURE_ADJUSTMENTS
#endif
	{ NULL, NULL, 0, NULL },

};


struct adjustment pricing_adjustments[] = {
	{ "GAME PRICING", &pricing_mode_value, PRICE_CUSTOM, &price_config.pricing_mode },
	/* The next 8 adjustments should be set automatically depending on the value
	of GAME PRICING */
	{ STR_LEFT "COIN UNITS", &nonzero_integer_value, 1, &price_config.coin_units[0] },
	{ STR_CENTER "COIN UNITS", &nonzero_integer_value, 1, &price_config.coin_units[1] },
	{ STR_RIGHT "COIN UNITS", &nonzero_integer_value, 1, &price_config.coin_units[2] },
	{ "4TH COIN UNITS", &nonzero_integer_value, 1, &price_config.coin_units[3] },
	{ "UNITS/CREDIT", &nonzero_integer_value, 2, &price_config.units_per_credit },
	{ "UNITS/BONUS", &integer_value, 0, &price_config.units_per_bonus },
	{ "BONUS CREDITS", &integer_value, 0, &price_config.bonus_credits },
	{ "MINIMUM UNITS", &integer_value, 1, &price_config.min_units },

	{ "COIN DOOR TYPE", &coin_door_type_value, COIN_DOOR_CUSTOM,
		&price_config.coin_door_type },
	/* The next 5 adjustments, plus BASE COIN SIZE and ALLOW HUNDREDTHS, should
	be set automatically depending on the value of COIN DOOR TYPE */
	{ "COLLECTION TEXT", &collection_text_value, CUR_DOLLAR,
		&price_config.collection_text },
	{ STR_LEFT "SLOT VALUE", &nonzero_integer_value, 25, &price_config.slot_values[0] },
	{ STR_CENTER "SLOT VALUE", &nonzero_integer_value, 100, &price_config.slot_values[1] },
	{ STR_RIGHT "SLOT VALUE", &nonzero_integer_value, 25, &price_config.slot_values[2] },
	{ "4TH SLOT VALUE", &nonzero_integer_value, 25, &price_config.slot_values[3] },

	{ "MAXIMUM CREDITS", &nonzero_integer_value, 10, &price_config.max_credits },

	/* When FREE_ONLY is defined, the option to set free play is removed from
	the menu entirely (by virtue of the null string).  The adjustment is still
	used for presets/factory reset and in this case, the default value of YES
	given here applies. */
#ifdef FREE_ONLY
	{ "", &yes_no_value, YES, &price_config.free_play },
#else
	{ "FREE PLAY", &yes_no_value, NO, &price_config.free_play },
#endif

	{ "HIDE COIN AUDITS", &yes_no_value, NO, &price_config.hide_coin_audits },
	{ "1-COIN BUY-IN", &yes_no_value, NO, &price_config.one_coin_buyin },
	{ "COIN METER UNITS", &integer_value, 0, &price_config.coin_meter_units },
	{ "FAST BILL SLOT", &yes_no_value, NO, &price_config.fast_bill_slot },
	{ "MIN. COIN MSEC.", &nonzero_integer_value, 50, &price_config.min_coin_msec },
	{ "SLAMTILT PENALTY", &yes_no_value, YES, &price_config.slamtilt_penalty },
	{ "ALLOW HUNDREDTHS", &yes_no_value, NO, &price_config.allow_hundredths },
	{ STR_CREDIT "FRACTION", &on_off_value, ON, &price_config.credit_fraction },
	{ NULL, NULL, 0, NULL },
};


struct adjustment hstd_adjustments[] = {
	{ "HIGHEST SCORES", &on_off_value, ON, &hstd_config.highest_scores },
	{ "H.S.T.D. AWARD", &free_award_value, FREE_AWARD_CREDIT, &hstd_config.hstd_award },
	{ "CHAMP. H.S.T.D.", &on_off_value, ON, &hstd_config.champion_hstd },
	{ "CHAMP. CREDITS", &credit_count_value, 2, &hstd_config.champion_credits },
	{ "H.S.T.D. 1 CREDITS", &credit_count_value, 1, &hstd_config.hstd_credits[0] },
	{ "H.S.T.D. 2 CREDITS", &credit_count_value, 1, &hstd_config.hstd_credits[1] },
	{ "H.S.T.D. 3 CREDITS", &credit_count_value, 1, &hstd_config.hstd_credits[2] },
	{ "H.S.T.D. 4 CREDITS", &credit_count_value, 1, &hstd_config.hstd_credits[3] },
	/* The next adjustment is stored in units of 250 games, so this is really
	3000 games per reset ... */
	{ "H.S. RESET EVERY", &hs_reset_value, 12, &hstd_config.hs_reset_every },
	{ "BACKUP CHAMP.", &score_value, 0, NULL },
	{ "BACKUP H.S.T.D. 1", &score_value, 0, NULL },
	{ "BACKUP H.S.T.D. 2", &score_value, 0, NULL },
	{ "BACKUP H.S.T.D. 3", &score_value, 0, NULL },
	{ "BACKUP H.S.T.D. 4", &score_value, 0, NULL },
	{ NULL, NULL, 0, NULL },
};


#ifdef CONFIG_NATIVE
#define DEFAULT_COL_WIDTH 50
#define DEFAULT_LINES_PER_PAGE 40
#else
#define DEFAULT_COL_WIDTH 72
#define DEFAULT_LINES_PER_PAGE 60
#endif
struct adjustment printer_adjustments[] = {
	{ "COLUMN WIDTH", &integer_value, DEFAULT_COL_WIDTH, &printer_config.column_width },
	{ "LINES PER PAGE", &lines_per_page_value, DEFAULT_LINES_PER_PAGE,
		&printer_config.lines_per_page },
	{ "PAUSE EVERY PAGE", &yes_no_value, NO, &printer_config.pause_every_page },
	{ "PRINTER TYPE", &printer_type_value, 0, &printer_config.printer_type },
	{ "SERIAL BAUD RATE", &baud_rate_value, 0, &printer_config.serial_baud_rate },
	{ "SERIAL D.T.R.", &integer_value, 0, &printer_config.serial_dtr },
	{ "NSM STUB ONLY", &on_off_value, OFF, &printer_config.nsm_stub_only },
	{ "AUTO PRINTOUT", &on_off_value, OFF, &printer_config.auto_printout },
	{ "AUTO LINE FILL", &on_off_value, OFF, &printer_config.auto_line_fill },
	{ NULL, NULL, 0, NULL },
};


#ifdef CONFIG_DEBUG_ADJUSTMENTS
extern U8 firq_row_value;
extern U8 switch_stress_enable;
struct adjustment debug_adjustments[] = {
#if (MACHINE_DMD == 1)
	{ "FIRQ ROW VALUE", &integer_value, DMD_REFRESH_ROW, &firq_row_value },
#endif
	{ "SWITCH STRESS", &yes_no_value, NO, &switch_stress_enable },
	{ NULL, NULL, 0, NULL },
};
#endif

struct adjustment empty_adjustments[] = {
	{ "EMPTY ADJ. SET", &integer_value, 0, NULL },
	{ NULL, NULL, 0, NULL },
};

void adj_reset (struct adjustment *adjs)
{
	pinio_nvram_unlock ();
	while (adjs->name != NULL)
	{
		if (adjs->nvram)
		{
			*(adjs->nvram) = adjs->factory_default;
		}
		adjs++;
	}
	pinio_nvram_lock ();
}


void adj_verify (struct adjustment *adjs)
{
	U8 val;

	while (adjs->name != NULL)
	{
		if (adjs->nvram)
		{
			val = *(adjs->nvram);
			if ((val < adjs->values->min) || (val > adjs->values->max))
			{
				pinio_nvram_unlock ();
				*(adjs->nvram) = adjs->factory_default;
				pinio_nvram_lock ();
			}
		}
		adjs++;
	}
}

struct adjustment *adj_lookup;


void adj_prepare_lookup (struct adjustment *table)
{
	adj_lookup = table;
}


/** Render the name of the adjustment that is located at the given
protected memory address, and its current value, to the display.
If there is no match, nothing is printed. */
void adj_name_for_preset (U8 * const nvram, const U8 value)
{
	if (adj_lookup == NULL)
		return;

	/* Searching through all adjustments, and then printing was known
	to be slow at one point, so these sleeps were added.  It may not
	be necessary now that printing is done in assembly. */
	task_sleep (TIME_16MS);
	while (adj_lookup->nvram != NULL)
	{
		if (adj_lookup->nvram == nvram)
		{
			task_sleep (TIME_16MS);
			font_render_string_center (&font_mono5, 64, 16, adj_lookup->name);
			adj_lookup->values->render (value);
			print_row_center (&font_mono5, 24);
			return;
		}
		adj_lookup++;
	}
}


void adj_reset_all (void)
{
	adj_reset (standard_adjustments);
	adj_reset (feature_adjustments);
	adj_reset (pricing_adjustments);
	adj_reset (hstd_adjustments);
	adj_reset (printer_adjustments);
}


void adj_verify_all (void)
{
	adj_verify (standard_adjustments);
	adj_verify (feature_adjustments);
	adj_verify (pricing_adjustments);
	adj_verify (hstd_adjustments);
	adj_verify (printer_adjustments);
}


void adj_set_current (struct adjustment *first)
{
	current_adjustment_set = first;
}


U8 adj_count_current (void)
{
	U8 count = 0;
	struct adjustment *ad = current_adjustment_set;

	/* Count the number of adjustments manually by stepping through
	 * the array of entries */
	while (ad->name != NULL)
	{
		count++;
		ad++;
	}
	return count;
}


const struct adjustment *adj_get (U8 num)
{
	extern struct adjustment *current_adjustment_set;
	struct adjustment *src = current_adjustment_set + num;

	current_adjustment.name = src->name;
	current_adjustment.values = &current_adjustment_value;
	current_adjustment_value.min = src->values->min;
	current_adjustment_value.max = src->values->max;
	current_adjustment_value.step = src->values->step;
	current_adjustment_value.render = src->values->render;
	current_adjustment.factory_default = src->factory_default;
	current_adjustment.nvram = src->nvram;

	return &current_adjustment;
}


#define std_adj_p(name) \
	(current_adjustment.nvram == &system_config.name)

#define std_adj_array_p(name, size) \
	(current_adjustment.nvram >= &system_config.name[0] \
		&& current_adjustment.nvram < &system_config.name[size])

bool adj_current_hidden_p (void)
{
	if (current_adjustment.name[0] == '\0')
		return TRUE;

	/* When the replay system is set to AUTO, do not allow
	configuration of the fixed replay levels */
	if (system_config.replay_system == REPLAY_AUTO &&
		std_adj_array_p (replay_level, MAX_REPLAY_LEVELS))
		return TRUE;

	/* When the replay system is set to FIXED, do not allow
	configuration of the start and percentaging options. */
	if (system_config.replay_system == REPLAY_FIXED &&
		(std_adj_p (replay_percent) || std_adj_p (replay_start)))
		return TRUE;

	return FALSE;
}


void adj_render_current_name (U8 id)
{
	sprintf ("%d.%c%s", id, ' ', current_adjustment.name);
	/* TODO : the following, more natural form fails
	 * due to a GCC bug: */
	//sprintf ("%d. %s", id, current_adjustment.name);
}

void adj_render_current_value (U8 val)
{
	current_adjustment_value.render (val);
}


