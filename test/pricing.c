/*
 * Copyright 2009, 2010 by Brian Dominy <brian@oddchange.com>
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

struct coin_door
{
	adjval_t collection_text;
	adjval_t base_coin_size;
	adjval_t slot_values[MAX_COIN_SLOTS];
	adjval_t fast_bill_slot;
	adjval_t allow_hundredths;
};


struct pricing_mode
{
	adjval_t coin_units[MAX_COIN_SLOTS];
	adjval_t units_per_credit;
	adjval_t units_per_bonus;
	adjval_t bonus_credits;
	adjval_t min_units;
	adjval_t coin_door_type;
};


const struct coin_door coin_door_table[] =
{
	[COIN_DOOR_25_25_25] = {
		.collection_text = CUR_DOLLAR,
		.base_coin_size = 25,
		.slot_values = { 1, 1, 1, 1 },
		.fast_bill_slot = 0,
		.allow_hundredths = YES,
	},
	[COIN_DOOR_25_100_25] = {
		.collection_text = CUR_DOLLAR,
		.base_coin_size = 25,
		.slot_values = { 1, 4, 1, 4 },
		.fast_bill_slot = 2,
		.allow_hundredths = YES,
	},
	[COIN_DOOR_UK] = {
		.collection_text = CUR_GBP,
		.base_coin_size = 10,
		.slot_values = { 10, 5, 2, 1 },
		.fast_bill_slot = 0,
		.allow_hundredths = YES,
	},
	[COIN_DOOR_EURO] = {
		.collection_text = CUR_EURO,
		.base_coin_size = 50,
		.slot_values = { 1, 2, 1, 2 },
		.fast_bill_slot = 0,
		.allow_hundredths = YES,
	},
};


const struct pricing_mode pricing_mode_table[] =
{
	[PRICE_USA_25CENT] = {
		.coin_units = { 1, 4, 1, 4 },
		.units_per_credit = 1,
		.units_per_bonus = 0,
		.bonus_credits = 0,
		.min_units = 0,
		.coin_door_type = COIN_DOOR_25_100_25,
	},
	[PRICE_USA_50CENT] = {
		.coin_units = { 1, 4, 1, 4 },
		.units_per_credit = 2,
		.units_per_bonus = 0,
		.bonus_credits = 0,
		.min_units = 0,
		.coin_door_type = COIN_DOOR_25_100_25,
	},
	[PRICE_USA_50_75_100] = {
		.coin_units = { 3, 12, 3, 12 },
		.units_per_credit = 4,
		.units_per_bonus = 0,
		.bonus_credits = 0,
		.min_units = 0,
		.coin_door_type = COIN_DOOR_25_100_25,
	},
	[PRICE_UK] = {
		.coin_units = { 20, 10, 4, 2 },
		.units_per_credit = 5,
		.units_per_bonus = 0,
		.bonus_credits = 0,
		.min_units = 0,
		.coin_door_type = COIN_DOOR_UK,
	},
	[PRICE_EURO] = {
		.coin_units = { 1, 2, 1, 2 },
		.units_per_credit = 2,
		.units_per_bonus = 0,
		.bonus_credits = 0,
		.min_units = 0,
		.coin_door_type = COIN_DOOR_EURO,
	},
};



/**
 * Install a new coin door type.
 */
void coin_door_install (adjval_t type)
{
	if (type != COIN_DOOR_CUSTOM)
	{
		const struct coin_door *door = coin_door_table + type;

		pinio_nvram_unlock ();
		price_config.collection_text = door->collection_text;
		memcpy (price_config.slot_values, door->slot_values, MAX_COIN_SLOTS);
		price_config.base_coin_size = door->base_coin_size;
		price_config.fast_bill_slot = door->fast_bill_slot;
		price_config.allow_hundredths = door->allow_hundredths;
		pinio_nvram_lock ();
	}
}


/**
 * Install a new pricing mode.
 */
void pricing_mode_install (adjval_t type)
{
	if (type != PRICE_CUSTOM)
	{
		const struct pricing_mode *mode = pricing_mode_table + type;

		pinio_nvram_unlock ();
		memcpy (price_config.coin_units, mode->coin_units, MAX_COIN_SLOTS);
		price_config.units_per_credit = mode->units_per_credit;
		price_config.units_per_bonus = mode->units_per_bonus;
		price_config.bonus_credits = mode->bonus_credits;
		price_config.min_units = mode->min_units;
		price_config.coin_door_type = mode->coin_door_type;
		pinio_nvram_lock ();
		coin_door_install (price_config.coin_door_type);
	}
}


/**
 * When the pricing mode or coin door type is changed by the user,
 * this requires changing all of the associated adjustments that
 * go with it.
 */
CALLSET_ENTRY (pricing, adjustment_changed)
{
	if (last_adjustment_changed == &price_config.pricing_mode)
	{
		pricing_mode_install (price_config.pricing_mode);
	}
	else if (last_adjustment_changed == &price_config.coin_door_type)
	{
		coin_door_install (price_config.coin_door_type);
	}
}


/* TODO : Do not allow the individual adjustments to be changed
unless the aggregate adjustments are set to CUSTOM. */
CALLSET_BOOL_ENTRY (pricing, adjustment_visible)
{
	return TRUE;
}

