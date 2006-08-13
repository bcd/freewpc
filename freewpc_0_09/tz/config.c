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

#include <freewpc.h>

/** Filename: mach/config.c
 * Machine-specific functions.  These are all callbacks
 * from the main kernel code to handle various events
 * in game-specific ways.
 */


/** An array in which each 1 bit represents an opto switch
 * and 0 represents a normal switch.  This is used for
 * determining the logical level of a switch (open/closed)
 * rather than its actual level.
 *
 * The first byte is for the dedicated switches, next is
 * column 1, etc. to column 8.
 */
const uint8_t mach_opto_mask[] = {
	0x00, 0x00, 0x28, 0x0, 0x0, 0x10, 0x0, 0x3F, 0x5D, 0xFF,
};

#if 0

#ifndef SW_11_OPTO_P
#define SW_11_OPTO_P 0
#endif
etc.

const U8 mach_opto_mask[] = {
	0x00,
	SW_11_OPTO_P+ (SW_12_OPTO_P << 1) +
		(SW_13_OPTO_P << 2) + (SW_14_OPTO_P << 3) +
		(SW_15_OPTO_P << 4) + (SW_16_OPTO_P << 5) +
		(SW_17_OPTO_P << 6) + (SW_18_OPTO_P << 7),
	SW_21_OPTO_P+ (SW_22_OPTO_P << 1) +
		(SW_23_OPTO_P << 2) + (SW_24_OPTO_P << 3) +
		(SW_25_OPTO_P << 4) + (SW_26_OPTO_P << 5) +
		(SW_27_OPTO_P << 6) + (SW_28_OPTO_P << 7),
	SW_31_OPTO_P+ (SW_32_OPTO_P << 1) +
		(SW_33_OPTO_P << 2) + (SW_34_OPTO_P << 3) +
		(SW_35_OPTO_P << 4) + (SW_36_OPTO_P << 5) +
		(SW_37_OPTO_P << 6) + (SW_38_OPTO_P << 7),
	SW_41_OPTO_P+ (SW_42_OPTO_P << 1) +
		(SW_43_OPTO_P << 2) + (SW_44_OPTO_P << 3) +
		(SW_45_OPTO_P << 4) + (SW_46_OPTO_P << 5) +
		(SW_47_OPTO_P << 6) + (SW_48_OPTO_P << 7),
	SW_51_OPTO_P+ (SW_52_OPTO_P << 1) +
		(SW_53_OPTO_P << 2) + (SW_54_OPTO_P << 3) +
		(SW_55_OPTO_P << 4) + (SW_56_OPTO_P << 5) +
		(SW_57_OPTO_P << 6) + (SW_58_OPTO_P << 7),
	SW_61_OPTO_P+ (SW_62_OPTO_P << 1) +
		(SW_63_OPTO_P << 2) + (SW_64_OPTO_P << 3) +
		(SW_65_OPTO_P << 4) + (SW_66_OPTO_P << 5) +
		(SW_67_OPTO_P << 6) + (SW_68_OPTO_P << 7),
	SW_71_OPTO_P+ (SW_72_OPTO_P << 1) +
		(SW_73_OPTO_P << 2) + (SW_74_OPTO_P << 3) +
		(SW_75_OPTO_P << 4) + (SW_76_OPTO_P << 5) +
		(SW_77_OPTO_P << 6) + (SW_78_OPTO_P << 7),
	SW_81_OPTO_P+ (SW_82_OPTO_P << 1) +
		(SW_83_OPTO_P << 2) + (SW_84_OPTO_P << 3) +
		(SW_85_OPTO_P << 4) + (SW_86_OPTO_P << 5) +
		(SW_87_OPTO_P << 6) + (SW_88_OPTO_P << 7),
	0xFF
};
#endif

/** An array in which each 0 bit indicates a 'normal'
 * playfield switch where the switch is serviced only
 * when it transitions from inactive->active.  When set
 * to a 1, the switch is also serviced on active->inactive
 * transitions.  These are called edge switches since
 * they 'trigger' on any edge transition.
 *
 * At a minimum, container switches need to be declared
 * as edge switches, since they must be handled whenever
 * they change state (the device count goes up or down).
 */
const uint8_t mach_edge_switches[] = {
	0x00, 0x70, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x98, 0x00,
};


#if 0

#ifndef SW_11_EDGE_P
#define SW_11_EDGE_P 0
#endif
etc.

const U8 mach_edge_switches[] = {
	0x00,
	SW_11_EDGE_P+ (SW_12_EDGE_P << 1) +
		(SW_13_EDGE_P << 2) + (SW_14_EDGE_P << 3) +
		(SW_15_EDGE_P << 4) + (SW_16_EDGE_P << 5) +
		(SW_17_EDGE_P << 6) + (SW_18_EDGE_P << 7),
	SW_21_EDGE_P+ (SW_22_EDGE_P << 1) +
		(SW_23_EDGE_P << 2) + (SW_24_EDGE_P << 3) +
		(SW_25_EDGE_P << 4) + (SW_26_EDGE_P << 5) +
		(SW_27_EDGE_P << 6) + (SW_28_EDGE_P << 7),
	SW_31_EDGE_P+ (SW_32_EDGE_P << 1) +
		(SW_33_EDGE_P << 2) + (SW_34_EDGE_P << 3) +
		(SW_35_EDGE_P << 4) + (SW_36_EDGE_P << 5) +
		(SW_37_EDGE_P << 6) + (SW_38_EDGE_P << 7),
	SW_41_EDGE_P+ (SW_42_EDGE_P << 1) +
		(SW_43_EDGE_P << 2) + (SW_44_EDGE_P << 3) +
		(SW_45_EDGE_P << 4) + (SW_46_EDGE_P << 5) +
		(SW_47_EDGE_P << 6) + (SW_48_EDGE_P << 7),
	SW_51_EDGE_P+ (SW_52_EDGE_P << 1) +
		(SW_53_EDGE_P << 2) + (SW_54_EDGE_P << 3) +
		(SW_55_EDGE_P << 4) + (SW_56_EDGE_P << 5) +
		(SW_57_EDGE_P << 6) + (SW_58_EDGE_P << 7),
	SW_61_EDGE_P+ (SW_62_EDGE_P << 1) +
		(SW_63_EDGE_P << 2) + (SW_64_EDGE_P << 3) +
		(SW_65_EDGE_P << 4) + (SW_66_EDGE_P << 5) +
		(SW_67_EDGE_P << 6) + (SW_68_EDGE_P << 7),
	SW_71_EDGE_P+ (SW_72_EDGE_P << 1) +
		(SW_73_EDGE_P << 2) + (SW_74_EDGE_P << 3) +
		(SW_75_EDGE_P << 4) + (SW_76_EDGE_P << 5) +
		(SW_77_EDGE_P << 6) + (SW_78_EDGE_P << 7),
	SW_81_EDGE_P+ (SW_82_EDGE_P << 1) +
		(SW_83_EDGE_P << 2) + (SW_84_EDGE_P << 3) +
		(SW_85_EDGE_P << 4) + (SW_86_EDGE_P << 5) +
		(SW_87_EDGE_P << 6) + (SW_88_EDGE_P << 7),
	0x00
};
#endif

U8 faster_quote_given;

CALLSET_ENTRY (tz, start_ball)
{
	faster_quote_given = 0;
}


CALLSET_ENTRY (tz, add_player)
{
#ifdef CONFIG_TZONE_IP
	if (num_players > 1)
		sound_send (SND_PLAYER_ONE + num_players - 1);
#endif
}


CALLSET_ENTRY (tz, bonus)
{
	task_sleep_sec (1);
	deff_start (DEFF_BONUS);
	leff_start (LEFF_BONUS);
	task_sleep_sec (1);
	while (deff_get_active () == DEFF_BONUS)
		task_sleep (TIME_33MS);
	leff_stop (LEFF_BONUS);
}


CALLSET_ENTRY (tz, tilt)
{
	sound_send (SND_TILT);
	task_sleep_sec (3);
	sound_send (SND_OH_NO);
}


CALLSET_ENTRY (tz, tilt_warning)
{
	sound_send (SND_TILT_WARNING);
}


CALLSET_ENTRY (tz, start_without_credits)
{
	sound_send (SND_GREEEED);
}


CALLSET_ENTRY (tz, timed_game_tick)
{
	if (!in_live_game || in_bonus)
		return;
	switch (timed_game_timer)
	{
		case 10: 
			if (faster_quote_given == 0)
				sound_send (SND_FASTER); 
			faster_quote_given = 1;
			break;
		case 5: sound_send (SND_FIVE); break;
		case 4: sound_send (SND_FOUR); break;
		case 3: sound_send (SND_THREE); break;
		case 2: sound_send (SND_TWO); break;
		case 1: sound_send (SND_ONE); break;
		case 0: callset_invoke (music_update); break;
		default: break;
	}
}


CALLSET_ENTRY (tz, music_update)
{
#ifdef CONFIG_TIMED_GAME
	if (ball_in_play && (timed_game_timer == 0))
		music_change (MUS_ENDGAME);
	else
#endif

	if (flag_test (FLAG_BTTZ_RUNNING))
		music_change (MUS_JACKPOT);

	else if (flag_test (FLAG_MULTIBALL_RUNNING))
		music_change (MUS_MULTIBALL);

	else if (flag_test (FLAG_QUICK_MULTIBALL_RUNNING))
		music_change (MUS_SPIRAL_ROUND);

	else if (!ball_in_play)
		music_change (MUS_MULTIBALL_LIT_PLUNGER);

	else
		music_change (MUS_MULTIBALL_LIT);
}

