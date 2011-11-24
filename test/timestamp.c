/*
 * Copyright 2009, 2010, 2011 by Brian Dominy <brian@oddchange.com>
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


/*
 * This module provides generic functions for dealing with timestamp
 * variables.
 */

#include <freewpc.h>
#include <test.h>

extern const struct area_csum audit_csum_info;


/*
 * Each timestamp is stored as a triple (hours, mins, secs).
 * Define the limits for each of these fields.
 * Minutes and seconds are self-explanatory; hours has an
 * artificial limit to prevent overflow and to keep the
 * display from becoming outrageous.  Also with this limit,
 * two maxed-out timestamps could be added without causing
 * further overflow.
 */
#define MAX_TS_HOUR 1000
#define MAX_TS_MIN  60
#define MAX_TS_SEC  60
#define MAX_SECS_PER_HOUR 3600UL

/**
 * Format a timestamp as text in the common buffer.
 */
void time_audit_format (time_audit_t *t)
{
	sprintf ("%ld:%02d:%02d", t->hr, t->min, t->sec);
}


/**
 * Reset a timestamp to 00:00:00.
 */
void time_audit_clear (time_audit_t *t)
{
	t->hr = t->min = t->sec = 0;
}


/**
 * Verify that a timestamp is sane.
 */
bool time_audit_validate (time_audit_t *t)
{
	if (t->hr >= MAX_TS_HOUR)
		return FALSE;
	if (t->min >= MAX_TS_MIN)
		return FALSE;
	if (t->sec >= MAX_TS_SEC)
		return FALSE;
	return TRUE;
}


/**
 * Normalize a timestamp, so that any minutes and seconds fields which
 * are out of range are corrected, by carrying the excess into the
 * next field.
 */
static void time_audit_normalize (time_audit_t *t)
{
	if (t->sec >= MAX_TS_SEC)
	{
		t->sec -= MAX_TS_SEC;
		t->min++;
	}
	if (t->min >= MAX_TS_MIN)
	{
		t->min -= MAX_TS_MIN;
		t->hr++;
	}
	if (t->hr >= MAX_TS_HOUR)
	{
		t->hr = 9999;
		t->min = 59;
		t->sec = 59;
	}
}


/**
 * Add one timestamp into another.
 */
void time_audit_add (time_audit_t *dst, const time_audit_t *src)
{
	dst->hr += src->hr;
	dst->min += src->min;
	dst->sec += src->sec;
	time_audit_normalize (dst);
}


/**
 * Increment a timestamp audit by the given number of seconds.
 */
void time_audit_add_sec (time_audit_t *t, volatile U16 seconds)
{
	while (seconds >= MAX_SECS_PER_HOUR)
	{
		seconds -= MAX_SECS_PER_HOUR;
		t->hr++;
	}
	while (seconds >= MAX_TS_SEC)
	{
		seconds -= MAX_TS_SEC;
		t->min++;
	}
	t->sec += seconds;
	time_audit_normalize (t);
}


/**
 * Copy one timestamp to another.
 */
void time_audit_copy (time_audit_t *dst, const time_audit_t *src)
{
	dst->hr = src->hr;
	dst->min = src->min;
	dst->sec = src->sec;
}


/**
 * Divide by a timestamp by integer N.
 *
 * This is going to be ugly.
 *
 */
void time_audit_divide (time_audit_t *t, volatile U16 n)
{
	U16 dividend;

	/* Catch divide by zero errors, and just make the result zero. */
	if (n == 0)
	{
		time_audit_clear (t);
		return;
	}

	/* As the timestamp is stored as a sum of three components,
	to divide the entire timestamp is just to divide each of the
	components.  We start from the largest (hour) and work towards
	the smallest (second).  The remainder from each step is
	propagated down into the next smaller field, or discarded at the end.
		To avoid real division on the 6809, we use repeated division. */

	dividend = t->hr;
	t->hr = 0;
	while (dividend >= n)
	{
		dividend -= n;
		t->hr++;
	}

	/* Note: the following multiplication would overflow if dividend > 1091.
	The dividend input is constrained to the range of 0 to n-1, though.
		If n <= 1091, then there is no possibility of overflow here.
		Also, dividend is only greater than n if t->hr > n, which means
	to overflow the timestamp must have a value greater than 1091 hours.
	To avoid this outright, we artifically limit the hours field to 999 hours. */

	dividend = (dividend * MAX_TS_MIN) + t->min;
	t->min = 0;
	while (dividend >= n)
	{
		dividend -= n;
		t->min++;
	}

	dividend = (dividend * MAX_TS_SEC) + t->sec;
	t->sec = 0;
	while (dividend >= n)
	{
		dividend -= n;
		t->sec++;
	}
}

void time_audit_format_per_ball (const time_audit_t *t)
{
	time_audit_t per_ball;
	time_audit_copy (&per_ball, t);
	time_audit_divide (&per_ball, system_audits.balls_played);
	time_audit_format (&per_ball);
}

void time_audit_format_per_credit (const time_audit_t *t)
{
	time_audit_t per_credit;
	time_audit_copy (&per_credit, t);
	time_audit_divide (&per_credit, system_audits.total_plays);
	time_audit_format (&per_credit);
}


/**
 * At the end of each player's game, take the total game time for that
 * player and add it to the total game time audit.
 */
CALLSET_ENTRY (timestamp, end_player)
{
	extern U16 game_time;

	/* TODO - don't do this if the player's game was
	 * aborted early */
	pinio_nvram_unlock ();
	time_audit_add_sec (&system_audits.total_game_time, game_time);
	csum_area_update (&audit_csum_info);
	pinio_nvram_lock ();
}

