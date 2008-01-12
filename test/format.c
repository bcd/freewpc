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
 * \brief Formatting functions for audit and adjustment types
 *
 * Rendering functions take an adjustment or audit value, and
 * convert it to human-readable form in the print buffer for
 * later display.
 */

#include <freewpc.h>
#include <test.h>

void decimal_render (U8 val) { sprintf ("%d", val); }
void on_off_render (U8 val) { sprintf (val ? "ON" : "OFF"); }
void yes_no_render (U8 val) { sprintf (val ? "YES" : "NO"); }
void clock_style_render (U8 val) { sprintf (val ? "24 HOUR" : "AM/PM"); }
void date_style_render (U8 val) { sprintf (val ? "D/M/Y" : "M/D/Y"); }
void lang_render (U8 val) { sprintf ("ENGLISH"); }
void replay_system_render (U8 val) { sprintf (val ? "AUTO" : "MANUAL"); }


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

void replay_score_render (U8 val)
{
#ifdef MACHINE_REPLAY_CODE_TO_SCORE
	score_t score;
	score_zero (score);
	MACHINE_REPLAY_CODE_TO_SCORE (score, val);
	sprintf_score (score);
#else
	sprintf ("CODE %d", val);
#endif
}


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


void us_dollar_audit (audit_t val)
{
	sprintf ("$%ld.%02d", val / 4, (val % 4) * 25);
}


void currency_audit (audit_t val)
{
	switch (0)
	{
		default:
			us_dollar_audit (val);
			break;
	}
}


void total_earnings_audit (audit_t val __attribute__((unused)))
{
	audit_t total_coins = 0;
	U8 i;
	for (i=0; i < 4; i++)
		total_coins += system_audits.coins_added[i];
	currency_audit (total_coins);
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


void hex4_audit (audit_t val)
{
	sprintf ("0X%04lX", val);
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
		case AUDIT_TYPE_US_DOLLAR:
			us_dollar_audit (val);
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
		case AUDIT_TYPE_HEX4:
			hex4_audit (val);
			break;
		default:
			break;
	}
}


