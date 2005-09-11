
#include <freewpc.h>

uint8_t credit_count;
uint8_t unit_count;

void credits_draw (void)
{
	dmd_alloc_low_clean ();
	dmd_alloc_high_clean ();

	sprintf ("CREDITS %d", credit_count);
	font_render_string_center (&font_5x5, 64, 10, sprintf_buffer);

	dmd_copy_low_to_high ();

	if (credit_count == 0)
	{
		sprintf ("INSERT COINS");
	}
	else
	{
		sprintf ("PRESS START");
	}
	font_render_string_center (&font_5x5, 64, 20, sprintf_buffer);

	deff_swap_low_high (25, TIME_100MS);
}


void credits_deff (void)
{
	credits_draw ();
	deff_delay_and_exit (TIME_100MS * 10);
}


void add_credit (void)
{
	sound_send (SND_SCROLL);
	deff_restart (DEFF_CREDITS);
	credit_count++;
}


bool has_credits_p (void)
{
	return (credit_count > 0);
}


void remove_credit (void)
{
	if (credit_count > 0)
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
