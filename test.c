
#include <freewpc.h>

typedef struct test_menu
{
	const char *banner;
	void (*enter_proc) (void) __taskentry__;
	const struct test_menu *next;
	uint8_t start_index;
} test_menu_t;

const test_menu_t *test_menu;

uint8_t test_index;


void test_menu_deff (void) __taskentry__ __noreturn__
{
	dmd_alloc_low_high ();
	dmd_clean_page (dmd_low_buffer);

	seg_write_bcd (SEG_ADDR (0,2,1), test_index);

	dmd_copy_low_to_high ();

	seg_write_string (SEG_ADDR (0,1,1), test_menu->banner);

	for (;;)
	{
		dmd_show_high ();
		task_sleep (TIME_100MS);
		dmd_show_low ();
		task_sleep (TIME_100MS);
	}
}


void test_loop (uint16_t unused_arg) __taskentry__
{
	deff_restart (DEFF_TEST_MENU);

	for (;;)
	{
		while (switch_bits[AR_RAW][0] == 0)
			task_sleep (TIME_33MS);

		switch (switch_bits[AR_RAW][0])
		{
			case SW_ROWMASK (SW_ESCAPE):
				break;

			case SW_ROWMASK (SW_DOWN):
				sound_send (SND_DOWN);
				test_index--;
				deff_restart (DEFF_TEST_MENU);
				break;

			case SW_ROWMASK (SW_UP):
				sound_send (SND_UP);
				test_index++;
				deff_restart (DEFF_TEST_MENU);
				break;

			case SW_ROWMASK (SW_ENTER):
				task_create_gid (0, test_menu->enter_proc, 0);
				task_sleep (TIME_1S * 2);
				break;
		}

		while (switch_bits[AR_RAW][0] != 0)
			task_sleep (TIME_33MS);
	}
}


void test_start (void)
{
	sound_send (SND_ENTER);
	test_menu = test_menu->next;
	test_index = test_menu->start_index;
	task_recreate_gid (GID_TEST_LOOP, test_loop, 0);
}


/********************************************************************/

void sound_enter_proc (void) __taskentry__
{
	*(uint8_t *)WPCS_DATA = test_index;
	task_exit ();
}

void music_enter_proc (void) __taskentry__
{
	*(uint8_t *)WPCS_DATA = test_index;
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
	seg_write_uint8 (SEG_ADDR(0,2,4), *(uint8_t *)WPC_CLK_HOURS_DAYS);
	seg_write_uint8 (SEG_ADDR(0,2,8), *(uint8_t *)WPC_CLK_MINS);
	asm ("jsr dmd_draw_border_low");
	dmd_show_low ();
	task_sleep (TIME_1S * 3);
	deff_exit ();
}


void rtc_enter_proc (void) __taskentry__
{
	deff_start (DEFF_PRINT_RTC);
	task_exit ();
}


const test_menu_t menus[] = {
	{ "SOUND TEST    ", sound_enter_proc, menus + 1, 0x00 },
	{ "MUSIC TEST    ", music_enter_proc, menus + 2, 0x00 },
	{ "SOLENOID TEST ", sol_enter_proc, menus + 3, 0x00 },
	{ "RTC TEST      ", rtc_enter_proc, menus, 0x00 },
};



void test_init (void)
{
	test_menu = menus;
	test_start ();	
}


