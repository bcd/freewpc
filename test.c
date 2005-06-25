
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


void test_loop (uint16_t unused_arg) __taskentry__
{
	asm ("jmp test_loop");
}


void test_start (void)
{
	sound_send (SND_ENTER);
	test_menu = test_menu->next;
	test_index = test_menu->start_index;
	task_recreate_gid (GID_TEST_LOOP, test_loop, 0);
}


void test_deff_proc (void) __taskentry__ __noreturn__
{
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

void rtc_enter_proc (void) __taskentry__
{
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


