
/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * \file
 * \brief Common flipper logic, for switches and solenoids
 */

#include <freewpc.h>

/** TRUE if the flipper coils are enabled */
bool flippers_enabled;

/** Cache of the last values read from the flipper input switches */
U8 flipper_inputs;

/** Cache of the next/last value to be written to the flipper outputs */
U8 flipper_outputs;



void sw_left_flipper_handler (void) 
{
	extern __test__ void test_left_flipper_button (void);

	if (in_test)
		test_left_flipper_button ();
	else if (in_live_game)
		status_report_check ();
#ifdef MACHINE_AMODE_LEFT_FLIPPER_HANDLER
	else if (deff_get_active () == DEFF_AMODE)
	{
		extern void MACHINE_AMODE_LEFT_FLIPPER_HANDLER (void);
		MACHINE_AMODE_LEFT_FLIPPER_HANDLER ();
	}
#endif
}

void sw_right_flipper_handler (void)
{
	extern __test__ void test_right_flipper_button (void);

	if (in_test)
		test_right_flipper_button ();
	else if (in_live_game)
		status_report_check ();
#ifdef MACHINE_AMODE_RIGHT_FLIPPER_HANDLER
	else if (deff_get_active () == DEFF_AMODE)
	{
		extern void MACHINE_AMODE_RIGHT_FLIPPER_HANDLER (void);
		MACHINE_AMODE_RIGHT_FLIPPER_HANDLER ();
	}
#endif
}

void sw_upper_left_flipper_handler (void)
{
}

void sw_upper_right_flipper_handler (void)
{
}


DECLARE_SWITCH_DRIVER (sw_left_flipper) {
	.fn = sw_left_flipper_handler,
	.flags = SW_IN_TEST,
};

DECLARE_SWITCH_DRIVER (sw_right_flipper) {
	.fn = sw_right_flipper_handler,
	.flags = SW_IN_TEST,
};

DECLARE_SWITCH_DRIVER (sw_upper_left_flipper) {
	.fn = sw_upper_left_flipper_handler,
};

DECLARE_SWITCH_DRIVER (sw_upper_right_flipper) {
	.fn = sw_upper_right_flipper_handler,
};


void flipper_enable (void)
{
	flipper_triac_enable ();
	flippers_enabled = TRUE;
}


void flipper_disable (void)
{
	flipper_triac_disable ();
	flippers_enabled = FALSE;
}


inline void flipper_service (
	U8 sw_button,
	U8 sw_eos,
	U8 sol_power,
	U8 sol_hold )
{
	if (flipper_inputs & sw_button)
	{
		if (flipper_inputs & sw_eos)
		{
			flipper_outputs |= sol_hold;
		}
		else
		{
			flipper_outputs |= sol_power;
		}
	}
}


void flipper_rtt (void)
{
	flipper_inputs = ~wpc_read_flippers ();
	flipper_outputs = 0;

	if (flippers_enabled)
	{
		flipper_service (WPC_LL_FLIP_SW, WPC_LL_FLIP_EOS, WPC_LL_FLIP_POWER, WPC_LL_FLIP_HOLD);
		flipper_service (WPC_LR_FLIP_SW, WPC_LR_FLIP_EOS, WPC_LR_FLIP_POWER, WPC_LR_FLIP_HOLD);

#ifdef MACHINE_HAS_UPPER_LEFT_FLIPPER
		flipper_service (WPC_UL_FLIP_SW, WPC_UL_FLIP_EOS, WPC_UL_FLIP_POWER, WPC_UL_FLIP_HOLD);
#endif

#ifdef MACHINE_HAS_UPPER_RIGHT_FLIPPER
		flipper_service (WPC_UR_FLIP_SW, WPC_UR_FLIP_EOS, WPC_UR_FLIP_POWER, WPC_UR_FLIP_HOLD);
#endif
	}

	wpc_write_flippers (~flipper_outputs);
}

void flipper_init (void)
{
	flipper_disable ();
}


