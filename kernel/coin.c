
#include <freewpc.h>

//#define FREE_ONLY

#define MAX_CREDITS 30

uint8_t free_play;
uint8_t credit_count;
uint8_t unit_count;

void credits_render (void)
{
#ifdef FREE_ONLY
	sprintf ("FREE ONLY");
#else
	if (free_play)
		sprintf ("FREE PLAY");
	else
		sprintf ("CREDITS %d", credit_count);
#endif
}


void credits_draw (void)
{
	dmd_alloc_low_clean ();
	dmd_alloc_high_clean ();

	credits_render ();
	font_render_string_center (&font_5x5, 64, 10, sprintf_buffer);
	dmd_copy_low_to_high ();

	if (!has_credits_p ())
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


void lamp_start_update (void)
{
	if (has_credits_p)
		lamp_flash_on (MACHINE_START_LAMP);
	else
		lamp_flash_off (MACHINE_START_LAMP);
}


void add_credit (void)
{
	if (credit_count < MAX_CREDITS)
	{
		sound_send (SND_THUD);
		deff_restart (DEFF_CREDITS);
#ifndef FREE_ONLY
		credit_count++;
		lamp_start_update ();
#endif
	}
}


bool has_credits_p (void)
{
#ifndef FREE_ONLY
	return (credit_count > 0);
#else
	return (TRUE);
#endif
}


void remove_credit (void)
{
#ifndef FREE_ONLY
	if (credit_count > 0)
	{
		credit_count--;
		lamp_start_update ();
	}
#endif
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

void sw_left_coin_handler (void) __taskentry__
{
	do_coin (0);
	task_exit ();
}

void sw_center_coin_handler (void) __taskentry__
{
	do_coin (1);
	task_exit ();
}

void sw_right_coin_handler (void) __taskentry__
{
	do_coin (2);
	task_exit ();
}

void sw_fourth_coin_handler (void) __taskentry__
{
	do_coin (3);
	task_exit ();
}


DECLARE_SWITCH_DRIVER (sw_left_coin)
{
	.fn = sw_left_coin_handler,
};


DECLARE_SWITCH_DRIVER (sw_center_coin)
{
	.fn = sw_center_coin_handler,
};


DECLARE_SWITCH_DRIVER (sw_right_coin)
{
	.fn = sw_right_coin_handler,
};


DECLARE_SWITCH_DRIVER (sw_fourth_coin)
{
	.fn = sw_fourth_coin_handler,
};


void coin_init (void)
{
	credit_count = 0;
	unit_count = 0;
#ifdef FREE_ONLY
	free_play = TRUE;
#else
	free_play = FALSE;
#endif
	lamp_start_update ();
}

