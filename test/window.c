/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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


struct window;

/** win_top always points to the current window, or NULL if
 * no window is open. */
struct window *win_top;

/* Equivalent to (win_top != NULL), but as a byte, this can
 * be tested with a single instruction.
 * TODO - these two variables could be overlapped into a union. */
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
	end_game ();
	sound_reset ();
	deff_stop_all ();
	leff_stop_all ();
	/* Ensure the lamp effects stop before resetting all lamps. */
	task_sleep (TIME_33MS);
	lamp_all_off ();
	/* Kill any other tasks still running */
	task_kill_all ();
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
	amode_start ();
	in_test = 0;
}

/** Starts the window's thread function, if it exists. */
void window_start_thread (void)
{
	if (win_top->ops->thread)
	{
		task_recreate_gid (GID_WINDOW_THREAD, win_top->ops->thread);
	}
}


/** Stops the window's thread function, if it exists. */
void window_stop_thread (void)
{
	task_kill_gid (GID_WINDOW_THREAD);
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
		window_call_op (win_top, suspend);
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
	window_call_op (win_top, draw);
	window_start_thread ();
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
		window_call_op (win_top, resume);
		window_call_op (win_top, draw);
		window_start_thread ();
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

	dmd_alloc_low_clean ();

	font_render_string_center (&font_mono5, 64, 2, m->name);

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
		menu_selection = 0;
}

void browser_down (void)
{
	sound_send (SND_TEST_DOWN);
	menu_selection--;
	if (menu_selection == 0xFF)
		menu_selection = browser_max;
}

#define INHERIT_FROM_BROWSER \
	DEFAULT_WINDOW, \
	.init = browser_init, \
	.draw = browser_draw, \
	.up = browser_up, \
	.down = browser_down \

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

void decimal_render (U8 val) { sprintf ("%d", val); }
void hex_render (U8 val) { sprintf ("%X", val); }
void on_off_render (U8 val) { sprintf (val ? "ON" : "OFF"); }
void yes_no_render (U8 val) { sprintf (val ? "YES" : "NO"); }
void clock_style_render (U8 val) { sprintf (val ? "24 HOUR" : "AM/PM"); }
void date_style_render (U8 val) { sprintf (val ? "D/M/Y" : "M/D/Y"); }
void lang_render (U8 val) { sprintf ("ENGLISH"); }
void replay_system_render (U8 val) { sprintf (val ? "AUTO" : "MANUAL"); }

void hs_reset_render (U8 val)
{ 
	if (val == 0)
		sprintf ("OFF");
	else
#if defined(__m6809__) && defined(__int16__)
		sprintf ("%d", val * 250);
#else
		sprintf ("%ld", val * 250UL);
#endif
}

void free_award_render (U8 val)
{
	switch (val)
	{
		case FREE_AWARD_OFF: sprintf ("OFF"); return;
		case FREE_AWARD_CREDIT: sprintf ("CREDIT"); return;
		case FREE_AWARD_EB: sprintf ("EXTRA BALL"); return;
		case FREE_AWARD_TICKET: sprintf ("TICKET"); return;
		case FREE_AWARD_POINTS: sprintf ("POINTS"); return;
	}
}

void game_restart_render (U8 val)
{
	switch (val)
	{
		case GAME_RESTART_ALWAYS: sprintf ("ALWAYS"); return;
		case GAME_RESTART_SLOW: sprintf ("SLOW"); return;
		case GAME_RESTART_NEVER: sprintf ("NEVER"); return;
	}
}

void percent_render (U8 val)
{
	if (val == 0)
		sprintf ("OFF");
	else
		sprintf ("%d%%", val);
}


struct adjustment_value integer_value = { 0, 0xFF, 1, decimal_render };
struct adjustment_value credit_count_value = { 0, 4, 1, decimal_render };
struct adjustment_value nonzero_integer_value = { 1, 0xFF, 1, decimal_render };
struct adjustment_value balls_per_game_value = { 1, 10, 1, decimal_render };

struct adjustment_value players_per_game_value = { 
#ifdef CONFIG_TIMED_GAME
	1, 1,
#else
	1, MAX_PLAYERS,
#endif
	1, decimal_render 
};

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
struct adjustment_value free_award_value = { 0, 3, 1, free_award_render };
struct adjustment_value percent_value = { 0, 100, 1, percent_render };

struct adjustment standard_adjustments[] = {
	{ "BALLS PER GAME", &balls_per_game_value, 3, &system_config.balls_per_game },
	{ "MAX PLAYERS", &players_per_game_value, 
#ifdef CONFIG_TIMED_GAME
		1, 
#else
		MAX_PLAYERS,
#endif
		&system_config.max_players },
	{ "TILT WARNINGS", &integer_value, 3, &system_config.tilt_warnings },
	{ "MAX E.B.", &max_eb_value, 5, &system_config.max_ebs },
	{ "MAX EB PER BIP", &max_eb_value, 4, &system_config.max_ebs_per_bip },
	{ "REPLAY SYSTEM", &replay_system_value, 0, &system_config.replay_system },
	{ "REPLAY AWARD", &free_award_value, 0, &system_config.replay_award },
	{ "SPECIAL AWARD", &free_award_value, 0, &system_config.special_award },
	{ "MATCH AWARD", &free_award_value, 0, &system_config.match_award },
	{ "MATCH FEATURE", &percent_value, OFF, &system_config.match_feature },
	{ "CUSTOM MESSAGE", &on_off_value, OFF, &system_config.custom_message },
	{ "LANGUAGE", &lang_value, 0, &system_config.language },
	{ "CLOCK STYLE", &clock_style_value, 0, &system_config.clock_style },
	{ "DATE STYLE", &date_style_value, 0, &system_config.date_style },
	{ "SHOW DATE/TIME", &yes_no_value, YES, &system_config.show_date_and_time },
	{ "ALLOW DIM ALLUM.", &yes_no_value, NO, &system_config.allow_dim_illum },
	{ "TOURNAMENT MODE", &yes_no_value, NO, &system_config.tournament_mode },
	{ "EURO. DIGIT SEP.", &yes_no_value, NO, &system_config.euro_digit_sep },
	{ "MIN. VOL. CONTROL", &integer_value, 8, &system_config.min_volume_control },
	{ "TICKET BOARD", &yes_no_value, NO, &system_config.ticket_board },
	{ "NO BONUS FLIPS", &yes_no_value, YES, &system_config.no_bonus_flips },
	{ "GAME RESTART", &game_restart_value, GAME_RESTART_SLOW, &system_config.game_restart },
	{ NULL, NULL, 0, NULL },
};


struct adjustment feature_adjustments[] = {
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
#ifdef MACHINE_FEATURE_ADJUSTMENTS
	MACHINE_FEATURE_ADJUSTMENTS
#endif
	{ NULL, NULL, 0, NULL },

};


struct adjustment pricing_adjustments[] = {
	{ "UNITS PER CREDIT", &nonzero_integer_value, 2, &price_config.units_per_credit },
	{ "UNITS PER BONUS", &integer_value, 0, &price_config.units_per_bonus },
	{ "BONUS CREDITS", &integer_value, 0, &price_config.bonus_credits },
	{ "LEFT SLOT VALUE", &nonzero_integer_value, 1, &price_config.slot_values[0] },
	{ "CENTER SLOT VALUE", &nonzero_integer_value, 4, &price_config.slot_values[1] },
	{ "RIGHT SLOT VALUE", &nonzero_integer_value, 1, &price_config.slot_values[2] },
	{ "4TH SLOT VALUE", &nonzero_integer_value, 1, &price_config.slot_values[3] },
	{ "MAXIMUM CREDITS", &nonzero_integer_value, 10, &price_config.max_credits },
#ifdef FREE_ONLY
	{ "", &yes_no_value, YES, &price_config.free_play },
#else
	{ "FREE PLAY", &yes_no_value, NO, &price_config.free_play },
#endif
	{ "HIDE COIN AUDITS", &yes_no_value, NO, NULL },
	{ "1-COIN BUY-IN", &yes_no_value, NO, &price_config.one_coin_buyin },
	{ "COIN METER UNITS", &integer_value, 0, NULL },
	{ "DOLLAR BILL SLOT", &yes_no_value, NO, NULL },
	{ "MIN. COIN MSEC.", &nonzero_integer_value, 50, &price_config.min_coin_msec },
	{ "SLAMTILT PENALTY", &yes_no_value, YES, &price_config.slamtilt_penalty },
	{ "ALLOW HUNDREDTHS", &yes_no_value, NO, NULL },
	{ "CREDIT FRACTION", &on_off_value, OFF, NULL },
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
	{ "COLUMN WIDTH", &integer_value, 72, NULL },
	{ "LINES PER PAGE", &integer_value, 60, NULL },
	{ "PAUSE EVERY PAGE", &yes_no_value, NO, NULL },
	{ "PRINTER TYPE", &integer_value, 0, NULL },
	{ "SERIAL BAUD RATE", &integer_value, 0, NULL },
	{ "SERIAL D.T.R.", &integer_value, 0, NULL },
	{ "AUTO PRINTOUT", &on_off_value, OFF, NULL },
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

	dmd_alloc_low_clean ();

	sprintf ("%d. %s", menu_selection+1, ad->name);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);

	if (ad->nvram == NULL)
	{
		font_render_string_center (&font_mono5, 32, 20, "N/A");
	}
	else
	{
		if (browser_action == ADJ_EDITING)
			ad->values->render (adj_edit_value);
		else if (ad->nvram)
			ad->values->render (adj_edit_value = *(ad->nvram));

		font_render_string_center (&font_mono5, 32, 21, sprintf_buffer);

		if (adj_edit_value == ad->factory_default)
			font_render_string_center (&font_mono5, 96, 21, "DEFAULT");
	}

	if (browser_action == ADJ_EDITING)
		dmd_invert_page (dmd_low_buffer);

	dmd_show_low ();
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

#if 0 /* not working */
	while (!*browser_adjs[menu_selection].name);
		browser_up ();
#endif
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
		sound_send (SND_TEST_ABORT);
		browser_action = ADJ_BROWSING;
	}
	else if (browser_action == ADJ_CONFIRMING)
	{
		browser_action = ADJ_EDITING;
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
	.down = adj_browser_down

struct window_ops adj_browser_window = {
	INHERIT_FROM_ADJ_BROWSER,
};

/*****************************************************/

void percentage_of_games_audit (audit_t val)
{
	if (system_audits.total_plays == 0)
	{
		sprintf ("0%");
		return;
	}

#ifndef __m6809__
	sprintf ("%d%%", 100 * val / system_audits.total_plays);
#else
	/* TODO */
#endif
}


void integer_audit (audit_t val) 
{ 
	sprintf ("%ld", val);
}


void secs_audit (audit_t val)
{
	U8 mins = 0;
	while (val > 60)
	{
		val -= 60;
		mins++;
	}
	sprintf ("%d:%02d", mins, (U8)val);
}


void currency_audit (audit_t val)
{
	sprintf ("%ld.%ld", val / 4, (val % 4) * 25);
}


void total_earnings_audit (audit_t val __attribute__((unused)))
{
	audit_t total_coins = 0;
	U8 i;
	for (i=0; i < 4; i++)
		total_coins += system_audits.coins_added[i];
	currency_audit (total_coins);
}


struct audit *browser_audits;


audit_t default_audit_value;


struct audit main_audits[] = {
	{ "EARNINGS", total_earnings_audit, &default_audit_value },
	{ "FREEPLAY PERCENT", },
	{ "AVG. BALL TIME", },
	{ "TIME PER CREDIT", secs_audit, &default_audit_value },
	{ "TOTAL PLAYS", },
	{ "REPLAY AWARDS", integer_audit, &system_audits.replays },
	{ "PERCENT REPLAYS", percentage_of_games_audit, &system_audits.replays },
	{ "EXTRA BALLS", integer_audit, &system_audits.extra_balls_awarded },
	{ "PERCENT EX. BALL", percentage_of_games_audit, &system_audits.extra_balls_awarded },
	{ NULL, NULL, NULL },
};

struct audit earnings_audits[] = {
	{ "EARNINGS", total_earnings_audit, &default_audit_value },
	{ "LEFT SLOT", integer_audit, &system_audits.coins_added[0] },
	{ "CENTER SLOT", integer_audit, &system_audits.coins_added[1] },
	{ "RIGHT SLOT", integer_audit, &system_audits.coins_added[2] },
	{ "4TH SLOT SLOT", integer_audit, &system_audits.coins_added[3] },
	{ "PAID CREDITS", integer_audit, &system_audits.paid_credits },
	{ "SERVICE CREDITS", integer_audit, &system_audits.service_credits },
	{ NULL, NULL, NULL },
};


struct audit standard_audits[] = {
	{ "GAMES STARTED", &integer_audit, &system_audits.games_started },
	{ "TOTAL PLAYS", },
	{ "TOTAL FREE PLAY", },
	{ "FREEPLAY PERCENT", },
	{ "TILTS", &integer_audit, &system_audits.tilts },
	{ "LEFT DRAINS", &integer_audit, &system_audits.left_drains },
	{ "RIGHT DRAINS", &integer_audit, &system_audits.right_drains },
	{ "CENTER DRAINS", &integer_audit, &system_audits.center_drains },
	{ "POWER UPS", &integer_audit, &system_audits.power_ups },
	{ "SLAM TILTS", &integer_audit, &system_audits.slam_tilts },
	{ "PLUMB BOB TILTS", &integer_audit, &system_audits.plumb_bob_tilts },
	{ "FATAL ERRORS", &integer_audit, &system_audits.fatal_errors },
	{ "NON-FATAL ERRORS", &integer_audit, &system_audits.non_fatal_errors },
	{ "LEFT FLIPPER", &integer_audit, &system_audits.left_flippers },
	{ "RIGHT FLIPPER", &integer_audit, &system_audits.right_flippers },
	{ NULL, NULL, NULL },
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

	dmd_alloc_low_clean ();

	sprintf ("%d. %s", menu_selection+1, aud->name);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);

	if (aud->nvram && aud->render)
	{
		aud->render (*(aud->nvram));
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

void (*confirm_banner) (void);
void (*confirm_action) (void);
U8 confirm_timer;

void confirm_init (void)
{
	confirm_banner = confirm_action = null_function;
	confirm_timer = 7;
}

void confirm_draw (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%d", confirm_timer);
	font_render_string_left (&font_mono5, 2, 2, sprintf_buffer);
	font_render_string_left (&font_mono5, 120, 2, sprintf_buffer);
	font_render_string_center (&font_mono5, 64, 14, "ENTER TO SAVE");
	font_render_string_center (&font_mono5, 64, 20, "ESCAPE TO CANCEL");
	dmd_show_low ();
}

void confirm_enter (void)
{
	dbprintf ("Confirming adjustment change.\n");
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


void menu_draw (void)
{
	struct menu *m = win_top->w_class.menu.self;
	struct menu **subm;
	U8 *sel = &win_top->w_class.menu.selected;

	dmd_alloc_low_clean ();

	font_render_string (&font_mono5, 8, 4, m->name);

	subm = m->var.submenus;
	if (subm != NULL)
	{
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
		font_render_string (&font_mono5, 8, 14, "ERROR... NO SUBMENUS");
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
		window_push (&menu_window, m);
	}
	else
	{
		struct window_ops *ops = m->var.subwindow.ops;
		if (ops != NULL)
		{
			window_push (ops, m->var.subwindow.priv ? m->var.subwindow.priv : m);
		}
		else
		{
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

void menu_start (void)
{
}

struct window_ops menu_window = {
	DEFAULT_WINDOW,
	.init = menu_init,
	.draw = menu_draw,
	.enter = menu_enter,
	.up = menu_up,
	.down = menu_down,
	.start = menu_start
};

/**********************************************************/

/* The test mode menus */

/*******************  Font Test  ************************/

U8 font_test_offset;

const font_t *font_test_lookup (void)
{
	switch (menu_selection)
	{
#if (MAX_FONTS > 0)
		case FON_MONO5: default: return &font_mono5;
		case FON_MONO9: return &font_mono9;
		case FON_FIXED10: return &font_fixed10;
		case FON_FIXED6: return &font_fixed6;
		case FON_LUCIDA9: return &font_lucida9;
		case FON_TERM6: return &font_term6;
		case FON_TIMES8: return &font_times8;
		case FON_TIMES10: return &font_times10; /* this and helv8 are the same? */
		case FON_HELV8: return &font_helv8;
		case FON_SCHU: return &font_schu;
		case FON_MISCTYPE: return &font_misctype; /* broken! */
		case FON_UTOPIA: return &font_utopia; /* broken! */
		case FON_FIXED12: return &font_fixed12; /* broken! */
		case FON_VAR5: return &font_var5;
		case FON_CU17: return &font_cu17;
#endif
	}
}

char font_test_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";


void font_test_init (void)
{
	browser_init ();
	browser_max = MAX_FONTS-1; /* set to highest valid font number */
	font_test_offset = 0;
}

void font_test_draw (void)
{
	const font_t *font = font_test_lookup ();

	dmd_alloc_low_clean ();

	if ((font->glyphs['A'] == NULL)
		&& (font_test_offset < 26))
	{
		font_test_offset = 0;
	}

	sprintf ("FONT %d", menu_selection+1);
	font_render_string_center (&font_mono5, 64, 5, sprintf_buffer);
	dmd_draw_horiz_line (dmd_low_buffer, 9);

	sprintf ("%8s", font_test_alphabet + font_test_offset);
	font_render_string_center (font, 64, 20, sprintf_buffer);
	dmd_show_low ();
}

void font_test_left (void)
{
	if (font_test_offset > 0)
		font_test_offset--;
	sound_send (SND_TEST_CHANGE);
}

void font_test_right (void)
{
	font_test_offset++;
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

/* TODO : escaping out of a running deff in test mode
will cause a crash */

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

static bool leff_test_running (U8 id)
{
	return (leff_get_active () == id) ||
		task_find_gid_data (GID_SHARED_LEFF, L_PRIV_ID, id);
}

struct deff_leff_ops dev_deff_ops = {
	.start = deff_start,
	.stop = deff_stop,
	.is_running = deff_test_running,
};

struct deff_leff_ops dev_leff_ops = {
	.start = leff_start,
	.stop = leff_stop,
	.is_running = leff_test_running,
};

void deff_leff_thread (void)
{
	bool is_active = deff_leff_test_ops->is_running (menu_selection);
	deff_leff_last_active = !is_active;
	browser_draw ();

	for (;;)
	{
		if (is_active != deff_leff_last_active)
		{
			if (deff_leff_test_ops == &dev_deff_ops) 
			{
				if (is_active == FALSE)
				{
					browser_draw ();
					sprintf_far_string (names_of_deffs + menu_selection);
					font_render_string_center (&font_var5, 64, 12, sprintf_buffer);
					browser_print_operation ("STOPPED");
					sound_reset ();
				}
			}
			else
			{
				browser_draw ();
				sprintf_far_string (names_of_leffs + menu_selection);
				font_render_string_center (&font_var5, 64, 12, sprintf_buffer);
				if (is_active == TRUE)
					browser_print_operation ("RUNNING");
				else
					browser_print_operation ("STOPPED");
			}
		}
		deff_leff_last_active = is_active;
		task_sleep (TIME_100MS * 4);
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

	for (;;)
	{
		dn = random_scaled (MAX_DEFFS);
		start_stop_flag = random_scaled (2);
		delay = random_scaled (TIME_200MS);
		delay += TIME_33MS;

		dbprintf ("Deff %d, start_stop %d, delay %d\n",
			dn, start_stop_flag, delay);

		if (start_stop_flag)
		{
			deff_start (dn);
			sound_send (SND_TEST_UP);
		}
		else
		{
			deff_stop (dn);
			sound_send (SND_TEST_DOWN);
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
	browser_max = BM_LAST-1;
}

void symbol_test_draw (void)
{
	union dmd_coordinate coord;
	
	browser_draw ();
	coord.x = 96;
	coord.y = 20;
	bitmap_draw (coord, menu_selection+1);
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

U8 lampset_update_mode;
U8 lampset_update_speed;

void lampset_init (void)
{
	browser_init ();
	browser_min = 1;
	browser_max = MAX_LAMPSET-1;
	browser_item_number = browser_decimal_item_number;
	lampset_update_mode = 0;
	lampset_update_speed = TIME_16MS;
}


void lampset_draw (void)
{
	browser_draw ();
	sprintf_far_string (names_of_lampsets + menu_selection);
	font_render_string_center (&font_var5, 64, 12, sprintf_buffer);
	sprintf ("SPEED %d", lampset_update_speed);
	font_render_string_center (&font_var5, 50, 21, sprintf_buffer);
	sprintf ("MODE %d", lampset_update_mode);
	font_render_string_center (&font_var5, 92, 21, sprintf_buffer);

	/* Restart the update thread so that the old lamps are
	cleared before the new effect is started */
	window_start_thread ();
}


void lampset_update (void)
{
	lamp_all_off ();
	for (;;)
	{
		lampset_set_apply_delay (lampset_update_speed);
		switch (lampset_update_mode)
		{
			case 0: 
				lamp_all_off ();
				lampset_apply_on (menu_selection); 
				break;
			case 1: lampset_apply_toggle (menu_selection); break;
			case 2: lampset_step_increment (menu_selection); break;
			case 3: lampset_step_decrement (menu_selection); break;
			case 4: lampset_build_increment (menu_selection); break;
			case 5: lampset_build_decrement (menu_selection); break;
		}
		task_sleep (TIME_200MS);
	}
}

void lampset_test_slower (void)
{
	if (lampset_update_speed > 1)
		lampset_update_speed--;
}

void lampset_test_faster (void)
{
	lampset_update_speed++;
}

void lampset_test_mode_change (void)
{
	lampset_update_mode++;
	if (lampset_update_mode == 6)
		lampset_update_mode = 0;
}

struct window_ops dev_lampset_window = {
	INHERIT_FROM_BROWSER,
	.init = lampset_init,
	.exit = lamp_all_off,
	.draw = lampset_draw,
	.thread = lampset_update,
	.left = lampset_test_slower,
	.right = lampset_test_faster,
	.enter = lampset_test_mode_change,
};

struct menu dev_lampset_test_item = {
	.name = "LAMPSETS",
	.flags = M_ITEM,
	.var = { .subwindow = { &dev_lampset_window, NULL } },
};

/*********** Ball Devices **********************/


void dev_balldev_test_init (void)
{
	browser_init ();
	browser_max = NUM_DEVICES-1;
}

void dev_balldev_test_draw (void)
{
	extern U8 counted_balls, missing_balls;
	device_t *dev;
	char *s;

	dev = &device_table[menu_selection];

	dmd_alloc_low_clean ();

	if ((dev == NULL) || (dev->props == NULL))
	{
		sprintf ("DEV %d. NOT INSTALLED", menu_selection);
		font_render_string_center (&font_mono5, 64, 3, sprintf_buffer);
	}
	else
	{
		sprintf ("DEV %d. %s", menu_selection, dev->props->name);
		font_render_string_center (&font_var5, 64, 3, sprintf_buffer);
	
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
				dev_balldev_test_draw ();
			}
			else if (i == 7)
				dev_balldev_test_draw ();

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
	&trans_scroll_right, /* TODO : broken */
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

void dev_random_test_enter (void)
{
	U16 i;
	static U8 rowcount[32];

	for (i=0; i < 32; i++)
		rowcount[i] = 0;

	dmd_alloc_low_clean ();
	dmd_show_low ();

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
		task_sleep (TIME_66MS);
	}

	dmd_invert_page (dmd_low_buffer);
	task_sleep (TIME_200MS);
	dmd_invert_page (dmd_low_buffer);
}

struct window_ops dev_random_test_window = {
	DEFAULT_WINDOW,
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

void dev_frametest_draw (void)
{
	const char *data = 0x4001 + menu_selection * DMD_PAGE_SIZE;
	if (switch_poll_logical (SW_ENTER))
	{
		dmd_alloc_low_high ();
		dmd_draw_image2 (data);
		dmd_show2 ();
	}
	else
	{
		dmd_alloc_low ();
		dmd_draw_image (data);
		dmd_show_low ();
	}
}

struct window_ops dev_frametest_window = {
	INHERIT_FROM_BROWSER,
	.draw = dev_frametest_draw,
};

struct menu dev_frametest_item = {
	.name = "DMD FRAME TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &dev_frametest_window, NULL } },
};

/**********************************************************************/

#define SCHED_TEST_DURATION TIME_2S
#define SCHED_TEST_WORKERS  16
#define SCHED_LOCAL_COUNT   16

U16 sched_test_count;

void sched_test_task (void)
{
	volatile U8 local_data[SCHED_LOCAL_COUNT];

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
}


void sched_test_draw (void)
{
	dmd_alloc_low_clean ();
	sprintf ("SCHED COUNT %ld", sched_test_count);
	font_render_string_left (&font_var5, 2, 3, sprintf_buffer);
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

const score_t score_test_increment = { 0x00, 0x01, 0x23, 0x45, 0x60 };

void score_test_init (void)
{
	score_t *s;
	for (s = &scores[0]; s < &scores[4]; s++)
	{
		score_zero (s);
		score_add (s, score_test_increment, sizeof (score_t));
	}
	num_players = 1;
	player_up = 1;
}

void score_test_draw (void)
{
	dmd_alloc_low_clean ();
	scores_draw ();
	dmd_show_low ();
}

void score_test_up (void)
{
	if (num_players == player_up)
	{
		num_players++;
		player_up = 1;
		if (num_players > 2)
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
	if (player_up == 0)
	{
		num_players--;
		if (num_players == 0)
		{
			num_players = 2;
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

struct menu *dev_menu_items[] = {
	&dev_font_test_item,
	&dev_deff_test_item,
	&dev_leff_test_item,
	&dev_lampset_test_item,
	&dev_balldev_test_item,
	&dev_random_test_item,
	&dev_trans_test_item,
	&dev_force_error_item,
	&dev_frametest_item,
	&dev_deff_stress_test_item,
	&symbol_test_item,
	&sched_test_item,
	&score_test_item,
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

void set_time_window_confirm (void)
{
	rtc_factory_reset ();
	confirm_enter ();
}

struct window_ops set_time_window = {
	INHERIT_FROM_CONFIRM_WINDOW,
	.enter = set_time_window_confirm,
};

struct menu set_time_item = {
	.name = "SET TIME/DATE",
	.flags = M_ITEM,
	.var = { .subwindow = { &set_time_window, NULL } },
};

/**********************************************************************/

struct menu custom_message_item = {
	.name = "CUSTOM MESSAGE",
	.flags = M_ITEM,
};

struct menu set_gameid_item = {
	.name = "SET GAME I.D.",
	.flags = M_ITEM,
};

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
	dmd_alloc_low_clean ();
	dmd_show_low ();
}


void burnin_test_thread (void)
{
	extern void all_lamp_test_thread (void);

	task_create_anon (all_lamp_test_thread);
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

struct preset_component
{
	struct adjustment *info;
	U8 *nvram;
	U8 value;
};

struct preset
{
	char *name;
	struct preset_component *comps;
};

struct preset_component preset_3ball_comps[] = {
	{ standard_adjustments, &system_config.balls_per_game, 3 },
	{ NULL, 0 },
};
struct preset preset_3ball = { .name = "3-BALL", preset_3ball_comps };

struct preset_component preset_5ball_comps[] = {
	{ standard_adjustments, &system_config.balls_per_game, 5 },
	{ NULL, 0 },
};
struct preset preset_5ball = { .name = "5-BALL", preset_5ball_comps };

struct preset_component preset_tournament_comps[] = {
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
	/* TODO : once extra ball buyin is implemented, disable it here */
	{ NULL, 0 },
};
struct preset preset_tournament = { .name = "TOURNAMENT", preset_tournament_comps };


struct preset_component preset_show_comps[] = {
	{ pricing_adjustments, &price_config.free_play, YES },
	{ standard_adjustments, &system_config.replay_award, FREE_AWARD_OFF },
	{ standard_adjustments, &system_config.special_award, FREE_AWARD_OFF },
	{ standard_adjustments, &system_config.match_feature, OFF },
	{ pricing_adjustments, &price_config.one_coin_buyin, OFF },
	{ NULL, 0 },
};
struct preset preset_show = { .name = "SHOW", preset_show_comps };


struct preset_component preset_timed_comps[] = {
	{ standard_adjustments, &system_config.max_players, 1 },
	{ NULL, 0 },
};
struct preset preset_timed_game = { .name = "TIMED GAME", preset_timed_comps };

struct preset *preset_table[] = {
	&preset_3ball,
	&preset_5ball,
	&preset_tournament,
	&preset_show,
	&preset_timed_game,
};


void presets_init (void)
{
	browser_init ();
	browser_max = (sizeof (preset_table) / sizeof (struct preset *)) - 1;
}

void presets_draw (void)
{
	struct preset *pre = preset_table[menu_selection];
	struct preset_component *comps = pre->comps;

	dmd_alloc_low_clean ();

	sprintf ("%d. INSTALL %s", menu_selection+1, pre->name);
	font_render_string_center (&font_mono5, 64, 5, sprintf_buffer);

	/* Is it installed now? */	
	while (comps->nvram != NULL)
	{
		if (*(comps->nvram) != comps->value)
		{
			font_render_string_center (&font_mono5, 64, 13, "NOT INSTALLED");
			break;
		}

		comps++;
		if (comps->nvram == NULL)
			font_render_string_center (&font_mono5, 64, 13, "INSTALLED");
	}

	dmd_show_low ();
}


void presets_enter (void)
{
	struct preset *pre = preset_table[menu_selection];
	struct preset_component *comps = pre->comps;

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 8, "INSTALLING PRESET");
	font_render_string_center (&font_mono5, 64, 16, pre->name);
	dmd_show_low ();
	task_sleep_sec (2);
	sound_send (SND_TEST_CONFIRM);

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


void presets_start (void)
{
	struct preset *pre = preset_table[menu_selection];
	struct preset_component *comps = pre->comps;
	struct adjustment *info;
	union dmd_coordinate coord;

	dmd_alloc_low_clean ();
	dmd_sched_transition (&trans_scroll_left);
	font_render_string_center (&font_mono5, 64, 5, pre->name);
	dmd_draw_horiz_line (dmd_low_buffer, 11);
	sound_send (SND_TEST_SCROLL);
	dmd_show_low ();
	task_sleep (TIME_1S + TIME_500MS);

	while (comps->nvram != NULL)
	{
		info = comps->info;
		dmd_alloc_low_clean ();
		font_render_string_center (&font_mono5, 64, 5, pre->name);
		dmd_draw_horiz_line (dmd_low_buffer, 11);
		if (info == NULL)
		{
			sprintf ("SET %p TO %02X", comps->nvram, comps->value);
			font_render_string_center (&font_mono5, 64, 16, sprintf_buffer);
		}
		else
		{
			while (info != NULL)
			{
				if (info->nvram == comps->nvram)
				{
					font_render_string_center (&font_mono5, 64, 16, info->name);
					(*info->values->render) (comps->value);
					font_render_string_center (&font_mono5, 64, 24, sprintf_buffer);
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
					break;
				}
				info++;
			}
		}
		dmd_show_low ();
		sound_send (SND_TEST_CONFIRM);
		task_sleep (TIME_1S + TIME_500MS);
		comps++;
	}
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
	extern void do_reset (void);

	dmd_alloc_low_clean ();
	dmd_show_low();
	task_sleep_sec (1);

	wpc_nvram_get ();
	freewpc_accepted[0] = 0;
	freewpc_accepted[1] = 0;
	freewpc_accepted[2] = 0;
	wpc_nvram_put ();

	do_reset ();
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
	&custom_message_item,
	&set_gameid_item,
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

void switch_test_add_queue (U8 sw)
{
}

void switch_matrix_draw (void)
{
	U8 row, col;

	for (row=0; row < 8; row++)
	{
		for (col=0; col < 8; col++)
		{
			U8 sw = MAKE_SWITCH (col+1,row+1);
#if 0 /* whether or not it is an opto isn't important now */
			bool opto_p = switch_is_opto (sw);
#endif
			bool state_p = switch_poll (sw);
			register U8 *dmd = dmd_low_buffer +
				((U16)row << 6) + (col >> 1);
			U8 mask = (col & 1) ? 0x0E : 0xE0;
			if (state_p)
			{
				dmd[0 * DMD_BYTE_WIDTH] |= mask;
				dmd[1 * DMD_BYTE_WIDTH] |= mask & ~0x44;
				dmd[2 * DMD_BYTE_WIDTH] |= mask;
			}
			else
			{
				dmd[0 * DMD_BYTE_WIDTH] &= ~mask;
				dmd[1 * DMD_BYTE_WIDTH] |= mask & 0x44;
				dmd[2 * DMD_BYTE_WIDTH] &= ~mask;
			}
		}
	}
}

void switch_edges_draw (void)
{
	dmd_alloc_low_clean ();
	switch_matrix_draw ();
	font_render_string_center (&font_mono5, 80, 4, "SWITCH EDGES");
	dmd_show_low ();
}

void switch_edges_thread (void)
{
	for (;;) {
		task_sleep (TIME_100MS);
		switch_matrix_draw ();
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
	dmd_alloc_low_clean ();
	switch_matrix_draw ();
	font_render_string_center (&font_mono5, 80, 4, "SWITCH LEVELS");
	dmd_show_low ();
}

struct window_ops switch_levels_window = {
	INHERIT_FROM_BROWSER,
	.draw = switch_levels_draw,
	.thread = switch_edges_thread,
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
	const char *state;
	const char *opto;

	dmd_alloc_low_clean ();
	switch_matrix_draw ();
	font_render_string_center (&font_mono5, 80, 4, "SINGLE SWITCH");

	(*browser_item_number) (menu_selection);
	font_render_string (&font_mono5, 36, 12, sprintf_buffer);

	sprintf_far_string (names_of_switches + menu_selection);
	font_render_string (&font_var5, 50, 12, sprintf_buffer);

	state = switch_poll (sel) ? "CLOSED" : "OPEN";
	opto = switch_is_opto (sel) ? "OPTO " : "";
	sprintf ("%s%s", opto, state);
	font_render_string_center (&font_mono5, 80, 20, sprintf_buffer);
	
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
		sound_reset ();
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

/****************** Solenoid Test **************************/

/* The browser action stores the pulse width */

void solenoid_test_init (void)
{
	browser_init ();
	browser_action = TIME_66MS;
#ifdef NUM_POWER_DRIVES
	browser_max = NUM_POWER_DRIVES;
#endif
}

void solenoid_test_draw (void)
{
	char *s;

	browser_draw ();
	switch (browser_action)
	{
		default: s = "UNKNOWN PULSE"; break;
		case TIME_16MS: s = "VERY SOFT PULSE"; break;
		case TIME_33MS: s = "SOFT PULSE"; break;
		case TIME_66MS: s = "NORMAL PULSE"; break;
		case TIME_100MS: s = "HARD PULSE"; break;
		case TIME_133MS: s = "VERY HARD PULSE"; break;
	}
	font_render_string_center (&font_mono5, 64, 12, s);
	sprintf_far_string (names_of_drives + menu_selection);
	browser_print_operation (sprintf_buffer);
}

void solenoid_test_enter (void)
{
	U8 sel = win_top->w_class.menu.selected;
	task_sleep (TIME_100MS * 3);
	sol_on (sel);
	task_sleep (browser_action);
	sol_off (sel);
	task_sleep (TIME_100MS);
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

struct window_ops solenoid_test_window = {
	INHERIT_FROM_BROWSER,
	.init = solenoid_test_init,
	.draw = solenoid_test_draw,
	.enter = solenoid_test_enter,
	.left = solenoid_test_left,
	.right = solenoid_test_right,
};

struct menu solenoid_test_item = {
	.name = "SOLENOID TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &solenoid_test_window, NULL } },
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

const char *gi_test_names[] = {
	"ALL OFF",
	"STRING 1",
	"STRING 2",
	"STRING 3",
	"STRING 4",
	"STRING 5",
	"ALL ON",
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
	browser_print_operation (gi_test_names[menu_selection]);
	sprintf ("BRIGHTNESS %d", gi_test_brightness);
	font_render_string_center (&font_mono5, 64, 29, sprintf_buffer);

	triac_leff_disable (TRIAC_GI_MASK);
	triac_set_brightness (gi_test_values[menu_selection], gi_test_brightness);
}

void gi_test_right (void)
{
	if (gi_test_brightness < 8)
		gi_test_brightness++;
}

void gi_test_left (void)
{
	if (gi_test_brightness > 1)
		gi_test_brightness--;
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
	.name = "LAMP TEST",
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

void dipsw_render_single (U8 sw, U8 state)
{
	sprintf ("SW%d %s", sw, state ? "OFF" : "ON");
}

void dipsw_test_draw (void)
{
	U8 sw;
	U8 dipsw = wpc_get_jumpers ();

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 3, "DIP SWITCHES");

#ifndef GCC4
	for (sw = 0; sw < 8; sw++)
	{
		dipsw_render_single (sw+1, dipsw & 0x1);
		font_render_string (&font_mono5, 
			((sw <= 3) ? 8 : 72), 
			((sw % 4) * 6 + 9), 
			sprintf_buffer);
		dipsw >>= 1;
	}
#endif

	dmd_show_low ();
}

struct window_ops dipsw_test_window = {
	INHERIT_FROM_BROWSER,
	.draw = dipsw_test_draw,
	.up = null_function,
	.down = null_function,
};

struct menu dipsw_test_item = {
	.name = "DIP SWITCH TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &dipsw_test_window, NULL } },
};

/*************** Empty Balls Test ********************/

void empty_balls_test_init (void)
{
	device_t *dev;
	U8 count;

	for (dev = device_entry (0); dev < device_entry (NUM_DEVICES); dev++)
		device_request_empty (dev);
#ifdef MACHINE_TZ
	for (count = 3; count > 0; --count)
		gumball_release ();
#endif
}

void empty_balls_test_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 3, "EMPTYING BALLS...");
	dmd_show_low ();
}

struct window_ops empty_balls_test_window = {
	INHERIT_FROM_BROWSER,
	.init = empty_balls_test_init,
	.draw = empty_balls_test_draw,
	.up = null_function,
	.down = null_function,
};

struct menu empty_balls_test_item = {
	.name = "EMPTY BALLS TEST",
	.flags = M_ITEM,
#if (NUM_DEVICES > 0)
	.var = { .subwindow = { &empty_balls_test_window, NULL } },
#endif
};

/****************** WPC ASIC Tests ***********************/

#define ASIC_REG_BASE 0x3FD0
#define ASIC_REG_COUNT 0x30

void asic_register_item_number (U8 val)
{
	sprintf ("%04lX", ASIC_REG_BASE + val);
}

void asic_register_init (void)
{
	browser_item_number = asic_register_item_number;
	browser_max = ASIC_REG_COUNT-1;
}

void asic_register_draw (void)
{
	browser_draw ();
	sprintf ("%02X", wpc_asic_read (ASIC_REG_BASE + menu_selection));
	browser_print_operation (sprintf_buffer);
}

struct window_ops asic_register_window = {
	INHERIT_FROM_BROWSER,
	.init = asic_register_init,
	.draw = asic_register_draw,
};

struct menu asic_register_item = {
	.name = "ASIC TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &asic_register_window, NULL } },
};

struct menu flipper_test_item = {
	.name = "FLIPPER TEST",
	.flags = M_ITEM,
};

struct menu display_test_item = {
	.name = "DISPLAY TEST",
	.flags = M_ITEM,
};

struct menu debugger_test_item = {
	.name = "DEBUGGER TEST",
	.flags = M_ITEM,
};


/****************** TEST MENU **************************/

struct menu *test_menu_items[] = {
	&switch_edges_item,
	&switch_levels_item,
	&single_switches_item,
	&display_test_item,
	&solenoid_test_item,
	&gi_test_item,
	&sound_test_item,
	&lamp_test_item,
	&all_lamp_test_item,
	&lamp_row_col_test_item,
	&dipsw_test_item,
	&flipper_test_item,
	&asic_register_item,
	&debugger_test_item,
	&empty_balls_test_item,
#ifdef MACHINE_TEST_MENU_ITEMS
	MACHINE_TEST_MENU_ITEMS
#endif
#ifdef MACHINE_TEST_ONLY
	&development_menu,
#endif
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

	dmd_alloc_low_clean ();
	s[offset * 2] ();
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	s[offset * 2 + 1] ();
	font_render_string_center (&font_mono5, 64, 22, sprintf_buffer);
	dmd_show_low ();
}

void scroller_up (void)
{
	if (++win_top->w_class.scroller.offset >= win_top->w_class.scroller.size)
		win_top->w_class.scroller.offset = 0;
	sound_send (SND_TEST_UP);
}

void scroller_down (void)
{
	if (--win_top->w_class.scroller.offset == 0xFF)
		win_top->w_class.scroller.offset = win_top->w_class.scroller.size-1;
	sound_send (SND_TEST_DOWN);
}

void scroller_resume (void)
{
	win_top->w_class.scroller.offset = 0;
}

#define INHERIT_FROM_SCROLLER \
	DEFAULT_WINDOW, \
	.init = scroller_init, \
	.draw = scroller_draw, \
	.up = scroller_up, \
	.down = scroller_down, \
	.resume = scroller_resume

struct window_ops scroller_window = {
	INHERIT_FROM_SCROLLER,
};

/**********************************************************/

/* A scroller instance for system information */

void sysinfo_machine_name (void) { sprintf (MACHINE_NAME); }
void sysinfo_machine_version (void) {
	extern char build_date[];
#ifdef DEBUGGER
	sprintf ("D%s.%s  %s", 
		C_STRING(MACHINE_MAJOR_VERSION), C_STRING(MACHINE_MINOR_VERSION), 
		build_date);
#else
	sprintf ("R%s.%s  %s", 
		C_STRING(MACHINE_MAJOR_VERSION), C_STRING(MACHINE_MINOR_VERSION), 
		build_date);
#endif
}
void sysinfo_system_version (void) { 
#ifdef USER_TAG
	sprintf ("%s %s.%s", C_STRING(USER_TAG), 
		C_STRING(FREEWPC_MAJOR_VERSION), C_STRING(FREEWPC_MINOR_VERSION));
#else
	sprintf ("SY %s.%s", FREEWPC_MAJOR_VERSION, FREEWPC_MINOR_VERSION);
#endif
}
void sysinfo_compiler_version (void) { 
	sprintf ("GCC %s", C_STRING(GCC_VERSION));
}

void sysinfo_stats1 (void) {
#ifdef CONFIG_DEBUG_STACK
	extern U8 task_largest_stack;
	sprintf ("MAX STACK %d", task_largest_stack);
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
	window_call_op (win_top, draw);

	for (i=0; i < 16; i++)
		if (!switch_poll (SW_UP))
			return;
		else
			task_sleep (TIME_33MS);

	while (switch_poll (SW_UP))
	{
		window_call_op (win_top, up);
		window_call_op (win_top, draw);
		task_sleep (TIME_33MS);
	}
}

void test_down_button (void)
{
	U8 i;
	if (!win_top) return;

	window_call_op (win_top, down);
	window_call_op (win_top, draw);

	for (i=0; i < 16; i++)
		if (!switch_poll (SW_DOWN))
			return;
		else
			task_sleep (TIME_33MS);

	while (switch_poll (SW_DOWN))
	{
		window_call_op (win_top, down);
		window_call_op (win_top, draw);
		task_sleep (TIME_33MS);
	}
}


CALLSET_ENTRY (test_mode, sw_l_l_flipper_button)
{
	if (win_top)
	{
		window_call_op (win_top, left);
		window_call_op (win_top, draw);
	}
}


CALLSET_ENTRY (test_mode, sw_l_r_flipper_button)
{
	if (win_top)
	{
		window_call_op (win_top, right);
		window_call_op (win_top, draw);
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
		window_call_op (win_top, draw);
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
			window_call_op (win_top, draw);
	}
}

CALLSET_ENTRY (test_mode, sw_start_button)
{
	if (win_top)
	{
		window_call_op (win_top, start);
		window_call_op (win_top, draw);
	}
}

