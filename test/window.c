
#include <freewpc.h>

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


struct window;

/** win_top always points to the current window, or NULL if
 * no window is open. */
struct window *win_top;

/* Equivalent to (win_top != NULL), but as a byte, this can
 * be tested with a single instruction.
 * TODO - these two variables could be overlapped into a union. */
U8 in_test;


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
	lamp_all_off ();
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
		window_pop_first ();
		win_top = NULL;
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

/* The menu object, which is shared between real menus
 * and menu items that aren't menus themselves.
 *
 * The M_xxx macros are used in the menu flags.
 */

#define M_MENU 0x1
#define M_ITEM 0x2
#define M_LETTER_PREFIX 0x4

struct menu
{
	const char *name;
	int flags;
	union {
		struct menu **submenus;
		struct {
			struct window_ops *ops;
			void *priv;
		} subwindow;
	} var;
};

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

	font_render_string_center (&font_5x5, 64, 2, m->name);

	if (browser_item_number)
	{
		(*browser_item_number) (menu_selection);
		font_render_string (&font_5x5, 8, 20, sprintf_buffer);
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
	font_render_string (&font_5x5, 56, 20, s);
}

/**********************************************************/

/* A window class for adjustment browsing.
 * It supports all of the standard browser operations,
 * but the enter key can be used to go into edit mode
 * and change the value of the item.
 */

#define ADJ_BROWSING 0
#define ADJ_EDITING 1
#define ADJ_CONFIRMING 2

struct adjustment_value
{
	U8 min;
	U8 max;
	U8 step;
	void (*render) (U8 val);
};

struct adjustment
{
	const char *name;
	struct adjustment_value *values;
	U8 factory_default;
	U8 *nvram;
};

struct adjustment *browser_adjs;
U8 adj_edit_value;

void decimal_render (U8 val) { sprintf ("%d", val); }
void hex_render (U8 val) { sprintf ("%X", val); }
void on_off_render (U8 val) { sprintf (val ? "ON" : "OFF"); }
void yes_no_render (U8 val) { sprintf (val ? "YES" : "NO"); }
void clock_style_render (U8 val) { sprintf (val ? "24 HOUR" : "AM/PM"); }
void date_style_render (U8 val) { sprintf (val ? "D/M/Y" : "M/D/Y"); }
void lang_render (U8 val) { sprintf ("ENGLISH"); }

void hs_reset_render (U8 val)
{ 
	if (val == 0)
		sprintf ("OFF");
	else
		sprintf ("%ld", val * 250UL);
}

struct adjustment_value integer_value = { 0, 0xFF, 1, decimal_render };
struct adjustment_value nonzero_integer_value = { 1, 0xFF, 1, decimal_render };
struct adjustment_value balls_per_game_value = { 1, 10, 1, decimal_render };
struct adjustment_value players_per_game_value = { 1, 4, 1, decimal_render };
struct adjustment_value max_eb_value = { 0, 10, 1, decimal_render };
struct adjustment_value on_off_value = { 0, 1, 1, on_off_render };
struct adjustment_value yes_no_value = { 0, 1, 1, yes_no_render };
struct adjustment_value game_restart_value = { 0, 2, 1, decimal_render };
struct adjustment_value max_credits_value = { 5, 99, 1, decimal_render };
struct adjustment_value hs_reset_value = { 0, 80, 1, hs_reset_render };
struct adjustment_value clock_style_value = { 0, 1, 1, clock_style_render };
struct adjustment_value date_style_value = { 0, 1, 1, date_style_render };
struct adjustment_value score_value = { 0, 250, 10, decimal_render };
struct adjustment_value lang_value = { 0, 0, 0, lang_render };

struct adjustment standard_adjustments[] = {
	{ "BALLS PER GAME", &balls_per_game_value, 3, &system_config.balls_per_game },
	{ "MAX PLAYERS", &players_per_game_value, 1, &system_config.max_players },
	{ "TILT WARNINGS", &integer_value, 3, &system_config.tilt_warnings },
	{ "MAX E.B.", &max_eb_value, 5, &system_config.max_ebs },
	{ "MAX EB PER BIP", &max_eb_value, 4, &system_config.max_ebs_per_bip },
	{ "LANGUAGE", &lang_value, 0, &system_config.language },
	{ "CLOCK STYLE", &clock_style_value, 0, &system_config.clock_style },
	{ "DATE STYLE", &date_style_value, 0, &system_config.date_style },
	{ "ALLOW DIM ALLUM.", &yes_no_value, NO, &system_config.allow_dim_illum },
	{ "TOURNAMENT MODE", &yes_no_value, NO, &system_config.tournament_mode },
	{ "EURO. DIGIT SEP.", &yes_no_value, NO, &system_config.euro_digit_sep },
	{ "NO BONUS FLIPS", &yes_no_value, NO, &system_config.no_bonus_flips },
	{ "GAME RESTART", &game_restart_value, 0, &system_config.game_restart },
	{ NULL, NULL, 0, NULL },
};


struct adjustment feature_adjustments[] = {
#ifdef MACHINE_LAUNCH_SWITCH
	{ "TIMED PLUNGER", &on_off_value, OFF, &system_config.timed_plunger },
	{ "FLIPPER PLUNGER", &on_off_value, OFF, &system_config.flipper_plunger },
#else
	{ "", &on_off_value, OFF, NULL }, /* skip over */
	{ "", &on_off_value, OFF, NULL }, /* skip over */
#endif
	{ "FAMILY MODE", &yes_no_value, NO, &system_config.family_mode },
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
	{ "1-COIN BUY-IN", &yes_no_value, NO, NULL },
	{ "COIN METER UNITS", &integer_value, 0, NULL },
	{ "DOLLAR BILL SLOT", &yes_no_value, NO, NULL },
	{ "MIN. COIN MSEC.", &nonzero_integer_value, 50, NULL },
	{ "SLAMTILT PENALTY", &yes_no_value, YES, NULL },
	{ "ALLOW HUNDREDTHS", &yes_no_value, NO, NULL },
	{ "CREDIT FRACTION", &on_off_value, OFF, NULL },
	{ NULL, NULL, 0, NULL },
};


struct adjustment hstd_adjustments[] = {
	{ "HIGHEST SCORES", &on_off_value, ON, &hstd_config.highest_scores },
	{ "H.S.T.D. AWARD", &on_off_value, ON, &hstd_config.hstd_award },
	{ "CHAMPION H.S.T.D.", &on_off_value, ON, &hstd_config.champion_hstd },
	{ "CHAMPION CREDITS", &on_off_value, ON, NULL },
	{ "H.S.T.D. 1 CREDITS", &integer_value, 1, &hstd_config.hstd_credits[0] },
	{ "H.S.T.D. 2 CREDITS", &integer_value, 1, &hstd_config.hstd_credits[1] },
	{ "H.S.T.D. 3 CREDITS", &integer_value, 1, &hstd_config.hstd_credits[2] },
	{ "H.S.T.D. 4 CREDITS", &integer_value, 1, &hstd_config.hstd_credits[3] },
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
		*(adjs->nvram) = adjs->factory_default;
		adjs++;
	}
	wpc_nvram_put ();
}


void adj_verify (struct adjustment *adjs)
{
	U8 val;

	while (adjs->name != NULL)
	{
		val = *(adjs->nvram);
		if ((val < adjs->values->min) || (val > adjs->values->max))
		{
			wpc_nvram_get ();
			*(adjs->nvram) = adjs->factory_default;
			wpc_nvram_put ();
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
	font_render_string_center (&font_5x5, 64, 10, sprintf_buffer);

	if (ad->nvram == NULL)
	{
		font_render_string_center (&font_5x5, 32, 20, "N/A");
	}
	else
	{
		if (browser_action == ADJ_EDITING)
			ad->values->render (adj_edit_value);
		else if (ad->nvram)
			ad->values->render (adj_edit_value = *(ad->nvram));

		font_render_string_center (&font_5x5, 32, 21, sprintf_buffer);

		if (adj_edit_value == ad->factory_default)
			font_render_string_center (&font_5x5, 96, 21, "DEFAULT");
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
	browser_max = -1;
	while (ad->name != NULL)
	{
		browser_max++;
		ad++;
	}
	if (browser_max == -1)
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
		//browser_action = ADJ_CONFIRMING;

		if (*(browser_adjs[menu_selection].nvram) != adj_edit_value)
		{
			wpc_nvram_get ();
			*(browser_adjs[menu_selection].nvram) = adj_edit_value;
			wpc_nvram_put ();
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

struct audit
{
	const char *name;
	void (*render) (audit_t);
	audit_t *nvram;
};


void integer_audit (audit_t val) 
{ 
	sprintf ("%ld", val);
}


struct audit *browser_audits;


struct audit standard_audits[] = {
	{ "LEFT COINS", &integer_audit, &system_audits.coins_added[0] },
	{ "CENTER COINS", &integer_audit, &system_audits.coins_added[1] },
	{ "RIGHT COINS", &integer_audit, &system_audits.coins_added[2] },
	{ "4TH SLOT COINS", &integer_audit, &system_audits.coins_added[3] },
	{ "PAID CREDITS", &integer_audit, &system_audits.paid_credits },
	{ "SERVICE CREDITS", &integer_audit, &system_audits.service_credits },
	{ "GAMES STARTED", &integer_audit, &system_audits.games_started },
	{ "TILTS", &integer_audit, &system_audits.tilts },
	{ "LEFT DRAINS", &integer_audit, &system_audits.left_drains },
	{ "RIGHT DRAINS", &integer_audit, &system_audits.right_drains },
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
	font_render_string_center (&font_5x5, 64, 10, sprintf_buffer);

	if (aud->nvram)
		aud->render (*(aud->nvram));
	font_render_string_center (&font_5x5, 32, 21, sprintf_buffer);

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

static int count_submenus (struct menu *m)
{
	int count = 0;
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

	font_render_string (&font_5x5, 8, 4, m->name);

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
		font_render_string (&font_5x5, 8, 14, sprintf_buffer);
	}
	else
	{
		font_render_string (&font_5x5, 8, 14, "ERROR... NO SUBMENUS");
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

const font_t *font_test_lookup (void)
{
	switch (menu_selection)
	{
		case 0: default: return &font_5x5;
		case 1: return &font_9x6;
		case 2: return &font_fixed10;
		case 3: return &font_fixed6;
		case 4: return &font_lucida9;
		case 5: return &font_term6;
		case 6: return &font_times8;
		case 7: return &font_times10;
		case 8: return &font_helv8;
		case 9: return &font_schu;
		case 10: return &font_misctype;
		case 11: return &font_utopia;
#if 0
				  /* Not working */
		case 100: return &font_cu17;
#endif
	}
}


void font_test_init (void)
{
	browser_init ();
	browser_max = 11; /* set to highest valid font number */
}

void font_test_draw (void)
{
	const font_t *font = font_test_lookup ();

	dmd_alloc_low_clean ();

	sprintf ("FONT %d", menu_selection+1);
	font_render_string_center (&font_5x5, 64, 16, sprintf_buffer);

	dmd_show_low ();
	task_sleep (TIME_100MS * 5);

	dmd_alloc_low_clean ();
	font_render_string (font, 0, 0, "ABCDEFGHIJKLM");
	task_dispatching_ok = TRUE;
	font_render_string (font, 0, 10, "NOPQRSTUVWXYZ");
	task_dispatching_ok = TRUE;
	font_render_string (font, 0, 20, "0123456789");
	task_dispatching_ok = TRUE;
	dmd_show_low ();
}

struct window_ops font_test_window = {
	INHERIT_FROM_BROWSER,
	.init = font_test_init,
	.draw = font_test_draw,
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
	U8 (*get_active) (void);
};

struct deff_leff_ops *deff_leff_test_ops;
U8 deff_leff_last_active;


struct deff_leff_ops dev_deff_ops = {
	.start = deff_start,
	.stop = deff_stop,
	.get_active = deff_get_active,
};

struct deff_leff_ops dev_leff_ops = {
	.start = leff_start,
	.stop = leff_stop,
	.get_active = leff_get_active,
};

void deff_leff_thread (void)
{
	browser_draw ();
	for (;;)
	{
		U8 is_active = deff_leff_test_ops->get_active ();
		if (is_active != deff_leff_last_active)
		{
			if (deff_leff_test_ops == &dev_deff_ops) 
			{
				if (is_active != menu_selection)
				{
					browser_draw ();
					browser_print_operation ("STOPPED");
					sound_reset ();
				}
			}
			else
			{
				browser_draw ();
				if (is_active == menu_selection)
					browser_print_operation ("RUNNING");
				else
					browser_print_operation ("STOPPED");
			}
		}
		deff_leff_last_active = is_active;
		task_sleep (TIME_100MS * 4);
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
		browser_max = MAX_DEFFS;
	}
	else
	{
		deff_leff_test_ops = &dev_leff_ops;
		browser_max = MAX_LEFFS;
	}
	deff_leff_last_active = 0xEE;
}

void deff_leff_up (void) { browser_up (); deff_leff_last_active++; }
void deff_leff_down (void) { browser_down (); deff_leff_last_active++; }

void deff_leff_enter (void)
{
	if (deff_leff_test_ops->get_active () == menu_selection)
	{
		/* deff/leff already running, so stop it */
		deff_leff_test_ops->stop (menu_selection);
	}
	else
	{
		/* deff/leff not running, so start it */
		deff_leff_test_ops->start (menu_selection);
	}
	sound_send (SND_TEST_ENTER);
	deff_leff_last_active++;
}

struct window_ops deff_leff_window = {
	INHERIT_FROM_BROWSER,
	.init = deff_leff_init,
	.thread = deff_leff_thread,
	.enter = deff_leff_enter,
	.up = deff_leff_up,
	.down = deff_leff_down,
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

/*********** Lampsets **********************/

void lampset_init (void)
{
	browser_init ();
	browser_min = 1;
	browser_max = MAX_LAMPSET;
	browser_item_number = browser_decimal_item_number;
}

void lampset_draw (void)
{
	browser_draw ();
	lamp_all_off ();
	lampset_set_apply_delay (0);
	lampset_apply_on (menu_selection);

}

struct window_ops dev_lampset_window = {
	INHERIT_FROM_BROWSER,
	.init = lampset_init,
	.draw = lampset_draw,
	.exit = lamp_all_off,
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
		sprintf ("DEV %d NOT INSTALLED", menu_selection);
		font_render_string_center (&font_5x5, 64, 3, sprintf_buffer);
	}
	else
	{
		sprintf ("DEV %d   %s", menu_selection, dev->props->name);
		font_render_string_center (&font_5x5, 64, 3, sprintf_buffer);
	
		sprintf ("SIZE %d", dev->size);
		font_render_string (&font_5x5, 8, 7, sprintf_buffer);

		sprintf ("COUNT %d", dev->actual_count);
		font_render_string (&font_5x5, 8, 13, sprintf_buffer);

		sprintf ("SOL %d", dev->props->sol+1);
		font_render_string (&font_5x5, 8, 19, sprintf_buffer);

		sprintf ("COUNTED %d", counted_balls);
		font_render_string (&font_5x5, 72, 7, sprintf_buffer);

		sprintf ("MISSING %d", missing_balls);
		font_render_string (&font_5x5, 72, 13, sprintf_buffer);

		switch (browser_action)
		{
			case 0: default: s = "EJECT 1"; break;
			case 1: s = "EJECT ALL"; break;
			case 2: s = "ENABLE LOCK"; break;
			case 3: s = "DISABLE LOCK"; break;
		}
		font_render_string_center (&font_5x5, 64, 28, s);
	}

	dmd_show_low ();
}


void dev_balldev_test_thread (void)
{
	U8 last_count = 0;
	device_t *last_dev = &device_table[menu_selection];
	int i;

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
			/* enable lock */
			break;
		case 3:
			/* disable lock */
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
	.var = { .subwindow = { &dev_balldev_test_window, NULL } },
};

/*************** Sound Editor *************************/

#define SOUNDEDIT_NOP		0xFFFF
#define SOUNDEDIT_SOUND(x)	(0x0000 + (x))
#define SOUNDEDIT_DELAY(x)	(0x4000 + (x))

U16 dev_soundedit_program[64];
U8 dev_soundedit_cursor;
U8 dev_soundedit_page_start;

void dev_soundedit_print_op (U16 op)
{
	if (op == SOUNDEDIT_NOP)
		sprintf ("");
	else if (op < SOUNDEDIT_DELAY(0))
		sprintf ("SND %04lx", op);
	else
		sprintf ("WAIT %04lx", op - 0x4000);
}

void dev_soundedit_init (void)
{
	memset (dev_soundedit_program, 0xFF, sizeof (dev_soundedit_program));
	dev_soundedit_cursor = 0;
	dev_soundedit_page_start = 0;
}

void dev_soundedit_draw (void)
{
	int i;

	dmd_alloc_low_clean ();
	for (i=0; i < 4; i++)
	{
		sprintf ("%s %02d",
			(dev_soundedit_page_start+i == dev_soundedit_cursor) ? "X" : "O",
			dev_soundedit_page_start+i);
		font_render_string (&font_5x5, 16, i*6+1, sprintf_buffer);
		dev_soundedit_print_op (dev_soundedit_program[dev_soundedit_page_start+i]);
		font_render_string (&font_5x5, 48, i*6+1, sprintf_buffer);
	}
	dmd_show_low ();
}

void dev_soundedit_up (void)
{
	dev_soundedit_cursor++;
	dev_soundedit_cursor %= 64;

	if (dev_soundedit_cursor / 4 != dev_soundedit_page_start / 4)
	{
		dev_soundedit_page_start += 4;
		dev_soundedit_page_start %= 64;
	}
}

void dev_soundedit_down (void)
{	
	dev_soundedit_cursor--;
	dev_soundedit_cursor %= 64;

	if (dev_soundedit_cursor / 4 != dev_soundedit_page_start / 4)
	{
		dev_soundedit_page_start -= 4;
		dev_soundedit_page_start %= 64;
	}
}

struct window_ops dev_soundedit_window = {
	DEFAULT_WINDOW,
	.init = dev_soundedit_init,
	.up = dev_soundedit_up,
	.down = dev_soundedit_down,
	.draw = dev_soundedit_draw,
};

struct menu dev_soundedit_item = {
	.name = "SOUND EDITOR",
	.flags = M_ITEM,
	.var = { .subwindow = { &dev_soundedit_window, NULL } },
};

/**********************************************************************/

void dev_random_test_draw (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%d", random ());
	font_render_string_center (&font_mono5, 64, 16, sprintf_buffer);
	dmd_show_low ();
}

struct window_ops dev_random_test_window = {
	DEFAULT_WINDOW,
	.draw = dev_random_test_draw,
};

struct menu dev_random_test_item = {
	.name = "RANDOM TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &dev_random_test_window, NULL } },
};

/**********************************************************************/

struct menu *dev_menu_items[] = {
	&dev_font_test_item,
	&dev_deff_test_item,
	&dev_leff_test_item,
	&dev_lampset_test_item,
	&dev_balldev_test_item,
	&dev_soundedit_item,
	&dev_random_test_item,
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

struct menu set_time_item = {
	.name = "SET TIME/DATE",
	.flags = M_ITEM,
};

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

struct menu burnin_item = {
	.name = "AUTO BURN-IN",
	.flags = M_ITEM,
};

/**********************************************************************/

struct menu presets_menu_item = {
	.name = "PRE-SETS",
	.flags = M_ITEM,
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
};

struct menu earnings_audits_item = {
	.name = "EARNINGS AUDITS",
	.flags = M_ITEM,
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

struct menu *audit_menu_items[] = {
	&main_audits_item,
	&earnings_audits_item,
	&standard_audits_item,
	&feature_audits_item,
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
	.var = { .subwindow = { &adj_browser_window, pricing_adjustments } },
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

void switch_matrix_draw (void)
{
	U8 row, col;

	for (row=0; row < 8; row++)
	{
		for (col=0; col < 8; col++)
		{
			U8 sw = MAKE_SWITCH (col+1,row+1);
			bool opto_p = switch_is_opto (sw);
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
	if (val < 8)
		sprintf ("D%d", val+1);
	else if (val >= SW_LR_FLIP_EOS)
		sprintf ("F%d", val - SW_LR_FLIP_EOS + 1);
	else
	{
		val -= 8;
		sprintf ("SW%d%d", (val / 8)+1, (val % 8)+1);
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

	browser_draw ();

	state = switch_poll (sel) ? "CLOSED" : "OPEN";
	opto = switch_is_opto (sel) ? "OPTO " : "";

	sprintf ("%s%s", opto, state);
	browser_print_operation (sprintf_buffer);
}

void single_switch_thread (void)
{
	U8 *sel = &win_top->w_class.menu.selected;
	U8 selected = *sel;
	int sw_state = switch_poll (*sel);
	int sw_poll;

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
	font_render_string_center (&font_5x5, 64, 9, s);
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
	sound_code_t snd = ((unsigned long)sound_test_set << 8) + sel;
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
	font_render_string_center (&font_5x5, 64, 12, s);
}

void solenoid_test_enter (void)
{
	U8 sel = win_top->w_class.menu.selected;
	browser_print_operation ("PULSING");
	task_sleep (TIME_100MS * 3);

	sol_on (sel);
	task_sleep (browser_action);
	sol_off (sel);

	browser_print_operation ("PULSE OFF");
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
	"STRING 1 ON",
	"STRING 2 ON",
	"STRING 3 ON",
	"STRING 4 ON",
	"STRING 5 ON",
	"ALL ON",
};

void gi_test_init (void)
{
	browser_init ();
	browser_max = NUM_GI_TRIACS+1;
}

void gi_test_draw (void)
{
	browser_draw ();
	browser_print_operation (gi_test_names[menu_selection]);
	sprintf ("BRIGHTNESS %d", triac_get_brightness ());
	font_render_string_center (&font_5x5, 64, 29, sprintf_buffer);

	triac_disable (TRIAC_GI_MASK);
	triac_enable (gi_test_values[menu_selection]);
}

void gi_test_right (void)
{
	U8 n = triac_get_brightness ();
	if (n < 8)
		triac_set_brightness (n+1);
}

void gi_test_left (void)
{
	U8 n = triac_get_brightness ();
	if (n != 0)
		triac_set_brightness (n-1);
}


struct window_ops gi_test_window = {
	INHERIT_FROM_BROWSER,
	.init = gi_test_init,
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
	sprintf ("LAMP%1d%1d", (val / 8) + 1, (val % 8) + 1);
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
	browser_print_operation ("FLASHING");
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
	int i;

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
	font_render_string_center (&font_5x5, 64, 3, "DIP SWITCHES");

	for (sw = 0; sw < 8; sw++)
	{
		dipsw_render_single (sw+1, dipsw & 0x1);
		font_render_string (&font_5x5, 
			(sw <= 3) ? 8 : 72, 
			(sw % 4) * 6 + 9, 
			sprintf_buffer);
		dipsw >>= 1;
	}

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
	sprintf ("%02X", *(U8 *)(ASIC_REG_BASE + menu_selection));
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

struct menu empty_balls_item = {
	.name = "EMPTY BALLS TEST",
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
	&empty_balls_item,
#ifdef MACHINE_TEST_MENU_ITEMS
	// MACHINE_TEST_MENU_ITEMS
#endif
	NULL,
};

struct menu test_menu = {
	.name = "TESTS",
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
	font_render_string_center (&font_5x5, 64, 10, sprintf_buffer);
	s[offset * 2 + 1] ();
	font_render_string_center (&font_5x5, 64, 22, sprintf_buffer);
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
	sprintf ("%s %1x.%02x", C_STRING(USER_TAG), 
		C_STRING(FREEWPC_MAJOR_VERSION), C_STRING(FREEWPC_MINOR_VERSION));
#else
	sprintf ("SY %1x.%02x", FREEWPC_MAJOR_VERSION, FREEWPC_MINOR_VERSION);
#endif
}
void sysinfo_compiler_version (void) { 
	sprintf ("GCC %s  ASM %s", C_STRING(GCC_VERSION), C_STRING(AS_VERSION));
}

#ifdef DEBUGGER
void sysinfo_stats1 (void) {
	extern U8 task_largest_stack;
	sprintf ("MAX STACK %d", task_largest_stack);
}
void sysinfo_stats2 (void) {
#ifdef TASKCOUNT
	extern U16 task_max_count;
	sprintf ("MAX TASKS %d", task_max_count);
#else
	extern U16 task_largest_stack_pc;
	sprintf ("SAVE FROM %p", task_largest_stack_pc);
#endif
}
#endif

scroller_item sysinfo_scroller[] = {
	sysinfo_machine_name, sysinfo_machine_version,
	sysinfo_system_version, sysinfo_compiler_version,
#ifdef DEBUGGER
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
	int i;
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
	int i;
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

void test_left_flipper_button (void)
{
	if (!win_top) return;
	window_call_op (win_top, left);
	window_call_op (win_top, draw);
}

void test_right_flipper_button (void)
{
	if (!win_top) return;
	window_call_op (win_top, right);
	window_call_op (win_top, draw);
}

void test_enter_button (void)
{
	if (!win_top)
	{
		window_push (&sysinfo_scroller_window, &sysinfo_scroller);
	}
	else
	{
		window_call_op (win_top, enter);
		window_call_op (win_top, draw);
	}
}

void test_escape_button (void)
{
	if (!win_top) return;
	window_call_op (win_top, escape);
	if (!win_top)
		return;
	else
		window_call_op (win_top, draw);
}

void test_start_button (void)
{
	if (!win_top) return;
	window_call_op (win_top, start);
	window_call_op (win_top, draw);
}

