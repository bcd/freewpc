
#include <freewpc.h>

uint8_t credit_count;
uint8_t unit_count;


void credit_added_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_alloc_high_clean ();

	seg_write_string (SEG_ADDR(0,1,3), "CREDITS");
	seg_write_uint8 (SEG_ADDR(0,1,11), credit_count);

	dmd_show_low ();
	deff_swap_low_high (16, TIME_100MS);
	deff_delay_and_exit (TIME_100MS * 5);
}


void add_credit (void)
{
	sound_send (SND_SCROLL);
	deff_restart (DEFF_CREDIT_ADDED);
	credit_count++;
}


int has_credits_p (void)
{
	return (credit_count > 0);
}


void remove_credit (void)
{
	credit_count--;
}


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
	add_credit ();
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

void coin_init (void)
{
	credit_count = 0;
	unit_count = 0;
}
