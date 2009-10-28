/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>

#define S_BIT 0x1
#define T_BIT 0x2
#define E_BIT 0x4
#define P_BIT 0x8
#define ALL_BITS (S_BIT+T_BIT+E_BIT+P_BIT)
#define S_LAMP LM_STEP_S
#define T_LAMP LM_STEP_T
#define E_LAMP LM_STEP_E
#define P_LAMP LM_STEP_P

__local__ U8 target_bits;
__local__ U8 step_eb_awards;

void light_step (void)
{
	if (!lamp_flash_test (LM_STEPS_FRENZY))
	{
		lamp_flash_on (LM_STEPS_FRENZY);
	}
	else if (!lamp_flash_test (LM_STEPS_500K))
	{
		lamp_flash_on (LM_STEPS_500K);
	}
	else if (!lamp_flash_test (LM_STEPS_EB) && !step_eb_awards)
	{
		lamp_flash_on (LM_STEPS_EB);
		step_eb_awards++;
	}
}

static void lamp_update_target (U8 bit, U8 lamp)
{
	lamp_on_if (lamp, target_bits & bit);
}

static void handle_unlit_target (const U8 bit)
{
	if (target_bits == ALL_BITS)
	{
		sample_start (SND_TARGET_COMPLETE, SL_2S);
		score (SC_100K);
		target_bits = 0;
	}
	else
	{
		sample_start (SND_TARGET1, SL_500MS);
	}
	effect_update_request ();
}

static void handle_lit_target (const U8 bit)
{
}

static inline void handle_target (const U8 bit)
{
	if (target_bits & bit)
	{
		handle_lit_target (bit);
	}
	else
	{
		target_bits |= bit;
		handle_unlit_target (bit);
	}
}

CALLSET_ENTRY (targets, lamp_update)
{
	lamp_update_target (S_BIT, S_LAMP);
	lamp_update_target (T_BIT, T_LAMP);
	lamp_update_target (E_BIT, E_LAMP);
	lamp_update_target (P_BIT, P_LAMP);
}

CALLSET_ENTRY (targets, sw_step_s)
{
	handle_target (S_BIT);
}

CALLSET_ENTRY (targets, sw_step_t)
{
	handle_target (T_BIT);
}

CALLSET_ENTRY (targets, sw_step_e)
{
	handle_target (E_BIT);
}

CALLSET_ENTRY (targets, sw_step_p)
{
	handle_target (P_BIT);
}

CALLSET_ENTRY (targets, start_player)
{
	target_bits = 0;
	step_eb_awards = 0;
}

