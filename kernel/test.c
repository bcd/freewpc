
#include <freewpc.h>


uint8_t in_test;

typedef struct test
{
	const char *banner;
	const struct test *up;
	uint8_t start_index;
	uint8_t child_count;
	union {
		struct {
			const struct test *first_child;
		} menu;

		struct {
			void (*enter_proc) (void) __taskentry__;
		} item;
	} u;
} test_t;

extern const test_t main_menu;


#define TEST_MENU_COUNT(m)	(sizeof (m) / sizeof (struct test))


const test_t *test_menu;

uint8_t test_index;


void test_start (void)
{
	end_game ();
	amode_stop ();
	test_menu = &main_menu;
	in_test = 1;
	deff_restart (DEFF_TEST_MENU);
	sound_send (SND_ENTER);
}


void test_stop (void)
{
	deff_stop (DEFF_TEST_MENU);
	in_test = 0;
	amode_start ();
}



void test_menu_deff (void) __taskentry__ __noreturn__
{
	dmd_alloc_low_high ();
	dmd_clean_page (dmd_low_buffer);

	font_render_string_center (&font_5x5, 64, 4, test_menu->banner);

	dmd_copy_low_to_high ();

	sprintf ("%02x", test_index);
	font_render_string (&font_5x5, 8, 14, sprintf_buffer);

	if (test_menu->child_count)
	{
		font_render_string (&font_5x5, 32, 14, 
			test_menu->u.menu.first_child[test_index].banner);
	}


	for (;;)
	{
		dmd_show_high ();
		task_sleep (TIME_100MS);
		dmd_show_low ();
		task_sleep (TIME_100MS);
	}
}

void test_up (void)
{
	sound_send (SND_UP);
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
	sound_send (SND_DOWN);
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
		test_menu = &test_menu->u.menu.first_child[test_index];
		test_index = test_menu->start_index;
		sound_send (SND_ENTER);
		deff_restart (DEFF_TEST_MENU);
	}
	else
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


/********************************************************************/

void sound_enter_proc (void) __taskentry__
{
	sound_send (test_index);
	task_exit ();
}


void sound2_enter_proc (void) __taskentry__
{
	sound_send (0x100 + test_index);
	task_exit ();
}


void lamp_enter_proc (void) __taskentry__
{
	lamp_flash (test_index);
	task_exit ();
}


void sol_enter_proc (void) __taskentry__
{
	sol_on (test_index);
	task_sleep (TIME_100MS);
	sol_off (test_index);
	task_exit ();
}


void rtc_print_deff (void) __taskentry__
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 4, "CURRENT TIME");

	sprintf ("%d %d", *(uint8_t *)WPC_CLK_HOURS_DAYS, *(uint8_t *)WPC_CLK_MINS);
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
	dmd_alloc_low_clean ();
	font_render_string (&font_5x5, 0, 0, "ABCDEFGHIJKLM");
	font_render_string (&font_5x5, 0, 8, "NOPQRSTUVWXYZ");
	font_render_string (&font_5x5, 0, 16, "0123456789");
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




#define TEST_ITEM(p)	.u = { .item = { p } }
#define TEST_MENU(m)	TEST_MENU_COUNT(m), .u = { .menu = { m } }


const test_t main_menu_items[] = {
	{ "SOUNDS 1", &main_menu, 0, 0, TEST_ITEM(sound_enter_proc) },
	{ "SOUNDS 2", &main_menu, 0, 0, TEST_ITEM(sound2_enter_proc) },
	{ "LAMPS", &main_menu, 0, 0, TEST_ITEM(lamp_enter_proc) },
	{ "SOLENOIDS", &main_menu, 0, 0, TEST_ITEM(sol_enter_proc) },
	{ "BALL DEVICES", &main_menu, 0, 0, TEST_ITEM(device_enter_proc) },
	{ "RT CLOCK", &main_menu, 0, 0, TEST_ITEM(rtc_enter_proc) },
	{ "FONT TEST", &main_menu, 0, 0, TEST_ITEM(font_enter_proc) },
};

const test_t main_menu = {
	"MAIN MENU", NULL, 0, TEST_MENU(main_menu_items)
};



void test_init (void)
{
	in_test = 0;
}

