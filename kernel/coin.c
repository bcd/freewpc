
#include <freewpc.h>

//#define FREE_ONLY

#define MAX_CREDITS 30

__nvram__ U8 free_play;
__nvram__ U8 credit_count;
__nvram__ U8 unit_count;
__nvram__ U8 units_per_credit;
__nvram__ U8 units_per_coin[4];

void credits_render (void)
{
#ifdef FREE_ONLY
	sprintf ("FREE ONLY");
#else
	if (free_play)
		sprintf ("FREE PLAY");
	else
	{
		if (unit_count != 0)
		{
			if (credit_count == 0)
			{
				sprintf ("%d/%d CREDIT", unit_count, units_per_credit);
			}
			else
			{
				sprintf ("%d %d/%d CREDITS",
					credit_count, unit_count, units_per_credit);
			}
		}
		else
		{
			if (credit_count == 1)
				sprintf ("%d CREDIT", credit_count);
			else
				sprintf ("%d CREDITS", credit_count);
		}
	}
#endif
}


void credits_draw (void)
{
	dmd_alloc_low_clean ();
	dmd_alloc_high ();

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
	if (has_credits_p ())
	{
		if (!in_game)
			lamp_flash_on (MACHINE_START_LAMP);
		else
		{
			lamp_flash_off (MACHINE_START_LAMP);
			lamp_on (MACHINE_START_LAMP);
		}
	}
	else
	{
		lamp_off (MACHINE_START_LAMP);
		lamp_flash_off (MACHINE_START_LAMP);
	}
}


void add_credit (void)
{
	if (credit_count >= MAX_CREDITS)
		return;

#ifndef FREE_ONLY
	wpc_nvram_get ();
	credit_count++;
	wpc_nvram_put ();

	lamp_start_update ();
#endif

#ifdef MACHINE_ADD_CREDIT_SOUND
	sound_send (MACHINE_ADD_CREDIT_SOUND);
#endif

	leff_start (LEFF_FLASH_ALL);
	deff_restart (DEFF_CREDITS);
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
		wpc_nvram_get ();
		credit_count--;
		wpc_nvram_put ();

		lamp_start_update ();
	}
#endif
}


void add_units (int n)
{
	if (credit_count >= MAX_CREDITS)
		return;

	wpc_nvram_get ();
	unit_count += n;
	wpc_nvram_put ();

	if (unit_count >= units_per_credit)
	{
		while (unit_count >= units_per_credit)
		{
			wpc_nvram_get ();
			unit_count -= units_per_credit;
			wpc_nvram_put ();

			add_credit ();
		}
	}
	else
	{
		deff_restart (DEFF_CREDITS);
	}
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
	add_units (units_per_coin[slot]);
}

void sw_left_coin_handler (void)
{
	do_coin (0);
}

void sw_center_coin_handler (void)
{
	do_coin (1);
}

void sw_right_coin_handler (void)
{
	do_coin (2);
}

void sw_fourth_coin_handler (void)
{
	do_coin (3);
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
	/* TODO : this should only be done during a factory reset */
	wpc_nvram_get ();
	credit_count = 0;
	unit_count = 0;
#ifdef FREE_ONLY
	free_play = TRUE;
#else
	free_play = FALSE;
#endif
	units_per_credit = 2;
	units_per_coin[0] = 1;
	units_per_coin[1] = 4;
	units_per_coin[2] = 1;
	units_per_coin[3] = 1;
	wpc_nvram_put ();

	lamp_start_update ();
}

