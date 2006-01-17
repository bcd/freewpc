
#include <freewpc.h>

/** 1 if we are in test mode, 0 if not */
__fastram__ U8 in_test;

extern const test_t main_menu;
extern const test_t main_menu_items[];
extern const test_t dev_menu_items[];

const test_t *test_menu_stack[8];
const test_t **test_menu_stack_top;

__fastram__ const test_t *test_menu;

/** The index of the current menu item that is being displayed */
U8 test_index;


void test_start (void)
{
	in_test = 1;
	end_game ();
	amode_stop ();
	/// wpc_set_rom_page (59);
	test_menu = &main_menu;
	test_menu_stack_top = &test_menu_stack[0];
	test_index = 0;
	sound_reset ();
	deff_stop_all ();
	sound_send (SND_TEST_ENTER);
	deff_restart (DEFF_TEST_MENU);
}


void test_stop (void)
{
	/// wpc_set_rom_page (61);
	deff_stop (DEFF_TEST_MENU);
	in_test = 0;
	amode_start ();
}



__taskentry__ __noreturn__ void test_menu_deff (void)
{
	dmd_alloc_low_high ();
	dmd_clean_page (dmd_low_buffer);

	font_render_string_center (&font_5x5, 64, 4, test_menu->banner);
	dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 11);

	dmd_copy_low_to_high ();

	sprintf ("%d.", test_index+1);
	font_render_string (&font_5x5, 8, 14, sprintf_buffer);

	if (test_menu->child_count)
	{
		const test_t *child_menu = &test_menu->u.menu.first_child[test_index];

		font_render_string (&font_5x5, 24, 14, child_menu->banner);

		if (child_menu->u.item.value_format)
		{
			(*child_menu->u.item.value_format) (child_menu->u.item.value);
			font_render_string (&font_5x5, 24, 21, sprintf_buffer);

			dmd_show_low ();
			for (;;)
				task_sleep_sec (5);
		}
	}
	else
	{
		sprintf ("%02X", test_index);
		font_render_string (&font_5x5, 32, 14, sprintf_buffer);
	}

	for (;;)
	{
		dmd_show_low ();
		task_sleep (TIME_100MS);
		dmd_show_high ();
		task_sleep (TIME_100MS);
	}
}

void test_up (void)
{
	sound_send (SND_TEST_UP);
	test_index++;
	if ((test_menu->child_count) && (test_index >= test_menu->child_count))
		test_index = test_menu->start_index;
	deff_restart (DEFF_TEST_MENU);
}

void test_up_button (void)
{
	uint8_t i;

	if (!in_test) return;
	test_up ();

	for (i=20; i > 0; --i)
		if (switch_poll (SW_UP) == FALSE)
			return;
		else
			task_sleep (TIME_33MS);

	while (switch_poll (SW_UP) == TRUE)
	{
		test_up ();
		task_sleep (TIME_100MS);
	}
}


void test_down (void)
{
	sound_send (SND_TEST_DOWN);
	if ((test_menu->child_count) && (test_index == 0))
		test_index = test_menu->child_count - 1;
	else
		test_index--;
	deff_restart (DEFF_TEST_MENU);
}


void test_down_button (void)
{
	if (!in_test) return;
	test_down ();
}

void test_enter_button (void)
{
	if (!in_test)
	{
		test_start ();
	}
	else if ((test_menu->child_count > 0) && (test_menu->u.menu.first_child))
	{
		if (test_menu->u.menu.first_child[test_index].u.item.enter_proc)
		{
			test_menu = &test_menu->u.menu.first_child[test_index];
			test_index = test_menu->start_index;
			sound_send (SND_TEST_ENTER);
			deff_restart (DEFF_TEST_MENU);
		}
		else
		{
			sound_send (SND_TEST_ALERT);
		}
	}
	else if (test_menu->u.item.enter_proc)
	{
		(*test_menu->u.item.enter_proc) ();
	}
}

void test_escape_button (void)
{
	if (!in_test) return;
	sound_send (SND_TEST_ESCAPE);
	if (test_menu->up)
	{
		test_menu = test_menu->up;
		test_index = test_menu->start_index;
		deff_restart (DEFF_TEST_MENU);
	}
	else
	{
		test_stop ();
	}
}


void test_start_button (void)
{
	if (!in_test) return;
}

void test_left_flipper_button (void)
{
	sound_send (SND_TEST_DOWN);
	if ((test_menu->child_count) && (test_index == 0))
		test_index = test_menu->child_count - 1;
	else
		test_index -= 0x10;
	deff_restart (DEFF_TEST_MENU);
}


void test_right_flipper_button (void)
{
	sound_send (SND_TEST_UP);
	test_index += 0x10;
	if ((test_menu->child_count) && (test_index >= test_menu->child_count))
		test_index = test_menu->start_index;
	deff_restart (DEFF_TEST_MENU);
}


/********************************************************************/

void sound_enter_proc (void) __taskentry__
{
	sound_send (test_index);
	task_exit ();
}


void sound2_enter_proc (void) __taskentry__
{
#if (MACHINE_DCS == 0)
	sound_send (0x100 + test_index);
#else
	sound_send (0x100 + test_index); /* or 0x300 for test sounds */
#endif
	task_exit ();
}


void lamp_enter_proc (void) __taskentry__
{
	lamp_flash_on (test_index);
	task_exit ();
}


void sol_enter_proc (void) __taskentry__
{
	sol_on (test_index);
	task_sleep (TIME_100MS);
	sol_off (test_index);
	task_exit ();
}


void flasher_enter_proc (void) __taskentry__
{
	flasher_pulse (test_index);
	task_exit ();
}


void rtc_print_deff (void) __taskentry__
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 4, "CURRENT TIME");

	sprintf ("%02d %02d", 
		*(uint8_t *)WPC_CLK_HOURS_DAYS, 
		*(uint8_t *)WPC_CLK_MINS);

	font_render_string_center (&font_5x5, 64, 16, sprintf_buffer);

	dmd_draw_border (dmd_low_bytes);
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}


void rtc_enter_proc (void) __taskentry__
{
	deff_start (DEFF_PRINT_RTC);
	task_exit ();
}


void device_enter_proc (void) __taskentry__
{
	if (test_index < 0x10)
		device_request_kick (device_entry (test_index));
	else
		device_request_empty (device_entry (test_index - 0x10));
	task_exit ();
}


void font_test_deff (void)
{
	const font_t *font;

	switch (test_index)
	{
		case 0:
			font = &font_5x5;
			break;

		case 1:
			font = &font_9x6;
			break;

		default:
			font = NULL;
			break;
	}

	dmd_alloc_low_clean ();
	if (font != NULL)
	{
		font_render_string (font, 0, 0, "ABCDEFGHIJKLM");
		font_render_string (font, 0, 10, "NOPQRSTUVWXYZ");
		font_render_string (font, 0, 20, "0123456789.,");
	}
	else
	{
		font_render_string_center (&font_5x5, 64, 16, "INVALID FONT");
	}

	dmd_show_low ();
	task_sleep_sec (2);
	while (switch_poll_logical (SW_ESCAPE) == FALSE)
		task_sleep (TIME_66MS);
	deff_exit ();
}

void font_enter_proc (void) __taskentry__
{
	deff_start (DEFF_FONT_TEST);
	task_exit ();
}


void leff_enter_proc (void) __taskentry__
{
	leff_start (test_index);
	task_exit ();
}


void deff_enter_proc (void) __taskentry__
{
	deff_start (test_index);
	task_exit ();
}

/********************************************************************/

void gcc_version_formatter (U8 *unused)
{
	sprintf (C_STRING (GCC_VERSION));
}

void build_date_formatter (U8 *unused)
{
	sprintf (__DATE__);
}

void user_tag_formatter (U8 *unused)
{
	sprintf (C_STRING (USER_TAG));
}

void yes_no_formatter (U8 *boolp)
{
	sprintf (*boolp ? "YES" : "NO");
}

void asm_version_formatter (U8 *unused)
{
	sprintf (C_STRING (AS_VERSION));
}

void switch_edges_formatter (U8 *unused)
{
}

/********************************************************************/


const test_t effect_menu_items[] = {
};


const test_t test_menu_items[] = {
	{ "SOUNDS 1", &main_menu_items[0], 0, 0, TEST_ITEM(sound_enter_proc) },
	{ "SOUNDS 2", &main_menu_items[0], 0, 0, TEST_ITEM(sound2_enter_proc) },
	{ "SWITCH EDGES", &main_menu_items[0], 0, 0, TEST_ITEM(NULL) },
	{ "SINGLE SWITCH", &main_menu_items[0], 0, 0, TEST_ITEM(NULL) },
	{ "MUSIC", &main_menu_items[0], 0, 0, TEST_ITEM(NULL) },
	{ "LAMPS", &main_menu_items[0], 0, 0, TEST_ITEM(lamp_enter_proc) },
	{ "ALL LAMPS", &main_menu_items[0], 0, 0, TEST_ITEM(NULL) },
	{ "SOLENOIDS", &main_menu_items[0], 0, 0, TEST_ITEM(sol_enter_proc) },
	{ "FLASHERS", &main_menu_items[0], 0, 0, TEST_ITEM(flasher_enter_proc) },
	{ "GEN. ILLUMINATION", &main_menu_items[0], 0, 0, TEST_ITEM(NULL) },
	{ "BALL DEVICES", &main_menu_items[0], 0, 0, TEST_ITEM(device_enter_proc) },
};

const test_t adj_menu_items[] = {
	{ "LOCALE", &main_menu_items[1], 0, 0, TEST_ITEM(NULL) },
	{ "DIM LAMPS", &main_menu_items[1], 0, 0, TEST_ITEM(NULL) },
	{ "GAME RESTART", &main_menu_items[1], 0, 0, TEST_ITEM(NULL) },
};

const test_t pricing_menu_items[] = {
};

const test_t utility_menu_items[] = {
	{ "DATE AND TIME", &main_menu_items[3], 0, 0, TEST_ITEM(rtc_enter_proc) },
	{ "CUSTOM MESSAGE", &main_menu_items[3], 0, 0, TEST_ITEM(NULL) },
	{ "RESET HIGH SCORES", &main_menu_items[3], 0, 0, TEST_ITEM(NULL) },
};

extern U8 db_attached;

const test_t system_info_items[] = {
	{ "GCC VERSION", &dev_menu_items[0], RO_VALUE_ITEM(gcc_version_formatter, NULL) },
	{ "ASM VERSION", &dev_menu_items[0], RO_VALUE_ITEM(asm_version_formatter, NULL) },
	{ "BUILD DATE", &dev_menu_items[0], RO_VALUE_ITEM(build_date_formatter, NULL) },
	{ "USER TAG", &dev_menu_items[0], RO_VALUE_ITEM(user_tag_formatter, NULL) },
	{ "DEBUG ATTACHED", &dev_menu_items[0], RO_VALUE_ITEM(yes_no_formatter, &db_attached) },
};

const test_t dev_menu_items[] = {
	{ "SYSTEM INFO", &main_menu_items[4], 0, TEST_MENU(system_info_items) },
	{ "FONT TEST", &main_menu_items[4], 0, 0, TEST_ITEM(font_enter_proc) },
	{ "DMD EFFECTS", &main_menu_items[4], 0, 0, TEST_ITEM(deff_enter_proc) },
	{ "LAMP EFFECTS", &main_menu_items[4], 0, 0, TEST_ITEM(leff_enter_proc) },
	{ "SOLENOID STEPPING", &main_menu_items[4], 0, 0, TEST_ITEM(NULL) },
	{ "RAM DUMP", &main_menu_items[4], 0, 0, TEST_ITEM(NULL) },
#ifdef MACHINE_TEST_MENU_ITEMS
	MACHINE_TEST_MENU_ITEMS
#endif
};

const test_t main_menu_items[] = {
	{ "TESTS", &main_menu, 0, TEST_MENU(test_menu_items) },
	{ "ADJUSTMENTS", &main_menu, 0, TEST_MENU(adj_menu_items) },
	{ "PRICING", &main_menu, 0, TEST_MENU(pricing_menu_items) },
	{ "UTILITIES", &main_menu, 0, TEST_MENU(utility_menu_items) },
	{ "DEVELOPERS", &main_menu, 0, TEST_MENU(dev_menu_items) },
};

const test_t main_menu = {
	"MAIN MENU", NULL, 0, TEST_MENU(main_menu_items)
};



void test_init (void)
{
	in_test = 0;
	test_menu_stack_top = NULL;
}

