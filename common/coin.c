/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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
 * \brief Coin switch handlers
 *
 * This module processes coin switches to increment the number of credits
 * available.  It also contains functions for displaying credits to the
 * player in the right format, and for updating the start button lamp
 * to indicate that a game can be started.
 */

#include <freewpc.h>
#include <coin.h>
#include <diag.h>


__nvram__ struct coin_state coin_state;


void coin_reset (void)
{
	coin_state.units = 0;
	coin_state.total_units = 0;
	coin_state.credits = 0;
}


const struct area_csum coin_csum_info = {
	.type = FT_COIN,
	.version = 1,
	.area = (U8 *)&coin_state,
	.length = sizeof (coin_state),
	.reset = coin_reset,
};

extern __common__ void buyin_coin_insert (void);



/** Update the start button lamp.  It will flash when a new game
 * can be started, or be solid on during a game.   It will be
 * off only when there are no credits. */
void lamp_start_update (void)
{
#ifdef MACHINE_START_LAMP
	if (!in_test && has_credits_p ())
	{
		if (!in_game)
			lamp_tristate_flash (MACHINE_START_LAMP);
		else
			lamp_tristate_on (MACHINE_START_LAMP);
	}
	else
		lamp_tristate_off (MACHINE_START_LAMP);
#endif
}


/**
 * Show the current number of credits.
 * Do not show during end game effects, though.
 */
void announce_credits (void)
{
	if (in_test)
		return;
#ifdef DEFF_CREDITS
	deff_restart (DEFF_CREDITS);
#endif
}


/** Increment the credit count by 1. */
void add_credit (void)
{
	if (coin_state.credits >= price_config.max_credits)
		return;

#ifndef FREE_ONLY
	pinio_nvram_unlock ();
	coin_state.credits++;
	csum_area_update (&coin_csum_info);
	pinio_nvram_lock ();
#endif

#ifdef MACHINE_ADD_CREDIT_SOUND
	sound_send (MACHINE_ADD_CREDIT_SOUND);
#endif
#ifdef LEFF_FLASH_ALL
	leff_start (LEFF_FLASH_ALL);
#endif
	announce_credits ();
}


/** Returns true if there are credits available.  This also returns
true if the game is in free play mode. */
bool has_credits_p (void)
{
	csum_area_check (&coin_csum_info);
#ifndef FREE_ONLY
	if (price_config.free_play)
		return (TRUE);
	else
		return (coin_state.credits > 0);
#else
	return (TRUE);
#endif
}


/** Decrement the credit count by 1. */
void remove_credit (void)
{
#ifndef FREE_ONLY
	csum_area_check (&coin_csum_info);
	if (coin_state.credits > 0)
	{
		pinio_nvram_unlock ();
		coin_state.credits--;
		csum_area_update (&coin_csum_info);
		pinio_nvram_lock ();
	}
#endif
}


/** Increment the units counter for a particular slot. */
void add_units (U8 n)
{
	csum_area_check (&coin_csum_info);
	if (coin_state.credits >= price_config.max_credits)
		return;

	/* Add credits based on units/bonus */
	pinio_nvram_unlock ();
	coin_state.units += n;
	if (price_config.units_per_bonus > 0)
	{
		coin_state.total_units += n;
		while (coin_state.total_units >= price_config.units_per_bonus)
		{
			coin_state.total_units -= price_config.units_per_bonus;
			coin_state.credits += price_config.bonus_credits;
		}
	}
	pinio_nvram_lock ();

	/* Check for enough units to convert to a credit */
	if (coin_state.units >= price_config.units_per_credit)
	{
		while (coin_state.units >= price_config.units_per_credit)
		{
			nvram_subtract (coin_state.units, price_config.units_per_credit);
			add_credit ();
			audit_increment (&system_audits.paid_credits);
		}
		callset_invoke (add_credits);
	}
	else
	{
#ifdef MACHINE_ADD_COIN_SOUND
		sound_send (MACHINE_ADD_COIN_SOUND);
#endif
		callset_invoke (add_partial_credits);
		announce_credits ();
	}
	csum_area_update (&coin_csum_info);
}


/** Handle the Nth coin slot switch. */
static void do_coin (U8 slot)
{
	/* Ignore coins after slam tilt */
	if (nonball_event_did_follow (sw_slam_tilt, sw_coin))
		return;

	add_units (price_config.coin_units[slot]);
	audit_increment (&system_audits.coins_added[slot]);
	buyin_coin_insert ();
}

/* TODO - use more robust drivers for the coin switches to
prevent cheating */

CALLSET_ENTRY (coin, sw_left_coin)
{
	do_coin (0);
}

CALLSET_ENTRY (coin, sw_center_coin)
{
	do_coin (1);
}

CALLSET_ENTRY (coin, sw_right_coin)
{
	do_coin (2);
}

CALLSET_ENTRY (coin, sw_fourth_coin)
{
	do_coin (3);
}


/** Clear the units and credits counters. */
void credits_clear (void)
{
	pinio_nvram_unlock ();
	coin_state.credits = 0;
	coin_state.units = 0;
	coin_state.total_units = 0;
	csum_area_update (&coin_csum_info);
	pinio_nvram_lock ();
}


/** Clear the units (partially credits) counter. */
void units_clear (void)
{
	pinio_nvram_unlock ();
	coin_state.units = 0;
	coin_state.total_units = 0;
	csum_area_update (&coin_csum_info);
	pinio_nvram_lock ();
}


CALLSET_ENTRY (coin, file_register)
{
	file_register (&coin_csum_info);
}

