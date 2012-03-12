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
 * \brief Formatting functions for audit and adjustment types
 *
 * Rendering functions take an adjustment or audit value, and
 * convert it to human-readable form in the print buffer for
 * later display.
 */

#include <freewpc.h>
#include <test.h>
#include <replay.h>

void decimal_render (U8 val) { sprintf ("%d", val); }
void hexadecimal_render (U8 val) { sprintf ("%02X", val); }
void on_off_render (U8 val) { sprintf (val ? "ON" : "OFF"); }
void yes_no_render (U8 val) { sprintf (val ? "YES" : "NO"); }
void clock_style_render (U8 val) { sprintf ((val == CLOCK_STYLE_24HOUR) ? "24 HOUR" : "AM/PM"); }
void date_style_render (U8 val) { sprintf ((val == DATE_STYLE_EURO) ? "D/M/Y" : "M/D/Y"); }
void replay_system_render (U8 val) { sprintf (val ? "AUTO" : "MANUAL"); }


void lang_render (U8 val)
{
	switch (val)
	{
		case LANG_US_ENGLISH: sprintf ("AMERICAN"); return;
		case LANG_GERMAN: sprintf ("DEUTSCH"); return;
		case LANG_FRENCH: sprintf ("FRANCAIS"); return;
		case LANG_SPANISH: sprintf ("ESPANOL"); return;
		case LANG_UK_ENGLISH: sprintf ("BRITISH"); return;
	}
}

/* The high score reset counter is stored in units of 250 games. */
void hs_reset_render (U8 val)
{ 
	if (val == 0)
		sprintf ("OFF");
	else
#if defined(__m6809__) && defined(__int16__)
		sprintf ("%d", val * 250);
#else
		sprintf ("%ld", val * 250UL);
#endif
}

void free_award_render (U8 val)
{
	switch (val)
	{
		case FREE_AWARD_OFF: sprintf ("OFF"); return;
		case FREE_AWARD_CREDIT: sprintf ("CREDIT"); return;
		case FREE_AWARD_EB: sprintf ("EXTRA BALL"); return;
		case FREE_AWARD_TICKET: sprintf ("TICKET"); return;
		case FREE_AWARD_POINTS: sprintf ("POINTS"); return;
	}
}

void game_restart_render (U8 val)
{
	switch (val)
	{
		case GAME_RESTART_ALWAYS: sprintf ("ALWAYS"); return;
		case GAME_RESTART_SLOW: sprintf ("SLOW"); return;
		case GAME_RESTART_NEVER: sprintf ("NEVER"); return;
	}
}

void percent_render (U8 val)
{
	if (val == 0)
		sprintf ("OFF");
	else
		sprintf ("%d%%", val);
}

/**
 * Format a replay score adjustment.
 * The adjustment system only allows for at most 256 different settings per
 * adjustment.  For replay scores, 0 means the replay level is OFF, and
 * 1-255 refer to specific replay values.
 */
void replay_score_render (U8 val)
{
	if (val == 0)
	{
		sprintf ("OFF");
	}
	else
	{
		score_t score;
		score_zero (score);
		replay_code_to_score (score, val);
		sprintf_score (score);
	}
}


#ifndef CONFIG_REPLAY_BOOST_BOOLEAN
/**
 * Likewise, format a replay boost value (non-boolean)
 */
void replay_boost_render (U8 val)
{
	if (val == 0)
	{
		sprintf ("OFF");
		return;
	}
	else
	{
		score_t score;
		score_zero (score);
		replay_code_to_boost (score, val);
		sprintf_score (score);
	}
}
#endif

void minutes_render (U8 val)
{
	if (val == 0)
		sprintf ("OFF");
	else
		sprintf ("%d MIN.", val);
}


void brightness_render (U8 val)
{
	switch (val)
	{
		case 4: sprintf ("4.DIMMEST"); break;
		case 5: sprintf ("5.DIM"); break;
		case 6: sprintf ("6.BRIGHT"); break;
		case 7: sprintf ("7.BRIGHTEST"); break;
	}
}


void printer_type_render (U8 val)
{
	switch (val)
	{
		case 0: sprintf ("PARALLEL"); break;
		case 1: sprintf ("SERIAL"); break;
		case 2: sprintf ("ADP"); break;
		case 3: sprintf ("M.DRUCK."); break;
		case 4: sprintf ("NSM"); break;
	}
}


void baud_rate_render (U8 val)
{
	switch (val)
	{
		case 0: sprintf ("300"); break;
		case 1: sprintf ("600"); break;
		case 2: sprintf ("1200"); break;
		case 3: sprintf ("2400"); break;
		case 4: sprintf ("4800"); break;
		case 5: sprintf ("9600"); break;
	}
}


void time_interval_render (U8 val)
{
	U8 ms;

	if (val > TIME_200MS)
	{
		sprintf ("BIG");
		return;
	}

	switch (val)
	{
		default: case 0: ms = 0; break;
		case TIME_33MS: ms = 33; break;
		case TIME_66MS: ms = 66; break;
		case TIME_100MS: ms = 100; break;
		case TIME_133MS: ms = 133; break;
		case TIME_166MS: ms = 166; break;
		case TIME_200MS: ms = 200; break;
	}

	sprintf ("%dMS", ms);
}


void pricing_mode_render (U8 val)
{
	switch (val)
	{
		case PRICE_CUSTOM: sprintf ("CUSTOM"); break;
		case PRICE_USA_25CENT: sprintf ("USA $0.25"); break;
		case PRICE_USA_50CENT: sprintf ("USA $0.50"); break;
		case PRICE_UK: sprintf ("U.K. 4/1["); break;
		case PRICE_EURO: sprintf ("EUROPE 2/<"); break;
		default: sprintf ("MODE %d", val); break;
	}
}


void coin_door_render (U8 val)
{
	switch (val)
	{
		case COIN_DOOR_CUSTOM: sprintf ("CUSTOM"); break;
		case COIN_DOOR_25_25_25: sprintf ("25-25-25"); break;
		case COIN_DOOR_25_100_25: sprintf ("25-100-25"); break;
		case COIN_DOOR_UK: sprintf ("U.K."); break;
		case COIN_DOOR_EURO: sprintf ("EURO"); break;
		default: sprintf ("MODE %d", val); break;
	}
}


void payment_method_render (U8 val)
{
	switch (val)
	{
		case PAY_COIN:
			sprintf ("COIN"); break;
		case PAY_BILL:
			sprintf ("BILL"); break;
		case PAY_TOKEN:
			sprintf ("TOKEN"); break;
		case PAY_CARD:
			sprintf ("CARD"); break;
	}
}


void difficulty_render (U8 val)
{
	switch (val)
	{
		case EXTRA_EASY: sprintf ("EX. EASY"); break;
		case EASY: sprintf ("EASY"); break;
		case MEDIUM: sprintf ("MEDIUM"); break;
		case HARD: sprintf ("HARD"); break;
		case EXTRA_HARD: sprintf ("EX. HARD"); break;
		default: break;
	}
}


void percentage_of_games_audit (audit_t val)
{
	/* Avoid divide-by-zero error */
	if (system_audits.total_plays == 0)
	{
		sprintf ("0%%");
		return;
	}

#ifndef __m6809__
	sprintf ("%d%%", 100 * val / system_audits.total_plays);
#else
	sprintf ("%d%%", 100 * val / system_audits.total_plays);
#endif
}


void integer_audit (audit_t val) 
{ 
	sprintf ("%ld", val);
}


void secs_audit (audit_t val)
{
	U8 mins = 0;
	while (val > 60)
	{
		val -= 60;
		mins++;
	}
	sprintf ("%d:%02d", mins, (U8)val);
}


/**
 * A table of currency / collection text types.
 */
const struct currency_info
{
	/** The sign for the larger unit of the currency */
	const char *sign;

	/* TODO - needed? */
	U8 base_unit;

	/** The number of the smaller units that make up one larger unit */
	U8 units_for_larger;

	/** True if the large unit sign belongs at the beginning, or
	false if at the end after the numbers. */
	bool prefix_sign;
} currency_info_table[] = {
	[CUR_DOLLAR] = { "$", 25, 4, TRUE },
	[CUR_FRANC] = { " FR.", 25, 4, FALSE },
	[CUR_LIRA] = { "L", 25, 4, FALSE },
	[CUR_PESETA] = { "P", 25, 4, FALSE },
	[CUR_YEN] = { "Y", 25, 4, FALSE },
	[CUR_DM] = { "DM", 25, 4, FALSE },
	[CUR_GBP] = { "[", 1, 100, TRUE },
	[CUR_TOKEN] = { "TOK.", 1, 100, FALSE },
	[CUR_EURO] = { "<", 1, 100, TRUE },
};


/**
 * Render an audit value in the given collection text type.
 */
static void specific_currency_audit (audit_t val, U8 type)
{
	const struct currency_info *info;
	U16 large;
	U16 small;
	U8 radix_char;

	/* For sanity, make sure the value is within range */
	if (type > (sizeof (currency_info_table) / sizeof (currency_info_table[0])))
	{
		sprintf ("???");
		return;
	}

	/* The input value is given in terms of the base coin size (part of the
	coindoor definition).  We need to multiply this, then divide by units_for_larger
	(part of the currency definition). */
	info = &currency_info_table[type];
	val *= price_config.base_coin_size;
	large = val / info->units_for_larger;
	small = (val % info->units_for_larger) * info->base_unit;

	if (system_config.euro_digit_sep == YES)
		radix_char = ',';
	else
		radix_char = '.';

	if (info->prefix_sign)
		sprintf ("%s%ld%c%02d", info->sign, large, radix_char, small);
	else
		sprintf ("%ld%c%02d%s", large, radix_char, small, info->sign);
}


/**
 * Render an audit value in the default collection text type.
 */
void currency_audit (audit_t base_units)
{
	specific_currency_audit (base_units, price_config.collection_text);
}


/**
 * Render a collection text type by printing how the value '0'
 * would look.
 */
void collection_text_render (U8 type)
{
	specific_currency_audit (0, type);
}


void total_earnings_audit (audit_t val __attribute__((unused)))
{
	audit_t base_units = 0;
	U8 i;

	/* For each coin slot, multiply the number of coins seen
	times the value of the slot */
	for (i=0; i < MAX_COIN_SLOTS; i++)
		base_units += system_audits.coins_added[i] * price_config.slot_values[i];
	currency_audit (base_units);
}


void average_per_game_audit (audit_t val)
{
	/* Avoid divide-by-zero error */
	if (system_audits.total_plays == 0)
	{
		sprintf ("N/A");
		return;
	}

	sprintf ("%d", val / system_audits.total_plays);
}


void average_per_ball_audit (audit_t val)
{
	/* Avoid divide-by-zero error */
	if (system_audits.balls_played == 0)
	{
		sprintf ("N/A");
		return;
	}

	sprintf ("%d", val / system_audits.balls_played);
}


void render_audit (audit_t val, audit_format_type_t type)
{
	switch (type)
	{
		case AUDIT_TYPE_INT:
			integer_audit (val);
			break;
		case AUDIT_TYPE_GAME_PERCENT:
			percentage_of_games_audit (val);
			break;
		case AUDIT_TYPE_SECS:
			secs_audit (val);
			break;
		case AUDIT_TYPE_CURRENCY:
			currency_audit (val);
			break;
		case AUDIT_TYPE_TOTAL_EARNINGS:
			total_earnings_audit (val);
			break;
		case AUDIT_TYPE_AVG_PER_GAME:
			average_per_game_audit (val);
			break;
		case AUDIT_TYPE_AVG_PER_BALL:
			average_per_ball_audit (val);
			break;
#ifndef CONFIG_NATIVE
	/* Timestamp audits are broken in native mode because we are using
	a 16-bit as a pointer, which only works on the 6809. */
		case AUDIT_TYPE_TIME_PER_BALL:
			time_audit_format_per_ball ((time_audit_t *)val);
			break;
		case AUDIT_TYPE_TIME_PER_CREDIT:
			time_audit_format_per_credit ((time_audit_t *)val);
			break;
#endif
		default:
			break;
	}
}


