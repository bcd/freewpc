
#include <freewpc.h>

///// #pragma section("testmode")

/*
 * Test Mode
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
 */

struct window_ops
{
	void (*init) (void);
	void (*exit) (void);

	void (*suspend) (void);
	void (*resume) (void);

	void (*draw) (void);

	/** Key handlers **/
	void (*escape) (void);
	void (*enter) (void);
	void (*up) (void);
	void (*down) (void);
	void (*left) (void);
	void (*right) (void);
	void (*start) (void);

	/** Thread function **/
	void (*thread) (void);
};


/** The device structure is a read-only descriptor that
 * contains various device properties. */

struct menu;

struct window
{
	struct window_ops *ops;
	union  {
		struct window_menu {
			struct menu *self;
			struct menu *parent;
			U8 selected;
		} menu;
		struct window_scroller {
			void *funcs;
			U8 offset;
			U8 size;
		} scroller;
		void *priv;
	} w_class;
};

#define menu_selection (win_top->w_class.menu.selected)


#define window_call_op(win, op) \
do { \
	if (win->ops->op) \
	{ \
		(*win->ops->op) (); \
	} \
} while (0)


extern struct window *win_top;

/**********************************************************/

struct window win_stack[16];
struct window *win_top;


void window_push_first (void)
{
	in_test = 1;
	end_game ();
	deff_stop_all ();
	leff_stop_all ();
}


void window_pop_first (void)
{
	in_test = 0;
	dmd_alloc_low_clean ();
	dmd_show_low ();
	amode_start ();
}

void window_start_thread (void)
{
	if (win_top->ops->thread)
		task_recreate_gid (GID_WINDOW_THREAD, win_top->ops->thread);
}

void window_stop_thread (void)
{
	task_kill_gid (GID_WINDOW_THREAD);
}

void window_push (struct window_ops *ops, void *priv)
{
	window_stop_thread ();
	if (win_top == NULL)
	{
		window_push_first ();
		win_top = &win_stack[0];
	}
	else
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


void window_pop (void)
{
	window_stop_thread ();
	window_call_op (win_top, exit);
	sound_send (SND_TEST_ESCAPE);

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


void window_init (void)
{
	win_top = NULL;
}


/***************************************************/

/* The menu object, which is shared between real menus
 * and menu items that aren't menus themselves. */

#define M_MENU 0x1
#define M_ITEM 0x2


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
void (*browser_item_number) (int);
U8 browser_min;
U8 browser_max;

void browser_default_item_number (int val)
{
	sprintf ("%02X", val);
}

void browser_init (void)
{
	struct menu *m = win_top->w_class.priv;
	
	win_top->w_class.menu.self = m;
	menu_selection = 0;	

	browser_action = 0;
	browser_last_selection_update = 0;
	browser_item_number = browser_default_item_number;
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
		menu_selection = browser_max-1;
}

#define INHERIT_FROM_BROWSER \
	.init = browser_init, \
	.draw = browser_draw, \
	.escape = window_pop, \
	.up = browser_up, \
	.down = browser_down \

struct window_ops browser_window = {
	INHERIT_FROM_BROWSER,
};

void browser_print_operation (const char *s)
{
	font_render_string (&font_5x5, 48, 20, s);
}

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
	dbprintf ("menu: init\n");

	struct menu *m = win_top->w_class.priv;
	
	win_top->w_class.menu.self = m;
	win_top->w_class.menu.parent = NULL;
	menu_selection = 0;
}

void menu_exit (void)
{
	dbprintf ("menu: exit\n");
}

void menu_draw (void)
{
	struct menu *m = win_top->w_class.menu.self;
	struct menu **subm;
	U8 *sel = &win_top->w_class.menu.selected;

	dmd_alloc_low_clean ();

	dbprintf ("menu: draw\n");
	font_render_string (&font_5x5, 8, 4, m->name);

	subm = m->var.submenus;
	if (subm != NULL)
	{
		sprintf ("%d. %s", (*sel)+1, subm[*sel]->name);
		font_render_string (&font_5x5, 8, 14, sprintf_buffer);
	}
	else
	{
		font_render_string (&font_5x5, 8, 14, "ERROR... NO SUBMENUS");
	}
	dmd_show_low ();
}

void menu_escape (void)
{
	dbprintf ("menu: escape\n");
	window_pop ();
}

void menu_enter (void)
{
	struct menu *m = win_top->w_class.menu.self;
	U8 sel = win_top->w_class.menu.selected;

	dbprintf ("menu: enter\n");

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
			window_push (ops, m);
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
	
	dbprintf ("menu: up\n");
	sound_send (SND_TEST_UP);
	(*sel)++;
	if ((*sel) >= count_submenus (m))
		*sel = 0;
}

void menu_down (void)
{
	struct menu *m = win_top->w_class.menu.self;
	U8 *sel = &win_top->w_class.menu.selected;
	
	dbprintf ("menu: down\n");
	sound_send (SND_TEST_DOWN);
	(*sel)--;
	if ((*sel) == 0xFF) 
	{
		*sel = count_submenus (m);
		if (*sel > 0)
			(*sel)--;
	}
}

void menu_left (void)
{
}

void menu_right (void)
{
}

void menu_start (void)
{
}

struct window_ops menu_window = {
	.init = menu_init,
	.draw = menu_draw,
	.escape = menu_escape,
	.enter = menu_enter,
	.up = menu_up,
	.down = menu_down,
	.left = menu_left,
	.right = menu_right,
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
	}
}

void font_test_init (void)
{
	browser_init ();
	browser_max = 1;
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
	font_render_string (font, 0, 10, "NOPQRSTUVWXYZ");
	font_render_string (font, 0, 20, "0123456789");
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

struct menu dev_deff_test_item = {
	.name = "DISPLAY EFFECTS",
	.flags = M_ITEM,
};

struct menu dev_leff_test_item = {
	.name = "LAMP EFFECTS",
	.flags = M_ITEM,
};


/*********** Ball Devices **********************/


void dev_balldev_test_init (void)
{
	browser_init ();
	browser_max = MAX_DEVICES-1;
}

#pragma long_branch
void dev_balldev_test_draw (void)
{
	extern U8 counted_balls, missing_balls;

	device_t *dev = &device_table[menu_selection];
	char *s;

	dmd_alloc_low_clean ();

	if ((dev == NULL) || (dev->props == NULL))
	{
		sprintf ("DEV %d NOT INSTALLED", menu_selection);
		font_render_string_center (&font_5x5, 64, 0, sprintf_buffer);
	}
	else
	{
		sprintf ("DEV %d   %s", menu_selection, dev->props->name);
		font_render_string_center (&font_5x5, 64, 0, sprintf_buffer);
	
		sprintf ("SIZE %d", dev->size);
		font_render_string (&font_5x5, 8, 6, sprintf_buffer);
		sprintf ("COUNT %d", dev->actual_count);
		font_render_string (&font_5x5, 8, 12, sprintf_buffer);
		sprintf ("SOL %d", dev->props->sol+1);
		font_render_string (&font_5x5, 8, 18, sprintf_buffer);

		task_sleep (TIME_16MS);

		sprintf ("COUNTED %d", counted_balls);
		font_render_string (&font_5x5, 72, 6, sprintf_buffer);
		sprintf ("MISSING %d", missing_balls);
		font_render_string (&font_5x5, 72, 12, sprintf_buffer);

		switch (browser_action)
		{
			case 0: default: s = "EJECT 1"; break;
			case 1: s = "EJECT ALL"; break;
			case 2: s = "ENABLE LOCK"; break;
			case 3: s = "DISABLE LOCK"; break;
		}
		font_render_string_center (&font_5x5, 64, 24, s);
	}
	dmd_show_low ();
}
#pragma short_branch


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
			}
			last_count = dev->actual_count;
			last_dev = dev;
			task_sleep (TIME_66MS);
		}

		dev_balldev_test_draw ();
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
	}
}

void dev_balldev_test_change (void)
{
	browser_action++;
	if (browser_action == 4)
		browser_action = 0;
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
		sprintf ("%s %02d %04lX",
			(dev_soundedit_page_start+i == dev_soundedit_cursor) ? "X" : "O",
			dev_soundedit_page_start+i,
			dev_soundedit_program[dev_soundedit_page_start+i]);
		font_render_string (&font_5x5, 16, i*6+1, sprintf_buffer);
		task_sleep (TIME_16MS);
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
	.init = dev_soundedit_init,
	.up = dev_soundedit_up,
	.down = dev_soundedit_down,
	.escape = window_pop,
	.draw = dev_soundedit_draw,
};

struct menu dev_soundedit_item = {
	.name = "SOUND EDITOR",
	.flags = M_ITEM,
	.var = { .subwindow = { &dev_soundedit_window, NULL } },
};


struct menu *dev_menu_items[] = {
	&dev_font_test_item,
	&dev_deff_test_item,
	&dev_leff_test_item,
	&dev_balldev_test_item,
	&dev_soundedit_item,
	NULL,
};

struct menu development_menu = {
	.name = "DEVELOPMENT",
	.flags = M_MENU,
	.var = { .submenus = dev_menu_items },
};

/**********************************************************************/

struct menu bookkeeping_menu = {
	.name = "BOOKKEEPING",
	.flags = M_MENU,
};

struct menu adjustments_menu = {
	.name = "ADJUSTMENTS",
	.flags = M_MENU,
};

struct menu switch_edges_item = {
	.name = "SWITCH EDGES",
	.flags = M_ITEM,
};

struct menu switch_levels_item = {
	.name = "SWITCH LEVELS",
	.flags = M_ITEM,
};

/*************** Single Switch Test ***********************/

void switch_item_number (int val)
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

		case 2:
			s = "UNDEFINED";
			break;

		case 3:
			s = "TEST MODE";
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
	if (sound_test_set > 3)
#endif
		sound_test_set = 0;
	win_top->w_class.menu.selected = 0;
}

void sound_test_play (U8 sel)
{
	sound_code_t snd = ((unsigned long)sound_test_set << 8) + sel;
	dbprintf ("test sound %04lX\n", snd);
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
	browser_last_selection_update = win_top->w_class.menu.selected + 1;
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

void solenoid_test_enter (void)
{
	U8 sel = win_top->w_class.menu.selected;
	sol_on (sel);
	browser_print_operation ("PULSE ON");
	task_sleep (TIME_100MS);
	sol_off (sel);
	browser_print_operation ("PULSE OFF");
}

struct window_ops solenoid_test_window = {
	INHERIT_FROM_BROWSER,
	.enter = solenoid_test_enter,
};

struct menu solenoid_test_item = {
	.name = "SOLENOID TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &solenoid_test_window, NULL } },
};

/****************** GI Test **************************/

struct menu gi_test_item = {
	.name = "GEN. ILLUMINATION",
	.flags = M_ITEM,
};

/****************** Lamp Test **************************/

struct menu lamp_test_item = {
	.name = "LAMP TEST",
	.flags = M_ITEM,
};

/****************** All Lamps Test ***********************/

void all_lamp_test_thread (void)
{
	for (;;)
	{
		lamp_all_on ();
		task_sleep_sec (1);
		lamp_all_off ();
		task_sleep (TIME_100MS * 5);
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
	.up = NULL,
	.down = NULL,
};

struct menu all_lamp_test_item = {
	.name = "ALL LAMPS TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &all_lamp_test_window, NULL } },
};
/***************** DIP Switch Test **********************/

void dipsw_test_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 3, "DIP SWITCHES");
	sprintf ("%02X", wpc_get_jumpers ());
	font_render_string_center (&font_5x5, 64, 18, sprintf_buffer);
	dmd_show_low ();
}

struct window_ops dipsw_test_window = {
	INHERIT_FROM_BROWSER,
	.draw = dipsw_test_draw,
};

struct menu dipsw_test_item = {
	.name = "DIP SWITCH TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &dipsw_test_window, NULL } },
};

/****************** WPC ASIC Tests ***********************/

#define ASIC_REG_BASE 0x3FD0
#define ASIC_REG_COUNT 0x30

void asic_register_item_number (int val)
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
	.name = "ASIC REGISTERS",
	.flags = M_ITEM,
	.var = { .subwindow = { &asic_register_window, NULL } },
};

struct menu asic_clock_item = {
	.name = "REAL TIME CLOCK",
	.flags = M_ITEM,
};

struct menu *asic_test_items[] = {
	&asic_register_item,
	&asic_clock_item,
	NULL,
};

struct menu asic_tests_menu = {
	.name = "WPC ASIC TESTS",
	.flags = M_MENU,
	.var = { .submenus = asic_test_items, },
};

/****************** TEST MENU **************************/

struct menu *test_menu_items[] = {
	&switch_edges_item,
	&switch_levels_item,
	&single_switches_item,
	&solenoid_test_item,
	&sound_test_item,
	&lamp_test_item,
	&all_lamp_test_item,
	&dipsw_test_item,
	&asic_tests_menu,
	NULL,
};

struct menu test_menu = {
	.name = "TESTS",
	.flags = M_MENU,
	.var = { .submenus = test_menu_items, },
};

struct menu *main_menu_items[] = {
	&bookkeeping_menu,
	&adjustments_menu,
	&test_menu,
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
	dmd_alloc_low_clean ();
	s[win_top->w_class.scroller.offset * 2] ();
	font_render_string_center (&font_5x5, 64, 10, sprintf_buffer);
	s[win_top->w_class.scroller.offset * 2 + 1] ();
	font_render_string_center (&font_5x5, 64, 22, sprintf_buffer);
	dmd_show_low ();
}

void scroller_up (void)
{
	if (++win_top->w_class.scroller.offset >= win_top->w_class.scroller.size)
		win_top->w_class.scroller.offset = 0;
}

void scroller_down (void)
{
	if (--win_top->w_class.scroller.offset == 0xFF)
		win_top->w_class.scroller.offset = win_top->w_class.scroller.size-1;
}

#define INHERIT_FROM_SCROLLER \
	.init = scroller_init, \
	.draw = scroller_draw, \
	.up = scroller_up, \
	.down = scroller_down, \
	.escape = window_pop

struct window_ops scroller_window = {
	INHERIT_FROM_SCROLLER,
};

/**********************************************************/

/* A scroller instance for system information */

void sysinfo_machine_name (void) { sprintf (MACHINE_NAME); }
void sysinfo_system_version (void) { 
	sprintf ("SYSTEM VERSION %1x.%02x", 
		FREEWPC_VERSION_MAJOR, FREEWPC_VERSION_MINOR);
}
void sysinfo_compiler_version (void) { 
	sprintf ("GCC VERSION %s", C_STRING(GCC_VERSION));
}
void sysinfo_assembler_version (void) {
	sprintf ("ASM VERSION %s", C_STRING(AS_VERSION));
}

scroller_item sysinfo_scroller[] = {
	sysinfo_machine_name,
	sysinfo_system_version,
	sysinfo_compiler_version,
	sysinfo_assembler_version,
};

void sysinfo_resume (void)
{
	win_top->w_class.scroller.offset = 0;
}

void sysinfo_enter (void)
{
	dbprintf ("Entering test mode\n");
	window_push (&menu_window, &main_menu);
}

struct window_ops sysinfo_scroller_window = {
	INHERIT_FROM_SCROLLER,
	.enter = sysinfo_enter,
	.resume = sysinfo_resume,
};

/**********************************************************/

/* Compatibility functions for old test mode */

U8 in_test;

//// #pragma section("sysrom")

void test_init (void)
{
	wpc_push_page (TEST_PAGE);
	window_init ();
	wpc_pop_page ();
	in_test = 0;
}

void test_up_button (void)
{
	int i;
	if (!win_top) return;

	window_call_op (win_top, up);
	window_call_op (win_top, draw);

	for (i=0; i < 32; i++)
		if (!switch_poll (SW_UP))
			return;
		else
			task_sleep (TIME_16MS);

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

	for (i=0; i < 32; i++)
		if (!switch_poll (SW_DOWN))
			return;
		else
			task_sleep (TIME_16MS);

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
		wpc_set_rom_page (TEST_PAGE);
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
		wpc_set_rom_page (TEST_PAGE);
	else
		window_call_op (win_top, draw);
}

void test_start_button (void)
{
	if (!win_top) return;
	window_call_op (win_top, start);
	window_call_op (win_top, draw);
}

