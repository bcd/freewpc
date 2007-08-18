/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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
#include <test.h>


/** Says whether or not the flipper coils are enabled */
U8 flippers_enabled;


/** Software controlled flipper inputs for Fliptronic games. */
#ifdef MACHINE_FLIPTRONIC
__fastram__ U8 flipper_overrides;
#endif


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


#ifdef MACHINE_FLIPTRONIC


void flipper_override_on (U8 sw_button)
{
}


void flipper_override_off (U8 sw_button)
{
}


/** Perform real-time processing for a single flipper. */
static inline void flipper_service (
	U8 inputs,
	U8 *outputs,
	U8 sw_button,
	U8 sw_eos,
	U8 sol_power,
	U8 sol_hold )
{
	/* TODO : this function is incredibly simplistic for now.
	 * There is no duty cycling, no debouncing, no error handling. */

	if (inputs & sw_button)
	{
		if (inputs & sw_eos)
		{
			*outputs |= sol_hold;
		}
		else
		{
			*outputs |= sol_power;
		}
	}
}


/** Real-time function that services all of the flipper switches and coils.
 * On non-Fliptronic games, the CPU has no visibility to the flippers so
 * this isn't necessary. */
void fliptronic_rtt (void)
{
	register U8 inputs __areg__ = ~wpc_read_flippers () | flipper_overrides;
	U8 outputs = 0;

	if (flippers_enabled)
	{
		flipper_service (inputs, &outputs, WPC_LL_FLIP_SW, WPC_LL_FLIP_EOS, WPC_LL_FLIP_POWER, WPC_LL_FLIP_HOLD);
		flipper_service (inputs, &outputs, WPC_LR_FLIP_SW, WPC_LR_FLIP_EOS, WPC_LR_FLIP_POWER, WPC_LR_FLIP_HOLD);

#ifdef MACHINE_HAS_UPPER_LEFT_FLIPPER
		flipper_service (inputs, &outputs, WPC_UL_FLIP_SW, WPC_UL_FLIP_EOS, WPC_UL_FLIP_POWER, WPC_UL_FLIP_HOLD);
#endif

#ifdef MACHINE_HAS_UPPER_RIGHT_FLIPPER
		flipper_service (inputs, &outputs, WPC_UR_FLIP_SW, WPC_UR_FLIP_EOS, WPC_UR_FLIP_POWER, WPC_UR_FLIP_HOLD);
#endif
	}

	wpc_write_flippers (~outputs);
}

#endif /* MACHINE_FLIPTRONIC */


void flipper_init (void)
{
	flipper_disable ();
}


