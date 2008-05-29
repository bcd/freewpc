/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Test mode infrastructure and test implementations
 *
 * The test mode system is comprised of a number of inter-related
 * modules.
 *
 * The heart of the module is the "window".  A window can be thought of
 * as a form that occupies the entire display.  Each window is an instance
 * of a "window class", which is defined by the window_ops structure.
 * The window class declares callbacks for various events, including
 * inputs (key presses) and focus change (new window created, etc.)
 * The module maintains a window stack, so that new windows can be 
 * created temporarily and then popped off to return to the originating
 * point.
 *
 * A "menu" is an instance of a window, and adds the functionality
 * of hierarchical navigation.  A menu object can either represent an
 * item -- a leaf in the hierarchy representing some desired function,
 * or an actual menu which contains submenus/subitems.  The flags
 * field of the menu object denotes the type (M_MENU or M_ITEM).
 * Every menu object has a name, which is displayed at the top of the
 * window for the current menu or as the currently selected choice.
 * The up/down buttons change the active selection.  Enter/Escape
 * are used to move up and down the hierarchy.
 *
 * At the leaf nodes, the menu object contains the name of a window
 * class and instance data to be passed to its constructor (init function).
 * For true menus, this data is replaced by a reference to the selected
 * item.
 *
 * Two additional generic window types are defined at present.  The
 * browser window class is used as an item class to browse (read-only)
 * a set of numbered values.  The class provides some extensions that
 * can modify the way that each object is rendered when selected.
 *
 * The scroller window class is used to display an array of text
 * messages.  A scroller is implemented as a set of function pointers
 * to functions that render the text; the common code takes care
 * of drawing it to the DMD and handles the keypresses to change pages.
 *
 * At present, the test implementations are also included in this same
 * file, although for modularity's sake they should be moved elsewhere.
 */

#include <freewpc.h>
#include <window.h>
#include <test.h>
#include <format.h>
#include <coin.h>
#include <highscore.h>
#include <preset.h>


#undef CONFIG_TEST_DURING_GAME

/** win_top always points to the current window, or NULL if
 * no window is open. */
struct window *win_top;

/* Equivalent to (win_top != NULL), but as a byte, this can
 * be tested with a single instruction.
 * IDEA: these two variables could be overlapped into a union. */
__fastram__ U8 in_test;


/* The window stack keeps track of where you came from, so when you
 * exit a menu/window/whatever, you can go back to where you started.
 * There is a maximum depth here, which should be sufficient. */
struct window win_stack[8];


/** Push the first window onto the stack.  This ends any game in progress
 * marks 'in test'.  It also resets sound, display, and lamps. */
void window_push_first (void)
{
	in_test = 1;
#ifdef CONFIG_TEST_DURING_GAME
	if (!switch_poll_logical (SW_LEFT_BUTTON))
#endif
	{
		end_game ();
		sound_reset ();
		/* Kill any other tasks still running */
		task_kill_all ();
	}
	callset_invoke (test_start);
}


/** Pop the first window pushed, returning out of test mode */
void window_pop_first (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();

	/* Delay before starting amode and actually
	 * exiting test mode; this keeps extra presses 
	 * of the escape button from adding service credits. */
	task_sleep_sec (1);
#ifdef CONFIG_TEST_DURING_GAME
	if (!switch_poll_logical (SW_LEFT_BUTTON))
#endif
	{
		amode_start ();
	}
	in_test = 0;
}

/** Starts the window's thread function, if it exists. */
void window_start_thread (void)
{
	if (win_top->ops->thread)
	{
		task_recreate_gid (GID_WINDOW_THREAD, win_top->ops->thread);
		task_yield ();
	}
}


/** Stops the window's thread function, if it exists. */
void window_stop_thread (void)
{
	task_kill_gid (GID_WINDOW_THREAD);
}


/** Redraws the current window. */
void window_redraw (void)
{
	dmd_alloc_low_high ();
	dmd_clean_page_low ();
	window_call_op (win_top, draw);
}


/** Push a new window onto the stack */
void window_push (struct window_ops *ops, void *priv)
{
	window_stop_thread ();
	if (win_top == NULL)
	{
		window_push_first ();
		win_top = &win_stack[0];
	}
	else if (win_top < &win_stack[8])
	{
		win_top++;
	}
	else
	{
		nonfatal (ERR_WINDOW_STACK_OVERFLOW);
		return;
	}

	win_top->ops = ops;
	win_top->w_class.priv = priv;
	sound_send (SND_TEST_ENTER);
	window_call_op (win_top, init);
	window_start_thread ();
	window_redraw ();
}


/** Pop the current window off the stack */
void window_pop_quiet (void)
{
	if (win_top == NULL)
		return;

	window_stop_thread ();
	window_call_op (win_top, exit);

	if (win_top == &win_stack[0])
	{
#ifndef MACHINE_TEST_ONLY
		window_pop_first ();
		win_top = NULL;
#endif
	}
	else
	{
		win_top--;
		window_start_thread ();
		window_redraw ();
	}
}

void window_pop (void)
{
	if (win_top == NULL)
		return;
	sound_send (SND_TEST_ESCAPE);
	window_pop_quiet ();
}


/** Initialize the window subsystem */
void window_init (void)
{
	win_top = NULL;
}

void window_title (const char *title)
{
	font_render_string_center (&font_mono5, 64, 2, title);
}

void print_row_center (font_t *f, U8 row)
{
	font_render_string_center (f, 64, row, sprintf_buffer);
}

/***************************************************/

extern struct menu main_menu;
extern struct window_ops menu_window;

/**********************************************************/

/* A window class for generic browsing.
 * You can customize this by copying the ops structure
 * and modifying the fields you need to. */

U8 browser_action;
U8 browser_last_selection_update;
void (*browser_item_number) (U8);
U8 browser_min;
U8 browser_max;

void browser_hex_item_number (U8 val)
{
	sprintf ("%02X", val);
}

void browser_decimal_item_number (U8 val)
{
	sprintf ("%d", val);
}

void browser_init (void)
{
	struct menu *m = win_top->w_class.priv;
	
	win_top->w_class.menu.self = m;
	menu_selection = 0;	

	browser_action = 0;
	browser_last_selection_update = 0;
	browser_item_number = browser_hex_item_number;
	browser_min = 0;
	browser_max = 0xFF;
}

void browser_draw (void)
{
	struct menu *m = win_top->w_class.menu.self;

	window_title (m->name);

	if (browser_item_number)
	{
		(*browser_item_number) (menu_selection);
		font_render_string (&font_mono5, 4, 20, sprintf_buffer);
	}

	dmd_show_low ();
}

void browser_up (void)
{
	sound_send (SND_TEST_UP);
	menu_selection++;
	if (menu_selection > browser_max)
		menu_selection = browser_min;
}

void browser_down (void)
{
	sound_send (SND_TEST_DOWN);
	menu_selection--;
	if (menu_selection == 0xFF)
		menu_selection = browser_max;
}

struct window_ops browser_window = {
	INHERIT_FROM_BROWSER,
};

void browser_print_operation (const char *s)
{
	font_render_string_right (&font_mono5, 127, 20, s);
}

/**********************************************************/

/* A window class for adjustment browsing.
 * It supports all of the standard browser operations,
 * but the enter key can be used to go into edit mode
 * and change the value of the item.
 */

struct adjustment *browser_adjs;
U8 adj_edit_value;

struct adjustment_value integer_value = { 0, 0xFF, 1, decimal_render };
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
struct adjustment_value clock_style_value = { 0, 1, 1, clock_style_render };
struct adjustment_value date_style_value = { 0, 1, 1, date_style_render };
struct adjustment_value score_value = { 0, 250, 10, decimal_render };
struct adjustment_value lang_value = { 0, 0, 0, lang_render };
struct adjustment_value replay_system_value = { 0, 1, 1, replay_system_render };
struct adjustment_value free_award_value = { 0, 4, 1, free_award_render };
struct adjustment_value percent_value = { 0, 100, 1, percent_render };

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
	{ "MAX EB PER BIP", &max_eb_value, 4, &system_config.max_ebs_per_bip },
	{ "REPLAY SYSTEM", &replay_system_value, 0, &system_config.replay_system },
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
	{ "EXTRA BALL TICKET", &yes_no_value, NO, &system_config.extra_ball_ticket },
	{ "MAX. TICKET/PLAYER" ,&max_tickets_value, 25, &system_config.max_tickets_per_player },
	{ "MATCH FEATURE", &percent_value, 7, &system_config.match_feature },
	{ "CUSTOM MESSAGE", &on_off_value, OFF, &system_config.custom_message },
	{ "LANGUAGE", &lang_value, 0, &system_config.language },
	{ "CLOCK STYLE", &clock_style_value, 0, &system_config.clock_style },
	{ "DATE STYLE", &date_style_value, 0, &system_config.date_style },
	{ "SHOW DATE/TIME", &yes_no_value, YES, &system_config.show_date_and_time },
	{ "ALLOW DIM ALLUM.", &yes_no_value, NO, &system_config.allow_dim_illum },
	{ "TOURNAMENT MODE", &yes_no_value, NO, &system_config.tournament_mode },
	{ "EURO. DIGIT SEP.", &yes_no_value, NO, &system_config.euro_digit_sep },
	{ "MIN. VOL. OVERRIDE", &integer_value, 8, &system_config.min_volume_control },
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
	{ "LEFT COIN UNITS", &nonzero_integer_value, 1, &price_config.coin_units[0] },
	{ "CENTER COIN UNITS", &nonzero_integer_value, 1, &price_config.coin_units[1] },
	{ "RIGHT COIN UNITS", &nonzero_integer_value, 1, &price_config.coin_units[2] },
	{ "4TH COIN UNITS", &nonzero_integer_value, 1, &price_config.coin_units[3] },
	{ "UNITS/CREDIT", &nonzero_integer_value, 2, &price_config.units_per_credit },
	{ "UNITS/BONUS", &integer_value, 0, &price_config.units_per_bonus },
	{ "BONUS CREDITS", &integer_value, 0, &price_config.bonus_credits },
	{ "MINIMUM UNITS", &integer_value, 1, &price_config.min_units },
	{ "COIN DOOR TYPE", &integer_value, 1, &price_config.coin_door_type },
	{ "COLLECTION TEXT", &integer_value, 1, &price_config.collection_text },
	{ "LEFT SLOT VALUE", &nonzero_integer_value, 1, &price_config.slot_values[0] },
	{ "CENTER SLOT VALUE", &nonzero_integer_value, 4, &price_config.slot_values[1] },
	{ "RIGHT SLOT VALUE", &nonzero_integer_value, 1, &price_config.slot_values[2] },
	{ "4TH SLOT VALUE", &nonzero_integer_value, 1, &price_config.slot_values[3] },
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

	{ "HIDE COIN AUDITS", &yes_no_value, NO, NULL },
	{ "1-COIN BUY-IN", &yes_no_value, NO, &price_config.one_coin_buyin },
	{ "COIN METER UNITS", &integer_value, 0, &price_config.coin_meter_units },
	{ "DOLLAR BILL SLOT", &yes_no_value, NO, NULL },
	{ "MIN. COIN MSEC.", &nonzero_integer_value, 50, &price_config.min_coin_msec },
	{ "SLAMTILT PENALTY", &yes_no_value, YES, &price_config.slamtilt_penalty },
	{ "ALLOW HUNDREDTHS", &yes_no_value, NO, &price_config.allow_hundredths },
	{ "CREDIT FRACTION", &on_off_value, ON, &price_config.credit_fraction },
	{ NULL, NULL, 0, NULL },
};


struct adjustment hstd_adjustments[] = {
	{ "HIGHEST SCORES", &on_off_value, ON, &hstd_config.highest_scores },
	{ "H.S.T.D. AWARD", &on_off_value, ON, &hstd_config.hstd_award },
	{ "CHAMPION H.S.T.D.", &on_off_value, ON, &hstd_config.champion_hstd },
	{ "CHAMPION CREDITS", &credit_count_value, 2, &hstd_config.champion_credits },
	{ "H.S.T.D. 1 CREDITS", &credit_count_value, 1, &hstd_config.hstd_credits[0] },
	{ "H.S.T.D. 2 CREDITS", &credit_count_value, 1, &hstd_config.hstd_credits[1] },
	{ "H.S.T.D. 3 CREDITS", &credit_count_value, 1, &hstd_config.hstd_credits[2] },
	{ "H.S.T.D. 4 CREDITS", &credit_count_value, 1, &hstd_config.hstd_credits[3] },
	{ "H.S. RESET EVERY", &hs_reset_value, 12, &hstd_config.hs_reset_every },
	{ "BACKUP CHAMPION", &score_value, 0, NULL },
	{ "BACKUP H.S.T.D. 1", &score_value, 0, NULL },
	{ "BACKUP H.S.T.D. 2", &score_value, 0, NULL },
	{ "BACKUP H.S.T.D. 3", &score_value, 0, NULL },
	{ "BACKUP H.S.T.D. 4", &score_value, 0, NULL },
	{ NULL, NULL, 0, NULL },
};


struct adjustment printer_adjustments[] = {
	{ "COLUMN WIDTH", &integer_value, 72, &printer_config.column_width },
	{ "LINES PER PAGE", &integer_value, 60, &printer_config.lines_per_page },
	{ "PAUSE EVERY PAGE", &yes_no_value, NO, &printer_config.pause_every_page },
	{ "PRINTER TYPE", &integer_value, 0, &printer_config.printer_type },
	{ "SERIAL BAUD RATE", &integer_value, 0, &printer_config.serial_baud_rate },
	{ "SERIAL D.T.R.", &integer_value, 0, &printer_config.serial_dtr },
	{ "NSM STUB ONLY", &on_off_value, OFF, &printer_config.nsm_stub_only },
	{ "AUTO PRINTOUT", &on_off_value, OFF, &printer_config.auto_printout },
	{ "AUTO LINE FILL", &on_off_value, OFF, &printer_config.auto_line_fill },
	{ NULL, NULL, 0, NULL },
};


struct adjustment empty_adjustments[] = {
	{ "EMPTY ADJ. SET", &integer_value, 0, NULL },
	{ NULL, NULL, 0, NULL },
};


void adj_reset (struct adjustment *adjs)
{
	wpc_nvram_get ();
	while (adjs->name != NULL)
	{
		if (adjs->nvram)
		{
			*(adjs->nvram) = adjs->factory_default;
		}
		adjs++;
	}
	wpc_nvram_put ();
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
				wpc_nvram_get ();
				*(adjs->nvram) = adjs->factory_default;
				wpc_nvram_put ();
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
			far_call_pointer (*adj_lookup->values->render, TEST2_PAGE, value);
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


void adj_browser_draw (void)
{
	struct adjustment *ad = browser_adjs + menu_selection;

	window_stop_thread ();

	sprintf ("%d. %s", menu_selection+1, ad->name);
	print_row_center (&font_mono5, 10);

	if (ad->nvram == NULL)
	{
		font_render_string_center (&font_mono5, 32, 21, "N/A");
		dmd_copy_low_to_high ();
	}
	else
	{
		if (ad->nvram && (browser_action != ADJ_EDITING))
			adj_edit_value = *(ad->nvram);

		if (adj_edit_value == ad->factory_default)
			font_render_string_center (&font_var5, 96, 21, "(FAC. DEFAULT)");
		dmd_copy_low_to_high ();

		if (ad->nvram)
			far_call_pointer (ad->values->render, TEST2_PAGE, adj_edit_value);

		font_render_string_center (&font_mono5, 32, 21, sprintf_buffer);
	}
	window_start_thread ();
}


void adj_browser_thread (void)
{
	for (;;)
	{
		if (browser_action == ADJ_EDITING)
			dmd_show_other ();
		else
			dmd_show_low ();
		task_sleep (TIME_100MS);
	}
}


void adj_browser_init (void)
{
	struct adjustment *ad;

	browser_init ();
	browser_action = ADJ_BROWSING;
	browser_min = 0;

	ad = browser_adjs = win_top->w_class.priv;

	/* Count the number of adjustments manually by stepping through
	 * the array of entries */
	browser_max = 0xFF;
	while (ad->name != NULL)
	{
		browser_max++;
		ad++;
	}
	if (browser_max == 0xFF)
	{
		/* No adjustments were defined in this menu. */
		browser_adjs = empty_adjustments;
		browser_max = 0;
	}
}


void adj_browser_enter (void)
{
	if ((browser_action == ADJ_BROWSING) &&
		 (browser_adjs[menu_selection].nvram != NULL))
	{
		adj_edit_value = *(browser_adjs[menu_selection].nvram);
		browser_action = ADJ_EDITING;
		sound_send (SND_TEST_ENTER);
	}
	else if (browser_action == ADJ_EDITING)
	{
		/* TODO - confirmation of changes not done yet */
		if (*(browser_adjs[menu_selection].nvram) != adj_edit_value)
		{
			/* Modify the adjustment */
			wpc_nvram_get ();
			*(browser_adjs[menu_selection].nvram) = adj_edit_value;
			wpc_nvram_put ();
			adj_modified ();
			sound_send (SND_TEST_CONFIRM);
		}
		browser_action = ADJ_BROWSING;
	}
}


void adj_browser_escape (void)
{
	if (browser_action == ADJ_EDITING)
	{
		/* abort */
		if (adj_edit_value != *(browser_adjs[menu_selection].nvram))
			sound_send (SND_TEST_ABORT);
		browser_action = ADJ_BROWSING;
	}
	else
	{
		window_pop ();
	}
}


void adj_browser_up (void)
{
	if (browser_action == ADJ_BROWSING)
	{
		do {
			browser_up ();
		} while (!*browser_adjs[menu_selection].name);
	}
	else if (browser_action == ADJ_EDITING)
	{
		sound_send (SND_TEST_UP);
		if (adj_edit_value < browser_adjs[menu_selection].values->max)
			adj_edit_value += browser_adjs[menu_selection].values->step;
		else
			adj_edit_value = browser_adjs[menu_selection].values->min;
	}
}


void adj_browser_down (void)
{
	if (browser_action == ADJ_BROWSING)
	{
		do {
			browser_down ();
		} while (!*browser_adjs[menu_selection].name);
	}
	else if (browser_action == ADJ_EDITING)
	{
		sound_send (SND_TEST_DOWN);
		if (adj_edit_value > browser_adjs[menu_selection].values->min)
			adj_edit_value -= browser_adjs[menu_selection].values->step;
		else
			adj_edit_value = browser_adjs[menu_selection].values->max;
	}
}


#define INHERIT_FROM_ADJ_BROWSER \
	INHERIT_FROM_BROWSER, \
	.init = adj_browser_init, \
	.draw = adj_browser_draw, \
	.enter = adj_browser_enter, \
	.escape = adj_browser_escape, \
	.up = adj_browser_up, \
	.down = adj_browser_down, \
	.thread = adj_browser_thread

struct window_ops adj_browser_window = {
	INHERIT_FROM_ADJ_BROWSER,
};

/*****************************************************/

struct audit *browser_audits;

audit_t default_audit_value;


struct audit main_audits[] = {
	{ "TOTAL EARNINGS", AUDIT_TYPE_TOTAL_EARNINGS, &default_audit_value },
	{ "RECENT EARNINGS", AUDIT_TYPE_NONE, NULL },
	{ "FREEPLAY PERCENT", AUDIT_TYPE_NONE, NULL },
	{ "AVG. BALL TIME", AUDIT_TYPE_NONE, NULL },
	{ "TIME PER CREDIT", AUDIT_TYPE_SECS, &default_audit_value },
	{ "TOTAL PLAYS", AUDIT_TYPE_INT, &system_audits.total_plays },
	{ "REPLAY AWARDS", AUDIT_TYPE_INT, &system_audits.replays },
	{ "PERCENT REPLAYS", AUDIT_TYPE_GAME_PERCENT, &system_audits.replays },
	{ "EXTRA BALLS", AUDIT_TYPE_INT, &system_audits.extra_balls_awarded },
	{ "PERCENT EX. BALL", AUDIT_TYPE_GAME_PERCENT, &system_audits.extra_balls_awarded },
	{ NULL, AUDIT_TYPE_NONE, NULL },
};

struct audit earnings_audits[] = {
	{ "RECENT EARNINGS", AUDIT_TYPE_TOTAL_EARNINGS, &default_audit_value },
	{ "RECENT LEFT SLOT", AUDIT_TYPE_INT, &system_audits.coins_added[0] },
	{ "RECENT CENTER SLOT", AUDIT_TYPE_INT, &system_audits.coins_added[1] },
	{ "RECENT RIGHT SLOT", AUDIT_TYPE_INT, &system_audits.coins_added[2] },
	{ "RECENT 4TH SLOT", AUDIT_TYPE_INT, &system_audits.coins_added[3] },
	{ "RECENT PAID CREDITS", AUDIT_TYPE_INT, &system_audits.paid_credits },
	{ "RECENT SERV. CRED.", AUDIT_TYPE_INT, &system_audits.service_credits },
	{ NULL, AUDIT_TYPE_NONE, NULL },
};


struct audit standard_audits[] = {
	{ "GAMES STARTED", AUDIT_TYPE_INT, &system_audits.games_started },
	{ "TOTAL PLAYS", AUDIT_TYPE_INT, &system_audits.total_plays },
	{ "TOTAL FREE PLAYS", AUDIT_TYPE_INT, &system_audits.total_free_plays },
	{ "FREEPLAY PERCENT", AUDIT_TYPE_GAME_PERCENT, &system_audits.total_free_plays },
	{ "REPLAY AWARDS", AUDIT_TYPE_INT, &system_audits.replays },
	{ "PERCENT REPLAYS", AUDIT_TYPE_GAME_PERCENT, &system_audits.replays },
	{ "SPECIAL AWARDS", AUDIT_TYPE_INT, &system_audits.specials },
	{ "PERCENT SPECIAL", AUDIT_TYPE_GAME_PERCENT, &system_audits.specials },
	{ "MATCH AWARDS", AUDIT_TYPE_INT, &system_audits.match_credits },
	{ "PERCENT MATCH", AUDIT_TYPE_GAME_PERCENT, &system_audits.match_credits },
	{ "EXTRA BALLS", AUDIT_TYPE_INT, &system_audits.extra_balls_awarded },
	{ "PERCENT EX. BALL", AUDIT_TYPE_GAME_PERCENT, &system_audits.extra_balls_awarded },
	{ "TILTS", AUDIT_TYPE_INT, &system_audits.tilts },
	{ "LEFT DRAINS", AUDIT_TYPE_INT, &system_audits.left_drains },
	{ "RIGHT DRAINS", AUDIT_TYPE_INT, &system_audits.right_drains },
	{ "CENTER DRAINS", AUDIT_TYPE_INT, &system_audits.center_drains },
	{ "POWER UPS", AUDIT_TYPE_INT, &system_audits.power_ups },
	{ "SLAM TILTS", AUDIT_TYPE_INT, &system_audits.slam_tilts },
	{ "PLUMB BOB TILTS", AUDIT_TYPE_INT, &system_audits.plumb_bob_tilts },
	{ "FATAL ERRORS", AUDIT_TYPE_INT, &system_audits.fatal_errors },
	{ "NON-FATAL ERRORS", AUDIT_TYPE_INT, &system_audits.non_fatal_errors },
	{ "LEFT FLIPPER", AUDIT_TYPE_INT, &system_audits.left_flippers },
	{ "RIGHT FLIPPER", AUDIT_TYPE_INT, &system_audits.right_flippers },
	{ "TROUGH RESCUE", AUDIT_TYPE_INT, &system_audits.trough_rescues },
	{ "CHASE BALLS", AUDIT_TYPE_INT, &system_audits.chase_balls },
	{ "LOCKUP 1 ADDR", AUDIT_TYPE_INT, &system_audits.lockup1_addr },
	{ "LOCKUP 1 PID/LEF", AUDIT_TYPE_INT, &system_audits.lockup1_pid_lef },
	{ NULL, AUDIT_TYPE_NONE, NULL },
};


void audit_browser_init (void)
{
	struct audit *aud;

	browser_init ();

	aud = browser_audits = win_top->w_class.priv;
	
	/* Count the number of adjustments manually by stepping through
	 * the array of entries */
	browser_min = 0;
	browser_max = -1;
	while (aud->name != NULL)
	{
		browser_max++;
		aud++;
	}
}

void audit_browser_draw (void)
{
	struct audit *aud = browser_audits + menu_selection;

	sprintf ("%d. %s", menu_selection+1, aud->name);
	print_row_center (&font_mono5, 10);

	if (aud->nvram)
	{
		render_audit (*(aud->nvram), aud->format);
		font_render_string_center (&font_mono5, 32, 21, sprintf_buffer);
	}

	dmd_show_low ();
}


#define INHERIT_FROM_AUDIT_BROWSER \
	INHERIT_FROM_BROWSER, \
	.init = audit_browser_init, \
	.draw = audit_browser_draw

struct window_ops audit_browser_window = {
	INHERIT_FROM_AUDIT_BROWSER,
};


/*****************************************************/

/* A window class for a confirmation screen.
 * A customizable message is displayed, with the
 * choice to confirm (start) or abort (escape).
 * The action taken on confirmation is also
 * customizable.
 */

U8 confirm_timer;

void confirm_init (void)
{
	confirm_timer = 7;
}

void confirm_draw (void)
{
	struct menu *m = (win_top-1)->w_class.priv;
	U8 sel = (win_top-1)->w_class.menu.selected;
	m = m->var.submenus[sel];

	sprintf ("%d", confirm_timer);
	font_render_string_left (&font_mono5, 1, 1, sprintf_buffer);
	font_render_string_right (&font_mono5, 127, 1, sprintf_buffer);

	window_title (m->name);
	font_render_string_center (&font_var5, 64, 18, "PRESS ENTER TO CONFIRM");
	font_render_string_center (&font_var5, 64, 24, "PRESS ESCAPE TO CANCEL");

	dmd_show_low ();
}

void confirm_enter (void)
{
	window_stop_thread ();
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 10, "SAVING NEW");
	font_render_string_center (&font_mono5, 64, 20, "ADJUSTMENT VALUE");
	dmd_show_low ();
	sound_send (SND_TEST_CONFIRM);
	task_sleep_sec (2);
	window_pop_quiet ();
}

void confirm_escape (void)
{
	window_stop_thread ();
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 10, "ESCAPE PRESSED");
	font_render_string_center (&font_mono5, 64, 20, "CHANGE IGNORED");
	dmd_show_low ();
	sound_send (SND_TEST_ABORT);
	task_sleep_sec (2);
	window_pop_quiet ();
}

void confirm_thread (void)
{
	while (confirm_timer > 0)
	{
		sound_send (SND_TEST_HSRESET);
		task_sleep_sec (1);
		confirm_timer--;
		dmd_alloc_low_clean ();
		confirm_draw ();
	}
	task_sleep (TIME_100MS * 5);
	window_pop_quiet ();
	task_exit ();
}

#define INHERIT_FROM_CONFIRM_WINDOW \
	DEFAULT_WINDOW, \
	.init = confirm_init, \
	.draw = confirm_draw, \
	.enter = confirm_enter, \
	.escape = confirm_escape, \
	.thread = confirm_thread

struct window_ops confirm_window = {
	INHERIT_FROM_CONFIRM_WINDOW,
};


/*****************************************************/

/* A window class for actual menus */

/** Return the number of items in a menu, by iterating
through the array of them until a NULL pointer is seen. */
static U8 count_submenus (struct menu *m)
{
	U8 count = 0;
	struct menu **submenus = m->var.submenus;
	if (submenus == NULL)
		return 0;
	while (*submenus != 0)
	{
		submenus++;
		count++;
	}
	return (count);
}


void menu_init (void)
{
	struct menu *m = win_top->w_class.priv;
	
	win_top->w_class.menu.self = m;
	win_top->w_class.menu.parent = NULL;
	menu_selection = 0;
}


/** The draw function for any menu. */
void menu_draw (void)
{
	struct menu *m = win_top->w_class.menu.self;
	struct menu **subm;
	U8 *sel = &win_top->w_class.menu.selected;

	/* First print the name of the menu itself */
	font_render_string (&font_mono5, 8, 4, m->name);

	/* Now try to print the current item.  *sel is the
	index to the item that is currently selected. */
	subm = m->var.submenus;
	if (subm != NULL)
	{
		/* For compatibility with real WPC games, the top-level
		menus are prefixed by a letter (the first letter in the
		naem of the item) instead of a number. */
		if (subm[*sel]->flags & M_LETTER_PREFIX)
		{
			sprintf ("%c. %s", subm[*sel]->name[0], subm[*sel]->name);
		}
		else
		{
			sprintf ("%d. %s", (*sel)+1, subm[*sel]->name);
		}
		font_render_string (&font_mono5, 8, 14, sprintf_buffer);
	}
	else
	{
		/* error : no submenus defined */
	}
	dmd_show_low ();
}


void menu_enter (void)
{
	struct menu *m = win_top->w_class.menu.self;
	U8 sel = win_top->w_class.menu.selected;

	m = m->var.submenus[sel];
	if (m->flags & M_MENU)
	{
		/* Enter on a menu item causes that menu to become active. */
		window_push (&menu_window, m);
	}
	else
	{
		struct window_ops *ops = m->var.subwindow.ops;
		/* Enter on a non-menu item pushes the window as declared by
		the item. */
		if (ops != NULL)
		{
			window_push (ops, m->var.subwindow.priv ? m->var.subwindow.priv : m);
		}
		else
		{
			/* Sound an error if a non-menu item is selected, but there is
			no window operations structure for it. */
			sound_send (SND_TEST_ABORT);
		}
	}
}

void menu_up (void)
{
	struct menu *m = win_top->w_class.menu.self;
	U8 *sel = &win_top->w_class.menu.selected;
	
	sound_send (SND_TEST_UP);
	(*sel)++;
	if ((*sel) >= count_submenus (m))
		*sel = 0;
}

void menu_down (void)
{
	struct menu *m = win_top->w_class.menu.self;
	U8 *sel = &win_top->w_class.menu.selected;
	
	sound_send (SND_TEST_DOWN);
	(*sel)--;
	if ((*sel) == 0xFF) 
	{
		*sel = count_submenus (m);
		if (*sel > 0)
			(*sel)--;
	}
}


void menu_left_flipper_task (void)
{
	task_sleep (TIME_66MS);
	if (!switch_poll_logical (SW_RIGHT_BUTTON))
		menu_down ();
	else
		window_pop ();
	task_exit ();
}

void menu_right_flipper_task (void)
{
	task_sleep (TIME_66MS);
	if (!switch_poll_logical (SW_LEFT_BUTTON))
		menu_up ();
	else
		window_pop ();
	task_exit ();
}


void menu_left_flipper (void)
{
	task_create_gid1 (GID_MENU_FLIPPER, menu_left_flipper_task);
}

void menu_right_flipper (void)
{
	task_create_gid1 (GID_MENU_FLIPPER, menu_right_flipper_task);
}

struct window_ops menu_window = {
	DEFAULT_WINDOW,
	.init = menu_init,
	.draw = menu_draw,
	.enter = menu_enter,
	.up = menu_up,
	.down = menu_down,
	.left = menu_left_flipper,
	.right = menu_right_flipper,
	.start = menu_enter,
};

/**********************************************************/

/* The test mode menus */

/*******************  Font Test  ************************/

U8 font_test_offset;
U8 font_test_char_width;

const font_t *font_test_lookup (void)
{
	extern const font_t *font_table[];
	return font_table[menu_selection];
}

char font_test_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";


void font_test_init (void)
{
	browser_init ();
	browser_max = MAX_FONTS-1; /* set to highest valid font number */
	font_test_offset = 0;
	font_test_char_width = 8;
}

void font_test_draw (void)
{
	const font_t *font = font_test_lookup ();
	char *gl, **glp;
	char bitwidth;

	/* TODO : this won't work because the font data is in a different page!
	 * Need to add a function in FONT_PAGE : char *font_get_glyph(font, char)
	 * that returns a pointer to the glyph for a character.  Then you can
	 * use the following code, although macros would be better:
	 * glyph_get_width(), glyph_get_height(), etc. */
	glp = (char **)far_read_pointer ((PTR_OR_U16 *)&font->glyphs, FONT_PAGE);

	gl = (char *)far_read_pointer ((PTR_OR_U16 *)&glp['A'], FONT_PAGE);
	if (gl == NULL) 
	{
		if (font_test_offset < 26)
			font_test_offset = 26;
	}
	else
		gl = (char *)far_read_pointer ((PTR_OR_U16 *)&glp['0'], FONT_PAGE);

	bitwidth = (char)far_read8 ((U8 *)&gl[0], FONT_PAGE);
	if (bitwidth <= 8)
		font_test_char_width = 13;
	else if (bitwidth <= 12)
		font_test_char_width = 10;
	else
		font_test_char_width = 8;


	sprintf ("FONT %d", menu_selection+1);
	font_render_string_left (&font_mono5, 0, 1, sprintf_buffer);
	sprintf_far_string (names_of_fonts + menu_selection);
	font_render_string_right (&font_mono5, 127, 1, sprintf_buffer);
	dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 8);

	sprintf ("%*s", font_test_char_width, font_test_alphabet + font_test_offset);
	print_row_center (font, 20);
	dmd_show_low ();
}

void font_test_left (void)
{
	bounded_decrement (font_test_offset, 0);
	sound_send (SND_TEST_CHANGE);
}

void font_test_right (void)
{
	bounded_increment (font_test_offset, sizeof (font_test_alphabet) - font_test_char_width - 1);
	sound_send (SND_TEST_CHANGE);
}

struct window_ops font_test_window = {
	INHERIT_FROM_BROWSER,
	.init = font_test_init,
	.draw = font_test_draw,
	.left = font_test_left,
	.right = font_test_right,
};


struct menu dev_font_test_item = {
	.name = "FONT TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &font_test_window, NULL } },
};

/**********************************************************/

struct deff_leff_ops {
	void (*start) (U8 id);
	void (*stop) (U8 id);
	bool (*is_running) (U8);
};

struct deff_leff_ops *deff_leff_test_ops;
bool deff_leff_last_active;

static bool deff_test_running (U8 id)
{
	return (deff_get_active () == id);
}

struct deff_leff_ops dev_deff_ops = {
	.start = deff_start,
	.stop = deff_stop,
	.is_running = deff_test_running,
};

struct deff_leff_ops dev_leff_ops = {
	.start = leff_start,
	.stop = leff_stop,
	.is_running = leff_running_p,
};


/** A thread for updating the currently running deff or leff.
This thread polls the currently selected item and updates the
display accordingly. */
void deff_leff_thread (void)
{
	bool is_active = deff_leff_test_ops->is_running (menu_selection);
	deff_leff_last_active = !is_active;
	window_redraw ();

	for (;;)
	{
		if (is_active != deff_leff_last_active)
		{
			if (deff_leff_test_ops == &dev_deff_ops) 
			{
				if (is_active == FALSE)
				{
					window_redraw ();
					sprintf_far_string (names_of_deffs + menu_selection);
					print_row_center (&font_var5, 12);
					browser_print_operation ("STOPPED");
				}
			}
			else
			{
				window_redraw ();
				sprintf_far_string (names_of_leffs + menu_selection);
				print_row_center (&font_var5, 12);
				if (is_active == TRUE)
					browser_print_operation ("RUNNING");
				else
					browser_print_operation ("STOPPED");
			}
		}
		deff_leff_last_active = is_active;
		task_sleep (TIME_200MS);
		is_active = deff_leff_test_ops->is_running (menu_selection);
	}
}


void deff_leff_init (void)
{
	extern struct menu dev_deff_test_item;
	struct menu *m = win_top->w_class.priv;

	browser_init ();
	browser_min = 1;
	browser_item_number = browser_decimal_item_number;

	if (m == &dev_deff_test_item)
	{
		deff_leff_test_ops = &dev_deff_ops;
		browser_max = MAX_DEFFS-1;
	}
	else
	{
		deff_leff_test_ops = &dev_leff_ops;
		browser_max = MAX_LEFFS-1;
	}
	deff_leff_last_active = FALSE;
}

void deff_leff_up (void) { browser_up (); deff_leff_last_active++; }
void deff_leff_down (void) { browser_down (); deff_leff_last_active++; }

void deff_leff_enter (void)
{
	deff_leff_last_active = 0x55;
	if (deff_leff_test_ops->is_running (menu_selection))
	{
		/* deff/leff already running, so stop it */
		deff_leff_test_ops->stop (menu_selection);
		sound_send (SND_TEST_ESCAPE);
	}
	else
	{
		/* deff/leff not running, so start it */
		deff_leff_test_ops->start (menu_selection);
		sound_send (SND_TEST_ENTER);
	}
	deff_leff_last_active++;
}

void deff_leff_exit (void)
{
	/* Note : escaping out of a running deff in test mode
	has been known to cause a crash, but this hasn't been
	seen in a while. */
	deff_stop_all ();
	leff_stop_all ();
}

struct window_ops deff_leff_window = {
	INHERIT_FROM_BROWSER,
	.init = deff_leff_init,
	.thread = deff_leff_thread,
	.enter = deff_leff_enter,
	.up = deff_leff_up,
	.down = deff_leff_down,
	.exit = deff_leff_exit,
};

struct menu dev_deff_test_item = {
	.name = "DISPLAY EFFECTS",
	.flags = M_ITEM,
	.var = { .subwindow = { &deff_leff_window, NULL } },
};

struct menu dev_leff_test_item = {
	.name = "LAMP EFFECTS",
	.flags = M_ITEM,
	.var = { .subwindow = { &deff_leff_window, NULL } },
};

/******* Display Stress Test ***************/


void deff_stress_thread (void)
{
	U8 dn;
	U8 start_stop_flag;
	U8 delay;

	/* The deff stress test continuously starts and stops
	display effects randomly. */
	for (;;)
	{
		do {
			dn = random_scaled (MAX_DEFFS);
		} while (dn == DEFF_NULL);
		start_stop_flag = random_scaled (2);
		delay = random_scaled (TIME_200MS);
		delay += TIME_33MS;

		if (start_stop_flag)
		{
			deff_start (dn);
		}
		else
		{
			deff_stop (dn);
		}

		task_sleep (delay);
	}
}

struct window_ops deff_stress_window = {
	DEFAULT_WINDOW,
	.thread = deff_stress_thread,
	.exit = deff_stop_all,
};

struct menu dev_deff_stress_test_item = {
	.name = "DEFF STRESS TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &deff_stress_window, NULL } },
};


/************ Symbol Test *****************/

void symbol_test_init (void)
{
	browser_init ();
	browser_min = 1;
	browser_max = BM_LAST-1;
}

void symbol_test_draw (void)
{
	union dmd_coordinate coord;
	
	browser_draw ();
	coord.x = 96;
	coord.y = 20;
	bitmap_draw (coord, menu_selection);
}

struct window_ops symbol_test_window = {
	INHERIT_FROM_BROWSER,
	.init = symbol_test_init,
	.draw = symbol_test_draw,
};

struct menu symbol_test_item = {
	.name = "SYMBOL BROWSER",
	.flags = M_ITEM,
	.var = { .subwindow = { &symbol_test_window, NULL } },
};



/*********** Lampsets **********************/

U8 lamplist_update_mode;
U8 lamplist_update_speed;

void lamplist_init (void)
{
	browser_init ();
	browser_min = 1;
	browser_max = MAX_LAMPLIST-1;
	browser_item_number = browser_decimal_item_number;
	lamplist_update_mode = 0;
	lamplist_update_speed = TIME_16MS;
}


void lamplist_draw (void)
{
	browser_draw ();
	sprintf_far_string (names_of_lamplists + menu_selection);
	print_row_center (&font_var5, 12);

	sprintf ("SPEED %d", lamplist_update_speed);
	print_row_center (&font_var5, 21);

	switch (lamplist_update_mode)
	{
		case 0: sprintf ("ENABLE"); break;
		case 1: sprintf ("TOGGLE"); break;
		case 2: sprintf ("STEP UP"); break;
		case 3: sprintf ("STEP DOWN"); break;
		case 4: sprintf ("BUILD UP"); break;
		case 5: sprintf ("BUILD DOWN"); break;
		case 6: sprintf ("ROTATE NEXT"); break;
		case 7: sprintf ("ROTATE PREV"); break;
	}
	font_render_string_center (&font_var5, 94, 21, sprintf_buffer);

	/* Restart the update thread so that the old lamps are
	cleared before the new effect is started */
	window_start_thread ();
}


void lamplist_update (void)
{
	leff_data_t *cdata;
	cdata = task_init_class_data (task_getpid (), leff_data_t);
	lamp_all_off ();
	for (;;)
	{
		cdata->apply_delay = lamplist_update_speed;
		switch (lamplist_update_mode)
		{
			case 0: 
				lamp_all_off ();
				lamplist_apply (menu_selection, lamp_on); 
				break;
			case 1: lamplist_apply (menu_selection, lamp_toggle);
				break;
			case 2: lamplist_step_increment (menu_selection, lamp_matrix);
				break;
			case 3: lamplist_step_decrement (menu_selection, lamp_matrix);
				break;
			case 4: lamplist_build_increment (menu_selection, lamp_matrix);
				break;
			case 5: lamplist_build_decrement (menu_selection, lamp_matrix);
				break;
			case 6: lamplist_rotate_next (menu_selection, lamp_matrix);
				break;
			case 7: lamplist_rotate_previous (menu_selection, lamp_matrix);
				break;
		}
		task_sleep (TIME_200MS);
	}
}

void lamplist_test_slower (void)
{
	if (lamplist_update_speed > 1)
		lamplist_update_speed--;
}

void lamplist_test_faster (void)
{
	lamplist_update_speed++;
}

void lamplist_test_mode_change (void)
{
	lamplist_update_mode++;
	if (lamplist_update_mode == 8)
		lamplist_update_mode = 0;
}

struct window_ops dev_lamplist_window = {
	INHERIT_FROM_BROWSER,
	.init = lamplist_init,
	.exit = lamp_all_off,
	.draw = lamplist_draw,
	.thread = lamplist_update,
	.left = lamplist_test_slower,
	.right = lamplist_test_faster,
	.enter = lamplist_test_mode_change,
};

struct menu dev_lamplist_test_item = {
	.name = "LAMPLISTS",
	.flags = M_ITEM,
	.var = { .subwindow = { &dev_lamplist_window, NULL } },
};

/*********** Ball Devices **********************/


void dev_balldev_test_init (void)
{
	browser_init ();
	browser_max = NUM_DEVICES-1;
}

void dev_balldev_test_draw (void)
{
	device_t *dev;
	char *s;

	dev = &device_table[menu_selection];
	if (likely (dev && dev->props))
	{
		sprintf ("DEV %d. %s", menu_selection, dev->props->name);
		print_row_center (&font_var5, 2);
	
		sprintf ("COUNT %d/%d", dev->actual_count, dev->size);
		font_render_string (&font_var5, 4, 7, sprintf_buffer);

		sprintf ("HOLD %d", dev->max_count);
		font_render_string (&font_var5, 4, 13, sprintf_buffer);

		sprintf ("SOL %d", dev->props->sol+1);
		font_render_string (&font_var5, 4, 19, sprintf_buffer);

		sprintf ("COUNTED %d", counted_balls);
		font_render_string (&font_var5, 64, 7, sprintf_buffer);

		sprintf ("MISSING %d", missing_balls);
		font_render_string (&font_var5, 64, 13, sprintf_buffer);

		sprintf ("LIVE/LOCKS %d/%d", live_balls, kickout_locks);
		font_render_string (&font_var5, 64, 19, sprintf_buffer);

		switch (browser_action)
		{
			case 0: default: s = "EJECT 1"; break;
			case 1: s = "EJECT ALL"; break;
			case 2: s = "ENABLE LOCK"; break;
			case 3: s = "DISABLE LOCK"; break;
		}
		font_render_string_center (&font_mono5, 64, 28, s);
	}

	dmd_show_low ();
}


void dev_balldev_test_thread (void)
{
	U8 last_count = 0;
	device_t *last_dev = &device_table[menu_selection];
	U8 i;

	for (;;)
	{
		device_t *dev = &device_table[menu_selection];

		for (i=0; i < 8; i++)
		{
			if ((last_count != dev->actual_count) &&
				(last_dev == dev))
			{
				sound_send (SND_TEST_CHANGE);
				dmd_alloc_low_clean ();
				dev_balldev_test_draw ();
			}
			else if (i == 7)
			{
				dmd_alloc_low_clean ();
				dev_balldev_test_draw ();
			}
			last_count = dev->actual_count;
			last_dev = dev;
			task_sleep (TIME_66MS);
		}

#if defined(MACHINE_LAUNCH_SOLENOID) && defined(MACHINE_LAUNCH_SWITCH)
		if (switch_poll (MACHINE_LAUNCH_SWITCH))
		{
			sol_pulse (MACHINE_LAUNCH_SOLENOID);
		}
#endif
	}
}

void dev_balldev_test_enter (void)
{
	device_t *dev = &device_table[menu_selection];
	if ((dev == NULL) || (dev->props == NULL))
		return;
	sound_send (SND_TEST_CHANGE);
	switch (browser_action)
	{
		case 0:
			device_request_kick (dev);
			break;
		case 1:
			device_request_empty (dev);
			break;
		case 2:
			device_enable_lock (dev);
			break;
		case 3:
			device_disable_lock (dev);
			break;
	}
}

void dev_balldev_test_change (void)
{
	browser_action++;
	if (browser_action == 4)
		browser_action = 0;
	sound_send (SND_TEST_CHANGE);
	dev_balldev_test_draw ();
}

struct window_ops dev_balldev_test_window = {
	INHERIT_FROM_BROWSER,
	.init = dev_balldev_test_init,
	.draw = dev_balldev_test_draw,
	.thread = dev_balldev_test_thread,
	.enter = dev_balldev_test_enter,
	.left = dev_balldev_test_change,
	.right = dev_balldev_test_change,
};

struct menu dev_balldev_test_item = {
	.name = "BALL DEVICES",
	.flags = M_ITEM,
#if (NUM_DEVICES > 0)
	.var = { .subwindow = { &dev_balldev_test_window, NULL } },
#endif
};

/************* Transition Test ******************/


dmd_transition_t *transition_table[] = {
	&trans_scroll_up,
	&trans_scroll_up_avg,
	&trans_scroll_up_slow,
	&trans_scroll_down,
	&trans_scroll_left,
	&trans_scroll_right,
	&trans_sequential_boxfade,
	&trans_random_boxfade,
	&trans_vstripe_left2right,
	&trans_vstripe_right2left,
	&trans_bitfade_slow,
	&trans_bitfade_fast,
};

#define NUM_TRANSITIONS \
	(sizeof (transition_table) / sizeof (dmd_transition_t *))

void dev_trans_test_init (void)
{
	browser_init ();
	browser_item_number = browser_decimal_item_number;
	browser_max = NUM_TRANSITIONS-1;
}


void dev_trans_test_enter (void)
{
	dmd_alloc_low_clean ();
	dmd_sched_transition (transition_table[menu_selection]);
	font_render_string_center (&font_fixed10, 64, 16, "TRANSITION");
	dmd_show_low ();
	task_sleep_sec (1);
}

struct window_ops dev_trans_test_window = {
	INHERIT_FROM_BROWSER,
	.init = dev_trans_test_init,
	.enter = dev_trans_test_enter,
};

struct menu dev_trans_test_item = {
	.name = "DMD TRANSITIONS",
	.flags = M_ITEM,
	.var = { .subwindow = { &dev_trans_test_window, NULL } },
};


/**********************************************************************/

void dev_random_test_task (void)
{
	U16 i;
	U8 *rowcount;

	/* Allocate a temporary buffer to store the histogram data */
	rowcount = malloc (32);
	if (rowcount == NULL)
		task_exit ();

	/* Initialize the histogram */
	for (i=0; i < 32; i++)
		rowcount[i] = 0;

	/* Unlike normal items, all drawing is done from here, not from
	the draw function. */
	dmd_alloc_low_clean ();
	dmd_show_low ();

	/* Allocate 200 random numbers from 0-31.  Increment a count for
	the number of times that number was chosen.  As long as the
	count doesn't exceed the width of the display, grow a horizontal
	bar for that value. */
	for (i=0; i < 200; i++)
	{
		U8 r = random ();
		r &= 31;
		if (rowcount[r] < 16)
		{
			U16 offset = ((U16)r << 4) + rowcount[r];
			dmd_low_buffer[offset] = 0xFF;
			rowcount[r]++;
		}
		task_sleep (TIME_33MS);
	}

	dmd_invert_page (dmd_low_buffer);
	task_sleep (TIME_200MS);
	dmd_invert_page (dmd_low_buffer);
	font_render_string_right (&font_var5, 127, 1, "PRESS ENTER");
	font_render_string_right (&font_var5, 127, 7, "TO REPEAT");

	free (rowcount);
	task_exit ();
}

void dev_random_test_enter (void)
{
	/* Enter while the test is running does nothing; otherwise
	it restarts the test. */
	task_create_gid1 (GID_WINDOW_THREAD, dev_random_test_task);
}

void dev_random_test_init (void)
{
	dev_random_test_enter ();
}

struct window_ops dev_random_test_window = {
	DEFAULT_WINDOW,
	.init = dev_random_test_init,
	.enter = dev_random_test_enter,
};

struct menu dev_random_test_item = {
	.name = "RANDOM TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &dev_random_test_window, NULL } },
};

/**********************************************************************/

void dev_force_error_init (void)
{
	fatal (ERR_NMI);
}

struct window_ops dev_force_error_window = {
	DEFAULT_WINDOW,
	.init = dev_force_error_init,
};

struct menu dev_force_error_item = {
	.name = "FORCE ERROR",
	.flags = M_ITEM,
	.var = { .subwindow = { &dev_force_error_window, NULL } },
};


/**********************************************************************/

extern const U8 fif_freewpc_logo[];
const U8 *dev_frametest_ptr;
const U8 *dev_frametest_next_ptr;

void dev_frametest_init (void)
{
	dev_frametest_ptr = fif_freewpc_logo;
	dev_frametest_next_ptr = NULL;
}

void dev_frametest_draw (void)
{
	if (dev_frametest_ptr)
	{
		dmd_alloc_low_high ();
		dev_frametest_next_ptr = dmd_draw_fif1 (dev_frametest_ptr);
		dmd_show2 ();
		dev_frametest_ptr = NULL;
	}
}

void dev_frametest_advance (void)
{
	dev_frametest_ptr = dev_frametest_next_ptr;
	if (far_read8 ((U8 *)dev_frametest_ptr, FIF_PAGE) > 2)
	{
		dev_frametest_ptr = fif_freewpc_logo;
	}
}

struct window_ops dev_frametest_window = {
	INHERIT_FROM_BROWSER,
	.init = dev_frametest_init,
	.draw = dev_frametest_draw,
	.up = dev_frametest_advance,
	.down = null_function,
	.enter = null_function,
};

struct menu dev_frametest_item = {
	.name = "DMD FRAME TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &dev_frametest_window, NULL } },
};

/**********************************************************************/

#define SCHED_TEST_DURATION TIME_500MS
#define SCHED_TEST_WORKERS  16
#define SCHED_LOCAL_COUNT   16

U16 sched_test_count;
volatile U8 *local_data_pointer;

void sched_test_task (void)
{
	volatile U8 local_data[SCHED_LOCAL_COUNT];

	local_data_pointer = local_data;
	for (;;)
	{
		sched_test_count++;
		task_yield ();
	}
}

void sched_test_init (void)
{
	U8 i;
	sched_test_count = 0;

	dmd_alloc_low_clean ();
	dmd_show_low ();

	for (i=0 ; i < SCHED_TEST_WORKERS; i++)
		task_create_gid (GID_SCHED_TEST_WORKER, sched_test_task);
	task_sleep (SCHED_TEST_DURATION);
	task_kill_gid (GID_SCHED_TEST_WORKER);
	sched_test_count *= 2;
}


void sched_test_draw (void)
{
	window_title ("SCHEDULER TEST");
	sprintf ("SCHEDULES PER SEC. = %ld", sched_test_count);
	print_row_center (&font_var5, 10);
	font_render_string_center (&font_var5, 64, 20, "PRESS ENTER TO REPEAT");
	dmd_show_low ();
}


struct window_ops sched_test_window = {
	DEFAULT_WINDOW,
	.init = sched_test_init,
	.draw = sched_test_draw,
	.enter = sched_test_init,
};

struct menu sched_test_item = {
	.name = "SCHEDULER TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &sched_test_window, NULL } },
};

/**********************************************************************/

#define SCORE_TEST_PLAYERS 3

const score_t score_test_increment = { 0x00, 0x01, 0x23, 0x45, 0x60 };

void score_test_init (void)
{
	bcd_t *s;
	for (s = &scores[0][0]; s < &scores[4][0]; s++)
	{
		score_zero (s);
		score_add (s, score_test_increment);
	}
	num_players = 1;
	player_up = 0;
}

void score_test_draw (void)
{
	scores_draw ();
	dmd_show_low ();
}

void score_test_up (void)
{
	if (num_players == player_up)
	{
		num_players++;
		player_up = 0;
		if (num_players > SCORE_TEST_PLAYERS)
			num_players = 1;
	}
	else
	{
		player_up++;
	}
}

void score_test_down (void)
{
	player_up--;
	if (player_up == 0xFF)
	{
		num_players--;
		if (num_players == 0)
		{
			num_players = SCORE_TEST_PLAYERS;
		}
		player_up = num_players;
	}
}

struct window_ops score_test_window = {
	DEFAULT_WINDOW,
	.init = score_test_init,
	.draw = score_test_draw,
	.up = score_test_up,
	.down = score_test_down,
};

struct menu score_test_item = {
	.name = "SCORE TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &score_test_window, NULL } },
};

/**********************************************************************/

#if (MACHINE_PIC == 1)
void pic_test_draw (void)
{
	extern U8 pic_unlock_code[];
	extern __common__ void pic_render_serial_number (void);

	window_title ("SECURITY TEST");

	sprintf ("UNLOCK CODE: %02X %02X %02X",
		pic_unlock_code[0], pic_unlock_code[1], pic_unlock_code[2]);
	font_render_string_left (&font_var5, 1, 9, sprintf_buffer);

	font_render_string_left (&font_var5, 1, 16, "SER. NO.:");
	pic_render_serial_number ();
	font_render_string_left (&font_var5, 40, 16, sprintf_buffer);

	dmd_show_low ();
}

struct window_ops pic_test_window = {
	DEFAULT_WINDOW,
	.draw = pic_test_draw,
};

struct menu pic_test_item = {
	.name = "SECURITY TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &pic_test_window, NULL } },
};
#endif /* MACHINE_PIC */

/**********************************************************************/

void asic_3fff_thread (void)
{
	U8 writeval = 0x96;
	U8 last_readval = 0;

	for (;;)
	{
		U8 readval = readb (WPC_ZEROCROSS_IRQ_CLEAR);
		U8 readval2 = readb (WPC_PERIPHERAL_TIMER_FIRQ_CLEAR);

		dmd_alloc_low_clean ();
		sprintf ("READ IRQ %02X", readval);
		font_render_string_left (&font_mono5, 32, 7, sprintf_buffer);
		sprintf ("READ FIRQ %02X", readval2);
		font_render_string_left (&font_mono5, 32, 15, sprintf_buffer);
		sprintf ("WRITE IRQ %02X", writeval);
		font_render_string_left (&font_mono5, 32, 23, sprintf_buffer);
		dmd_show_low ();
		last_readval = readval;
		task_sleep (TIME_100MS);
	}
}

struct window_ops asic_3fff_window = {
	DEFAULT_WINDOW,
	.thread = asic_3fff_thread,
};

struct menu asic_3fff_item = {
	.name = "ASIC 3FFF TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &asic_3fff_window, NULL } },
};

/**********************************************************************/

struct menu *dev_menu_items[] = {
#if defined(MACHINE_DMD) && !defined(CONFIG_NATIVE)
	&dev_font_test_item,
#endif
	&dev_deff_test_item,
	&dev_leff_test_item,
	&dev_lamplist_test_item,
	&dev_balldev_test_item,
	&dev_random_test_item,
	&dev_trans_test_item,
	&dev_force_error_item,
	&dev_frametest_item,
	&dev_deff_stress_test_item,
#if defined(MACHINE_DMD) && !defined(CONFIG_NATIVE)
	&symbol_test_item,
#endif
	&sched_test_item,
	&score_test_item,
#if (MACHINE_PIC == 1)
	&pic_test_item,
#endif
	&asic_3fff_item,
	NULL,
};

struct menu development_menu = {
	.name = "DEVELOPMENT",
	.flags = M_MENU | M_LETTER_PREFIX,
	.var = { .submenus = dev_menu_items },
};

/**********************************************************************/

void factory_adjust_confirm (void)
{
	adj_reset_all ();
	confirm_enter ();
}

struct window_ops factory_adjust_window = {
	INHERIT_FROM_CONFIRM_WINDOW,
	.enter = factory_adjust_confirm,
};

struct menu factory_adjust_item = {
	.name = "FACTORY ADJUST",
	.flags = M_ITEM,
	.var = { .subwindow = { &factory_adjust_window, NULL } },
};

/**********************************************************************/

void factory_reset_confirm (void)
{
	adj_reset_all ();
	/* TODO : this should also clear audits, reset the high scores,
	 * and reset the custom message/game ID */
	confirm_enter ();
}

struct window_ops factory_reset_window = {
	INHERIT_FROM_CONFIRM_WINDOW,
	.enter = factory_reset_confirm,
};

struct menu factory_reset_item = {
	.name = "FACTORY RESET",
	.flags = M_ITEM,
	.var = { .subwindow = { &factory_reset_window, NULL } },
};

/**********************************************************************/

void clear_audits_confirm (void)
{
	audit_reset ();
	confirm_enter ();
}

struct window_ops clear_audits_window = {
	INHERIT_FROM_CONFIRM_WINDOW,
	.enter = clear_audits_confirm,
};

struct menu clear_audits_item = {
	.name = "CLEAR AUDITS",
	.flags = M_ITEM,
	.var = { .subwindow = { &clear_audits_window, NULL } },
};

/**********************************************************************/

void clear_coins_confirm (void)
{
	confirm_enter ();
}

struct window_ops clear_coins_window = {
	INHERIT_FROM_CONFIRM_WINDOW,
	.enter = clear_coins_confirm,
};

struct menu clear_coins_item = {
	.name = "CLEAR COINS",
	.flags = M_ITEM,
	.var = { .subwindow = { &clear_coins_window, NULL } },
};

/**********************************************************************/

void reset_hstd_window_confirm (void)
{
	high_score_reset ();
	confirm_enter ();
}

struct window_ops reset_hstd_window = {
	INHERIT_FROM_CONFIRM_WINDOW,
	.enter = reset_hstd_window_confirm,
};

struct menu reset_hstd_item = {
	.name = "RESET H.S.T.D.",
	.flags = M_ITEM,
	.var = { .subwindow = { &reset_hstd_window, NULL } },
};

/**********************************************************************/

void set_time_init (void)
{
	rtc_begin_modify ();
}

void set_time_exit (void)
{
	rtc_end_modify (0);
}

void set_time_thread (void)
{
	for (;;)
	{
		task_sleep_sec (5);
		rtc_show_date_time ();
	}
}

void set_time_window_draw (void)
{
	rtc_show_date_time ();
}

void set_time_up (void)
{
	rtc_modify_field (1);
}

void set_time_down (void)
{
	rtc_modify_field (0);
}

void set_time_enter (void)
{
	rtc_next_field ();
}

struct window_ops set_time_window = {
	DEFAULT_WINDOW,
	.init = set_time_init,
	.exit = set_time_exit,
	.draw = set_time_window_draw,
	.up = set_time_up,
	.down = set_time_down,
	.enter = set_time_enter,
	.thread = set_time_thread,
};

struct menu set_time_item = {
	.name = "SET TIME/DATE",
	.flags = M_ITEM,
	.var = { .subwindow = { &set_time_window, NULL } },
};

/**********************************************************************/

#ifdef WMSLY_CORRECT
struct menu custom_message_item = {
	.name = "CUSTOM MESSAGE",
	.flags = M_ITEM,
};

struct menu set_gameid_item = {
	.name = "SET GAME I.D.",
	.flags = M_ITEM,
};
#endif

/**********************************************************************/

void clear_credits_confirm (void)
{
	credits_clear ();
	confirm_enter ();
}

struct window_ops clear_credits_window = {
	INHERIT_FROM_CONFIRM_WINDOW,
	.enter = clear_credits_confirm,
};

struct menu clear_credits_item = {
	.name = "CLEAR CREDITS",
	.flags = M_ITEM,
	.var = { .subwindow = { &clear_credits_window, NULL } },
};

/**********************************************************************/

void burnin_test_draw (void)
{
	dmd_show_low ();
}


void burnin_test_thread (void)
{
	extern void all_lamp_test_thread (void);

	/* TODO : need a good design for how burnin test
	should work.  Also a method for detecting errors,
	and for counting total burnin time as an audit. */
	task_create_peer (all_lamp_test_thread);
	task_exit ();
}

void burnin_test_exit (void)
{
}

struct window_ops burnin_test_window = {
	DEFAULT_WINDOW,
	.draw = burnin_test_draw,
	.thread = burnin_test_thread,
	.exit = burnin_test_exit,
};

struct menu burnin_item = {
	.name = "AUTO BURN-IN",
	.flags = M_ITEM,
	.var = { .subwindow = { &burnin_test_window, NULL } },
};

/**********************************************************************/

void presets_init (void)
{
	browser_init ();
	browser_max = preset_count () - 1;
}

void presets_draw_finish (void)
{
	task_sleep (TIME_500MS);
	font_render_string_center (&font_var5, 64, 20, "PRESS ENTER TO INSTALL");
	font_render_string_center (&font_var5, 64, 27, "PRESS START TO VIEW DETAILS");
	task_exit ();
}

void presets_draw (void)
{
	font_render_string_left (&font_mono5, 1, 1, "PRESETS");
	sprintf ("%d.", menu_selection+1);
	font_render_string_left (&font_mono5, 1, 9, sprintf_buffer);
	preset_render_name (menu_selection);
	font_render_string_left (&font_mono5, 15, 9, sprintf_buffer);

	/* Is it installed now? */	
	font_render_string_right (&font_mono5, 127, 9, 
		preset_installed_p (menu_selection) ? "YES" : "NO");

	task_recreate_gid (GID_SLOW_DRAW_FINISH, presets_draw_finish);
	dmd_show_low ();
}


void presets_enter (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 8, "INSTALLING PRESET");
	preset_render_name (menu_selection);
	print_row_center (&font_mono5, 16);
	dmd_show_low ();
	task_sleep_sec (2);
	sound_send (SND_TEST_CONFIRM);
	preset_install_from_test ();
}

void presets_start (void)
{
	far_task_create_gid (GID_WINDOW_THREAD, preset_show_components, TEST2_PAGE);
}

struct window_ops presets_window = {
	INHERIT_FROM_BROWSER,
	.init = presets_init,
	.draw = presets_draw,
	.enter = presets_enter,
	.start = presets_start,
};

struct menu presets_menu_item = {
	.name = "PRE-SETS",
	.flags = M_ITEM,
	.var = { .subwindow = { &presets_window, NULL } },
};

/**********************************************************************/

void revoke_init (void)
{
	extern U8 freewpc_accepted[];
	extern void freewpc_init (void);

	dmd_alloc_low_clean ();
	dmd_show_low();
	task_sleep_sec (1);

	wpc_nvram_get ();
	freewpc_accepted[0] = 0;
	freewpc_accepted[1] = 0;
	freewpc_accepted[2] = 0;
	wpc_nvram_put ();

	freewpc_init ();
}

struct window_ops revoke_window = {
	DEFAULT_WINDOW,
	.init = revoke_init,
};

struct menu revoke_item = {
	.name = "REVOKE FREEWPC",
	.flags = M_ITEM,
	.var = { .subwindow = { &revoke_window, NULL } },
};


struct menu *util_menu_items[] = {
	&clear_audits_item,
	&clear_coins_item,
	&reset_hstd_item,
	&set_time_item,
#ifdef WMSLY_CORRECT
	&custom_message_item,
	&set_gameid_item,
#endif
	&factory_adjust_item,
	&factory_reset_item,
	&presets_menu_item,
	&clear_credits_item,
	&burnin_item,
	&revoke_item,
	NULL,
};

struct menu utilities_menu = {
	.name = "UTILITIES",
	.flags = M_MENU | M_LETTER_PREFIX,
	.var = { .submenus = util_menu_items },
};

/**********************************************************************/

struct menu main_audits_item = {
	.name = "MAIN AUDITS",
	.flags = M_ITEM,
	.var = { .subwindow = { &audit_browser_window, main_audits } },
};

struct menu earnings_audits_item = {
	.name = "EARNINGS AUDITS",
	.flags = M_ITEM,
	.var = { .subwindow = { &audit_browser_window, earnings_audits } },
};

struct menu standard_audits_item = {
	.name = "STANDARD AUDITS",
	.flags = M_ITEM,
	.var = { .subwindow = { &audit_browser_window, standard_audits } },
};

struct menu feature_audits_item = {
	.name = "FEATURE AUDITS",
	.flags = M_ITEM,
#ifdef MACHINE_FEATURE_AUDITS
	.var = { .subwindow = { &audit_browser_window, MACHINE_FEATURE_AUDITS } },
#endif
};

struct menu histogram_audits_item = {
	.name = "HISTOGRAMS",
	.flags = M_ITEM,
};


struct menu timestamp_audits_item = {
	.name = "TIME-STAMPS",
	.flags = M_ITEM,
};

struct menu *audit_menu_items[] = {
	&main_audits_item,
	&earnings_audits_item,
	&standard_audits_item,
	&feature_audits_item,
	&histogram_audits_item,
	&timestamp_audits_item,
	NULL,
};

struct menu bookkeeping_menu = {
	.name = "BOOKKEEPING",
	.flags = M_MENU | M_LETTER_PREFIX,
	.var = { .submenus = audit_menu_items },
};

/**********************************************************************/

void printout_thread (void)
{
	extern __common__ void print_all_audits (void);
	print_all_audits ();

#if 0
	task_setgid (0);
	window_pop ();
#endif
	task_exit ();
}


void printout_draw (void)
{
	font_render_string_center (&font_mono5, 64, 16, "PRINTING...");
	dmd_show_low ();
}


struct window_ops printout_window = {
	DEFAULT_WINDOW,
	.thread = printout_thread,
	.draw = printout_draw,
};


struct menu print_all_data_item = {
	.name = "ALL DATA",
	.flags = M_ITEM,
	.var = { .subwindow = { &printout_window, NULL } },
};


struct menu *printouts_menu_items[] = {
	&print_all_data_item,
	NULL,
};


struct menu printouts_menu = {
	.name = "PRINTOUTS",
	.flags = M_MENU | M_LETTER_PREFIX,
	.var = { .submenus = printouts_menu_items },
};


/**********************************************************************/

struct menu standard_adjustments_menu = {
	.name = "STANDARD ADJ.",
	.flags = M_ITEM,
	.var = { .subwindow = { &adj_browser_window, standard_adjustments } },
};

struct menu feature_adjustments_menu = {
	.name = "FEATURE ADJ.",
	.flags = M_ITEM,
	.var = { .subwindow = { &adj_browser_window, feature_adjustments } },
};

struct menu pricing_adjustments_menu = {
	.name = "PRICING ADJ.",
	.flags = M_ITEM,
#ifndef FREE_ONLY
	.var = { .subwindow = { &adj_browser_window, pricing_adjustments } },
#endif
};

struct menu hstd_adjustments_menu = {
	.name = "H.S.T.D. ADJ.",
	.flags = M_ITEM,
	.var = { .subwindow = { &adj_browser_window, hstd_adjustments } },
};

struct menu printer_adjustments_menu = {
	.name = "PRINTER ADJ.",
	.flags = M_ITEM,
	.var = { .subwindow = { &adj_browser_window, printer_adjustments } },
};

struct menu *adj_menu_items[] = {
	&standard_adjustments_menu,
	&feature_adjustments_menu,
	&pricing_adjustments_menu,
	&hstd_adjustments_menu,
	&printer_adjustments_menu,
	NULL,
};

struct menu adjustments_menu = {
	.name = "ADJUSTMENTS",
	.flags = M_MENU | M_LETTER_PREFIX,
	.var = { .submenus = adj_menu_items },
};

/**********************************************************************/

extern __test2__ void switch_matrix_draw (void);
extern __test2__ void switch_edges_update (void);
extern __test2__ void switch_levels_update (void);

void switch_window_title (const char *title)
{
	font_render_string_center (&font_mono5, 80, 3, title);
}


void switch_edges_draw (void)
{
	switch_matrix_draw ();
	switch_window_title ("SWITCH EDGES");
	dmd_show_low ();
}

void switch_edges_thread (void)
{
	for (;;)
	{
		switch_edges_update ();
		task_sleep (TIME_100MS);
	}
}

struct window_ops switch_edges_window = {
	INHERIT_FROM_BROWSER,
	.draw = switch_edges_draw,
	.thread = switch_edges_thread,
	.up = null_function,
	.down = null_function,
};

struct menu switch_edges_item = {
	.name = "SWITCH EDGES",
	.flags = M_ITEM,
	.var = { .subwindow = { &switch_edges_window, NULL } },
};


void switch_levels_draw (void)
{
	switch_matrix_draw ();
	switch_window_title ("SWITCH LEVELS");
	dmd_show_low ();
}

void switch_levels_thread (void)
{
	for (;;)
	{
		switch_levels_update ();
		task_sleep (TIME_100MS);
	}
}


struct window_ops switch_levels_window = {
	INHERIT_FROM_BROWSER,
	.draw = switch_levels_draw,
	.thread = switch_levels_thread,
	.up = null_function,
	.down = null_function,
};

struct menu switch_levels_item = {
	.name = "SWITCH LEVELS",
	.flags = M_ITEM,
	.var = { .subwindow = { &switch_levels_window, NULL } },
};

/*************** Single Switch Test ***********************/

void switch_item_number (U8 val)
{
	if (val < NUM_DEDICATED_SWITCHES)
		sprintf ("D%d", val+1);
	else if (val >= NUM_PF_SWITCHES + NUM_DEDICATED_SWITCHES)
		sprintf ("F%d", val - (NUM_PF_SWITCHES + NUM_DEDICATED_SWITCHES) + 1);
	else
	{
		val -= NUM_DEDICATED_SWITCHES;
		sprintf ("%d%d", (val / 8)+1, (val % 8)+1);
	}
}

void single_switch_init (void)
{
	browser_init ();
	browser_item_number = switch_item_number;
	browser_max = NUM_SWITCHES-1;
}

void single_switch_draw (void)
{
	U8 sel = win_top->w_class.menu.selected;
	const char *level;
	const char *active;

	switch_matrix_draw ();
	switch_window_title ("SINGLE SWITCHES");

	/* Display a description of the switch */
	(*browser_item_number) (menu_selection);
	font_render_string_left (&font_mono5, 34, 9, sprintf_buffer);

	sprintf_far_string (names_of_switches + menu_selection);
	font_render_string_left (&font_var5, 50, 9, sprintf_buffer);

	if (switch_is_opto (sel))
		font_render_string_center (&font_var5, 116, 19, "OPTO");

	/* Display the state of the switch */
	active = switch_poll_logical (sel) ? "ACTIVE" : "INACTIVE";
	level = switch_poll (sel) ? "CLOSED" : "OPEN";
	sprintf ("%s-%s", active, level);
	font_render_string_center (&font_var5, 68, 19, sprintf_buffer);
	
	dmd_show_low ();
}

void single_switch_thread (void)
{
	U8 *sel = &win_top->w_class.menu.selected;
	U8 selected = *sel;
	U8 sw_state = switch_poll (*sel);
	U8 sw_poll;

	for (;;)
	{
		task_sleep (TIME_33MS);
		if ((sw_poll = switch_poll (*sel)) != sw_state)
		{
			if (*sel == selected)
			{
				sound_send (SND_TEST_CHANGE);
				if (((*sel != SW_UP) && (*sel != SW_DOWN)) || !sw_poll)
					single_switch_draw ();
			}
			else
			{
				selected = *sel;
			}
			sw_state = sw_poll;
		}
	}
}

struct window_ops single_switch_window = {
	INHERIT_FROM_BROWSER,
	.init = single_switch_init,
	.draw = single_switch_draw,
	.thread = single_switch_thread,
};


struct menu single_switches_item = {
	.name = "SINGLE SWITCHES",
	.flags = M_ITEM,
	.var = { .subwindow = { &single_switch_window, NULL } },
};

/****************** Sound Test **************************/

U8 sound_test_set;

void sound_test_set_draw (void)
{
	char *s;
	switch (sound_test_set)
	{
#if (MACHINE_DCS == 0)
		case 0:
		default:
			s = "SET 1";
			break;

		case 1:
			s = "SET 2";
			break;
#else
		case 0:
		default:
			s = "MUSIC";
			break;

		case 1:
			s = "SPEECH";
			break;

		case 3:
			s = "MISC/TEST MODE";
			break;
#endif
	}
	font_render_string_center (&font_mono5, 64, 9, s);
}

void sound_test_set_change (void)
{
	sound_test_set++;
#if (MACHINE_DCS == 0)
	if (sound_test_set > 1)
#else
	if (sound_test_set == 2)
		sound_test_set = 3;
	if (sound_test_set > 3)
#endif
		sound_test_set = 0;
	win_top->w_class.menu.selected = 0;
}

void sound_test_play (U8 sel)
{
	sound_code_t snd = ((U16)sound_test_set << 8) + sel;
	sound_send (snd);
}

void sound_test_repeat (void)
{
	browser_print_operation ("PLAYING...");
	sound_test_play (menu_selection);
	task_sleep (TIME_100MS * 2);
}

void sound_test_draw (void)
{
	U8 sel = menu_selection;

	browser_draw ();
	sound_test_set_draw ();

	if (browser_action == 0xFF)
	{
		if (browser_last_selection_update != sel)
			sound_test_play (sel);
		browser_print_operation ("SOUND ON");
	}
	else
	{
		browser_print_operation ("SOUND OFF");
	}
	browser_last_selection_update = sel;
}

void sound_test_enter (void)
{
	browser_action = ~browser_action;
	browser_last_selection_update = menu_selection + 1;

	if (browser_action == 0)
	{
		task_sleep (TIME_100MS);
		sound_reset ();
	}
	browser_draw ();
}

void sound_test_init (void)
{	
	browser_init ();
	sound_test_set = 0;
}

struct window_ops sound_test_window = {
	INHERIT_FROM_BROWSER,
	.init = sound_test_init,
	.exit = sound_reset,
	.enter = sound_test_enter,
	.draw = sound_test_draw,
	.left = sound_test_set_change,
	.right = sound_test_set_change,
	.start = sound_test_repeat,
};

struct menu sound_test_item = {
	.name = "SOUND TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &sound_test_window, NULL } },
};

/************ Solenoid and Flasher Test ********************/

/* The browser action stores the pulse width */


/* TODO - if no solenoids/flashers defined, solenoid_test_ok
 * may loop indefinitely */

bool solenoid_test_selection_ok (void)
{
	extern struct window_ops flasher_test_window;

	return (win_top->ops == &flasher_test_window)
		== (MACHINE_SOL_FLASHERP (menu_selection));
}


void solenoid_test_init (void)
{
	browser_init ();
	while (!solenoid_test_selection_ok ())
		menu_selection++;
	browser_item_number = browser_decimal_item_number;
	browser_action = TIME_66MS;
#ifdef NUM_POWER_DRIVES
	browser_max = NUM_POWER_DRIVES-1;
#endif
}

void solenoid_test_draw (void)
{
	char *s;

	browser_draw ();
	switch (browser_action)
	{
		default: s = "ERR"; break;
		case TIME_16MS: s = "16MS"; break;
		case TIME_33MS: s = "33MS"; break;
		case TIME_66MS: s = "66MS"; break;
		case TIME_100MS: s = "100MS"; break;
		case TIME_133MS: s = "133MS"; break;
	}
	font_render_string_center (&font_mono5, 64, 12, s);
	sprintf_far_string (names_of_drives + menu_selection);
	browser_print_operation (sprintf_buffer);
}

void solenoid_test_enter (void)
{
	U8 sel = win_top->w_class.menu.selected;
	task_sleep (TIME_100MS);
	/* TODO : Use 100% duty cycle for now; this probably ought to
	change for certain coils. */
	sol_start (sel, SOL_DUTY_100, browser_action);
	task_sleep (TIME_100MS);
}

void solenoid_test_up (void)
{
	do {
		browser_up ();
	} while (!solenoid_test_selection_ok ());
}

void solenoid_test_down (void)
{
	do {
		browser_down ();
	} while (!solenoid_test_selection_ok ());
}

void solenoid_test_right (void)
{
	if (browser_action == TIME_133MS)
		sound_send (SND_TEST_ABORT);
	else if (browser_action == TIME_16MS)
		browser_action = TIME_33MS;
	else
		browser_action += TIME_33MS;
}

void solenoid_test_left (void)
{
	if (browser_action == TIME_16MS)
		sound_send (SND_TEST_ABORT);
	else if (browser_action == TIME_33MS)
		browser_action = TIME_16MS;
	else
		browser_action -= TIME_33MS;
}

/* TODO - these two window ops are identical; we are only
using two ops to figure out which test we are in.  Better
would be to use one ops struct and use a priv pointer to
sort it out. */
struct window_ops solenoid_test_window = {
	INHERIT_FROM_BROWSER,
	.init = solenoid_test_init,
	.draw = solenoid_test_draw,
	.enter = solenoid_test_enter,
	.left = solenoid_test_left,
	.right = solenoid_test_right,
	.up = solenoid_test_up,
	.down = solenoid_test_down,
};

struct window_ops flasher_test_window = {
	INHERIT_FROM_BROWSER,
	.init = solenoid_test_init,
	.draw = solenoid_test_draw,
	.enter = solenoid_test_enter,
	.left = solenoid_test_left,
	.right = solenoid_test_right,
	.up = solenoid_test_up,
	.down = solenoid_test_down,
};

struct menu solenoid_test_item = {
	.name = "SOLENOID TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &solenoid_test_window, NULL } },
};

struct menu flasher_test_item = {
	.name = "FLASHER TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &flasher_test_window, NULL } },
};

/****************** GI Test **************************/

U8 gi_test_brightness;

U8 gi_test_values[] = {
	0,
	TRIAC_GI_STRING(0),	
	TRIAC_GI_STRING(1),	
	TRIAC_GI_STRING(2),	
	TRIAC_GI_STRING(3),	
	TRIAC_GI_STRING(4),	
	TRIAC_GI_MASK,
};


void gi_test_init (void)
{
	browser_init ();
	browser_max = NUM_GI_TRIACS+1;
	gi_test_brightness = 8;
	triac_disable (TRIAC_GI_MASK);
	triac_leff_allocate (TRIAC_GI_MASK);
}

void gi_test_exit (void)
{
	triac_leff_free (TRIAC_GI_MASK);
	triac_enable (TRIAC_GI_MASK);
}

void gi_test_draw (void)
{
	browser_draw ();

	if (menu_selection == 0)
		browser_print_operation ("ALL OFF");
	else if (menu_selection == NUM_GI_TRIACS+1)
		browser_print_operation ("ALL ON");
	else
	{
		sprintf_far_string (names_of_gi + menu_selection - 1);
		browser_print_operation (sprintf_buffer);
	}

	sprintf ("BRIGHTNESS %d", gi_test_brightness);
	print_row_center (&font_mono5, 29);

	triac_leff_disable (TRIAC_GI_MASK);
	triac_set_brightness (gi_test_values[menu_selection], gi_test_brightness);
}

void gi_test_right (void)
{
	bounded_increment (gi_test_brightness, 8);
}

void gi_test_left (void)
{
	bounded_decrement (gi_test_brightness, 1);
}


struct window_ops gi_test_window = {
	INHERIT_FROM_BROWSER,
	.init = gi_test_init,
	.exit = gi_test_exit,
	.draw = gi_test_draw,
	.left = gi_test_left,
	.right = gi_test_right,
};

struct menu gi_test_item = {
	.name = "GEN. ILLUMINATION",
	.flags = M_ITEM,
	.var = { .subwindow = { &gi_test_window, NULL } },
};


/****************** Lamp Test **************************/

void lamp_test_item_number (U8 val)
{
	sprintf ("%1d%1d", (val / 8) + 1, (val % 8) + 1);
}

void lamp_test_init (void)
{
	browser_init ();
	browser_max = NUM_LAMPS-1;
	browser_item_number = lamp_test_item_number;
}

void lamp_test_draw (void)
{
	lamp_flash_on (menu_selection);
	browser_draw ();
	sprintf_far_string (names_of_lamps + menu_selection);
	browser_print_operation (sprintf_buffer);
}

void lamp_test_up (void)
{
	lamp_flash_off (menu_selection);
	browser_up ();
}

void lamp_test_down (void)
{
	lamp_flash_off (menu_selection);
	browser_down ();
}

struct window_ops lamp_test_window = {
	INHERIT_FROM_BROWSER,
	.init = lamp_test_init,
	.draw = lamp_test_draw,
	.exit = lamp_all_off,
	.up = lamp_test_up,
	.down = lamp_test_down,
};

struct menu lamp_test_item = {
	.name = "SINGLE LAMP TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &lamp_test_window, NULL } },
};

/****************** All Lamps Test ***********************/

void all_lamp_test_thread (void)
{
	for (;;)
	{
		lamp_all_on ();
		task_sleep (TIME_100MS * 7);
		lamp_all_off ();
		task_sleep (TIME_100MS * 3);
	}
}

void all_lamp_test_init (void)
{
	browser_init ();
	browser_item_number = NULL;
}

struct window_ops all_lamp_test_window = {
	INHERIT_FROM_BROWSER,
	.thread = all_lamp_test_thread,
	.init = all_lamp_test_init,
	.exit = lamp_all_off,
	.up = null_function,
	.down = null_function,
};

struct menu all_lamp_test_item = {
	.name = "ALL LAMPS TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &all_lamp_test_window, NULL } },
};

/*************** Lamp Row/Col Test ********************/

void lamp_row_col_test_up (void)
{
	lamp_all_off ();
	browser_up ();
}

void lamp_row_col_test_down (void)
{
	lamp_all_off ();
	browser_down ();
}

void lamp_row_col_test_item_number (U8 val)
{
	U8 lamp;
	U8 i;

	if (val < NUM_LAMP_COLS)
	{
		sprintf ("COL %d", val+1);
		lamp = MAKE_LAMP (val+1, 1);
		for (i=0; i < 8; i++)
			lamp_on (lamp++);
	}
	else
	{
		sprintf ("ROW %d", val-8+1);
		lamp = MAKE_LAMP (1, val-8+1);
		for (i=0; i < 8; i++)
		{
			lamp_on (lamp);
			lamp += 8;
		}
	}
}

void lamp_row_col_test_init (void)
{
	browser_init ();
	browser_max = (NUM_LAMP_COLS * 2) - 1;
	browser_item_number = lamp_row_col_test_item_number;
}

struct window_ops lamp_row_col_test_window = {
	INHERIT_FROM_BROWSER,
	.init = lamp_row_col_test_init,
	.up = lamp_row_col_test_up,
	.down = lamp_row_col_test_down,
	.exit = lamp_all_off,
};

struct menu lamp_row_col_test_item = {
	.name = "LAMP ROWS/COLS",
	.flags = M_ITEM,
	.var = { .subwindow = { &lamp_row_col_test_window, NULL } },
};

/***************** DIP Switch Test **********************/

void dipsw_test_draw (void)
{
	U8 sw;
	U8 dipsw = wpc_get_jumpers ();
	extern __common__ void locale_render (U8 locale);

	window_title ("DIP SWITCH TEST");

	locale_render ( (dipsw & 0x3C) >> 2 );
	print_row_center (&font_mono5, 10);

	for (sw = 0; sw < 8; sw++)
	{
#if (MACHINE_WPC95 == 1) || (MACHINE_PIC == 1)
		sprintf ("SW%d", sw+1);
#else
		sprintf ("W%d", sw+13);
#endif
		font_render_string_center (&font_var5, sw*16+8, 18, sprintf_buffer);


#if (MACHINE_WPC95 == 1) || (MACHINE_PIC == 1)
		sprintf (dipsw ? "ON" : "OFF");
#else
		sprintf (dipsw ? "OUT" : "IN");
#endif
		font_render_string_center (&font_var5, sw*16+8, 25, sprintf_buffer);

		dipsw >>= 1;
	}

	dmd_show_low ();
}


void dipsw_test_thread (void)
{
	for (;;)
	{
		/* Not likely, but in case DIP switches are changed
		on the fly, we keep redrawing. */
		task_sleep (TIME_500MS);
		dmd_alloc_low_clean ();
		dipsw_test_draw ();
	}
}


struct window_ops dipsw_test_window = {
	INHERIT_FROM_BROWSER,
	.draw = dipsw_test_draw,
	.up = null_function,
	.down = null_function,
	.thread = dipsw_test_thread,
};

struct menu dipsw_test_item = {
	.name = "DIP SWITCH TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &dipsw_test_window, NULL } },
};

/*************** Empty Balls Test ********************/

void empty_balls_test_init (void)
{
	if (counted_balls != 0)
	{
		device_t *dev;
		for (dev = device_entry (0); dev < device_entry (NUM_DEVICES); dev++)
		{
			device_request_empty (dev);
			task_sleep (TIME_500MS);
		}
	}
	callset_invoke (empty_balls_test);
}


void empty_balls_test_draw (void)
{
	window_title (counted_balls ? "EMPTYING BALLS..." : "EMPTY BALLS DONE");
	dmd_show_low ();
}

void empty_balls_test_thread (void)
{
	for (;;)
	{
		task_sleep_sec (1);
		dmd_alloc_low_clean ();
		empty_balls_test_draw ();
	}
}

struct window_ops empty_balls_test_window = {
	INHERIT_FROM_BROWSER,
	.init = empty_balls_test_init,
	.draw = empty_balls_test_draw,
	.up = null_function,
	.down = null_function,
	.thread = empty_balls_test_thread,
};

struct menu empty_balls_test_item = {
	.name = "EMPTY BALLS TEST",
	.flags = M_ITEM,
#if (NUM_DEVICES > 0)
	.var = { .subwindow = { &empty_balls_test_window, NULL } },
#endif
};


/******* Fliptronic Flipper Test ***********************/

#if MACHINE_FLIPTRONIC


void flipper_item_number (U8 val)
{
	sprintf ("F%d", val+1);
}


void flipper_test_init (void)
{
	browser_init ();
	browser_max = NUM_FLIPTRONIC_SWITCHES-1;
	browser_item_number = flipper_item_number;
}


void flipper_test_draw (void)
{
	browser_draw ();
}


struct window_ops flipper_test_window = {
	INHERIT_FROM_BROWSER,
	.init = flipper_test_init,
	.draw = flipper_test_draw,
};


struct menu flipper_test_item = {
	.name = "FLIPPER TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &flipper_test_window, NULL } },
};

#endif /* MACHINE_FLIPTRONIC */

/************   Display Test   **************************/


void display_test_init (void)
{
	browser_init ();
	browser_max = 100;
}


void display_test_draw (void)
{
	U8 n = menu_selection;

	if (n < 16)
	{
		wpc_dmd_set_low_page (menu_selection);
		dmd_clean_page_low ();
		dmd_draw_border (dmd_low_buffer);
		sprintf ("PAGE %d", n);
		print_row_center (&font_mono5, 16);
		dmd_show_low ();
		return;
	}
	else
		n -= 16;
}


struct window_ops display_test_window = {
	INHERIT_FROM_BROWSER,
	.init = display_test_init,
	.draw = display_test_draw,
};

struct menu display_test_item = {
	.name = "DISPLAY TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &display_test_window, NULL } },
};


/****************** TEST MENU **************************/

struct menu *test_menu_items[] = {
	&switch_edges_item,
	&switch_levels_item,
	&single_switches_item,
	&solenoid_test_item,
	&flasher_test_item,
	&gi_test_item,
	&sound_test_item,
	&lamp_test_item,
	&all_lamp_test_item,
	/* TODO : lamp_flasher_test_item */
	&display_test_item,
#if MACHINE_FLIPTRONIC
	&flipper_test_item,
#endif
	/* TODO : ordered_lamp_test_item */
	&lamp_row_col_test_item,
	&dipsw_test_item,
#ifdef MACHINE_TEST_MENU_ITEMS
	MACHINE_TEST_MENU_ITEMS
#endif
#ifdef MACHINE_TEST_ONLY
	&development_menu,
#endif
	&empty_balls_test_item,
	NULL,
};

struct menu test_menu = {
#ifdef MACHINE_TEST_ONLY
	.name = MACHINE_NAME,
#else
	.name = "TESTS",
#endif
	.flags = M_MENU | M_LETTER_PREFIX,
	.var = { .submenus = test_menu_items, },
};

struct menu *main_menu_items[] = {
	&bookkeeping_menu,
	&adjustments_menu,
	&test_menu,
	&utilities_menu,
	&printouts_menu,
	&development_menu,
	NULL,
};

struct menu main_menu = {
	.name = "MAIN MENU",
	.flags = M_MENU,
	.var = { .submenus = main_menu_items, },
};

/**********************************************************/

/* A window class for a text scroller. */

typedef void (*scroller_item) (void);
typedef scroller_item *scroller;

void scroller_init (void)
{
	scroller s = win_top->w_class.priv;

	win_top->w_class.scroller.funcs = s;
	win_top->w_class.scroller.offset = 0;
	win_top->w_class.scroller.size = 0;
	while (*s != NULL)
	{
		s++;
		win_top->w_class.scroller.size++;
	}
	win_top->w_class.scroller.size /= 2;
}

void scroller_draw (void)
{
	scroller s = win_top->w_class.scroller.funcs;
	U8 offset = win_top->w_class.scroller.offset;

	s[offset * 2] ();
	print_row_center (&font_mono5, 10);
	s[offset * 2 + 1] ();
	print_row_center (&font_mono5, 22);
	dmd_show_low ();
}

void scroller_up (void)
{
	if (++win_top->w_class.scroller.offset >= win_top->w_class.scroller.size)
		win_top->w_class.scroller.offset = 0;
	sound_send (SND_TEST_UP);
	task_kill_gid (GID_WINDOW_THREAD);
}

void scroller_down (void)
{
	if (--win_top->w_class.scroller.offset == 0xFF)
		win_top->w_class.scroller.offset = win_top->w_class.scroller.size-1;
	sound_send (SND_TEST_DOWN);
	task_kill_gid (GID_WINDOW_THREAD);
}

void scroller_thread (void)
{
	struct window_scroller *ws = &win_top->w_class.scroller;
	ws->offset = 0;

	for (;;)
	{
		task_sleep_sec (4);

		if (ws->offset < ws->size - 1)
			ws->offset++;
		else
		{
			callset_invoke (sw_enter);
			task_exit ();
		}

		dmd_alloc_low_clean ();
		scroller_draw ();
	}
}

#define INHERIT_FROM_SCROLLER \
	DEFAULT_WINDOW, \
	.init = scroller_init, \
	.draw = scroller_draw, \
	.up = scroller_up, \
	.down = scroller_down, \
	.thread = scroller_thread

struct window_ops scroller_window = {
	INHERIT_FROM_SCROLLER,
};

/**********************************************************/

/* A scroller instance for system information */

void sysinfo_machine_name (void) { sprintf (MACHINE_NAME); }

void sysinfo_machine_version (void) {
	extern __common__ void render_build_date (void);
	render_build_date ();
#ifdef DEBUGGER
	sprintf ("%E   D%s.%s", 
		C_STRING(MACHINE_MAJOR_VERSION), C_STRING(MACHINE_MINOR_VERSION));
#else
	sprintf ("%E   R%s.%s",
		C_STRING(MACHINE_MAJOR_VERSION), C_STRING(MACHINE_MINOR_VERSION));
#endif
}

void sysinfo_system_version (void) { 
#ifdef USER_TAG
	sprintf ("%s %s.%s", C_STRING(USER_TAG), 
		C_STRING(FREEWPC_MAJOR_VERSION), C_STRING(FREEWPC_MINOR_VERSION));
#else
	sprintf ("SYSTEM V%s.%s",
		C_STRING(FREEWPC_MAJOR_VERSION), C_STRING(FREEWPC_MINOR_VERSION));
#endif
}


void sysinfo_compiler_version (void) { 
	sprintf ("GCC6809 V%s", C_STRING(GCC_VERSION));
}

void sysinfo_stats1 (void) {
#ifdef CONFIG_DEBUG_STACK
	extern U8 task_largest_stack;
	extern U16 task_small_stacks;
	extern U16 task_medium_stacks;
	extern U16 task_large_stacks;
	sprintf ("STACK %d %ld %ld %ld", task_largest_stack, task_small_stacks, task_medium_stacks, task_large_stacks);
#else
	sprintf ("");
#endif
}

void sysinfo_stats2 (void) {
#ifdef TASKCOUNT
	extern U16 task_max_count;
	sprintf ("MAX TASKS %d", task_max_count);
#else
	sprintf ("");
#endif
}

scroller_item sysinfo_scroller[] = {
	sysinfo_machine_name, sysinfo_machine_version,
	sysinfo_system_version, sysinfo_compiler_version,
#if defined(CONFIG_DEBUG_STACK) || defined(TASKCOUNT)
	sysinfo_stats1, sysinfo_stats2,
#endif
	NULL
};

void sysinfo_enter (void)
{
	window_push (&menu_window, &main_menu);
}

void sysinfo_up (void)
{
	if (++win_top->w_class.scroller.offset >= win_top->w_class.scroller.size)
		sysinfo_enter ();
	else
		sound_send (SND_TEST_UP);
}

struct window_ops sysinfo_scroller_window = {
	INHERIT_FROM_SCROLLER,
	.enter = sysinfo_enter,
	.up = sysinfo_up,
};

/**********************************************************/

/* Compatibility functions for old test mode */

void test_init (void)
{
	window_init ();
	in_test = 0;
}

void test_up_button (void)
{
	U8 i;
	if (!win_top) return;

	window_call_op (win_top, up);
	window_redraw ();

	for (i=0; i < 16; i++)
		if (!switch_poll (SW_UP))
			return;
		else
			task_sleep (TIME_33MS);

	while (switch_poll (SW_UP))
	{
		window_call_op (win_top, up);
		window_redraw ();
		task_sleep (TIME_66MS);
	}
}

void test_down_button (void)
{
	U8 i;
	if (!win_top) return;

	window_call_op (win_top, down);
	window_redraw ();

	for (i=0; i < 16; i++)
		if (!switch_poll (SW_DOWN))
			return;
		else
			task_sleep (TIME_33MS);

	while (switch_poll (SW_DOWN))
	{
		window_call_op (win_top, down);
		window_redraw ();
		task_sleep (TIME_66MS);
	}
}

CALLSET_ENTRY (test_mode, sw_left_button)
{
	if (win_top)
	{
		window_call_op (win_top, left);
		window_redraw ();
	}
}


CALLSET_ENTRY (test_mode, sw_right_button)
{
	if (win_top)
	{
		window_call_op (win_top, right);
		window_redraw ();
	}
}


CALLSET_ENTRY (test_mode, sw_enter)
{
	if (!win_top)
	{
#ifdef MACHINE_TEST_ONLY
		window_push (&menu_window, &test_menu);
#else
		window_push (&sysinfo_scroller_window, &sysinfo_scroller);
#endif
	}
	else
	{
		window_call_op (win_top, enter);
		window_redraw ();
	}
}

CALLSET_ENTRY (test_mode, sw_escape)
{
	if (win_top)
	{
		window_call_op (win_top, escape);
		if (!win_top)
			return;
		else
			window_redraw ();
	}
}

CALLSET_ENTRY (test_mode, sw_start_button)
{
	if (win_top)
	{
		window_call_op (win_top, start);
		window_redraw ();
	}
}


CALLSET_ENTRY (test_mode, sw_buyin_button)
{
	if (win_top)
	{
		dmd_alloc_low_clean ();
		dmd_show_low ();
	}
}

