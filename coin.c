
#include <freewpc.h>


void coin_deff (void) __taskentry__
{
	register int8_t z = 4;
	while (--z > 0)
	{
		dmd_invert_page (dmd_low_buffer);
		task_sleep (TIME_100MS * 2);
	}
	deff_exit ();
}

static void do_coin (uint8_t slot)
{
	sound_send (SND_SCROLL);
	deff_start (DEFF_COIN_INSERT);
	task_exit ();
}

void sw_left_coin (void) __taskentry__
{
	do_coin (0);
}

void sw_center_coin (void) __taskentry__
{
	do_coin (1);
}

void sw_right_coin (void) __taskentry__
{
	do_coin (2);
}

void sw_fourth_coin (void) __taskentry__
{
	do_coin (3);
}

