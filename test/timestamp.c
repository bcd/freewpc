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
#include <test.h>

/* CALLSET_SECTION (timestamp, __test2__) */

extern const struct area_csum audit_csum_info;


void timestamp_format (timestamp_t *t)
{
	sprintf ("%ld:%02d:%02d", t->hr, t->min, t->sec);
}

void timestamp_clear (timestamp_t *t)
{
	t->hr = t->min = t->sec = 0;
}

void timestamp_validate (timestamp_t *t)
{
}

static void timestamp_normalize (timestamp_t *t)
{
	if (t->sec >= 60)
	{
		t->sec -= 60;
		t->min++;
	}
	if (t->min >= 60)
	{
		t->min -= 60;
		t->hr++;
	}
}

void timestamp_add (timestamp_t *dst, timestamp_t *src)
{
	dst->hr += src->hr;
	dst->min += src->min;
	dst->sec += src->sec;
	timestamp_normalize (dst);
}

void timestamp_add_sec (timestamp_t *t, volatile U16 seconds)
{
	while (seconds >= 3600)
	{
		seconds -= 3600;
		t->hr++;
	}
	while (seconds >= 60)
	{
		seconds -= 60;
		t->min++;
	}
	t->sec += seconds;
	timestamp_normalize (t);
}

void timestamp_copy (timestamp_t *dst, timestamp_t *src)
{
	dst->hr = src->hr;
	dst->min = src->min;
	dst->sec = src->sec;
	timestamp_normalize (dst);
}


/**
 * Divide by a timestamp by integer N.
 *
 * This is going to be ugly.
 *
 */
void timestamp_divide (timestamp_t *t, volatile U16 n)
{
	U16 dividend;

	/* Catch divide by zero errors, and just make the result zero. */
	if (n == 0)
	{
		timestamp_clear (t);
		return;
	}

	dividend = t->hr;
	t->hr = 0;
	while (dividend >= n)
	{
		dividend -= n;
		t->hr++;
	}

	dividend = (dividend * 60) + t->min;
	t->min = 0;
	while (dividend >= n)
	{
		dividend -= n;
		t->min++;
	}

	dividend = (dividend * 60) + t->sec;
	t->sec = 0;
	while (dividend >= n)
	{
		dividend -= n;
		t->sec++;
	}
}

void timestamp_format_per_ball (timestamp_t *t)
{
	timestamp_t per_ball;
	timestamp_copy (&per_ball, t);
	timestamp_divide (&per_ball, system_audits.balls_played);
	timestamp_format (&per_ball);
}

void timestamp_format_per_credit (timestamp_t *t)
{
	timestamp_t per_credit;
	timestamp_copy (&per_credit, t);
	timestamp_divide (&per_credit, system_audits.total_plays);
	timestamp_format (&per_credit);
}

CALLSET_ENTRY (timestamp, init)
{
	/* TODO - validate instead */
	pinio_nvram_unlock ();
	timestamp_clear (&system_audits.total_game_time);
	csum_area_update (&audit_csum_info);
	pinio_nvram_lock ();
}


CALLSET_ENTRY (timestamp, end_player)
{
	extern U16 game_time;

	/* TODO - don't do this if the player's game was
	 * aborted early */
	pinio_nvram_unlock ();
	timestamp_add_sec (&system_audits.total_game_time, game_time);
	csum_area_update (&audit_csum_info);
	pinio_nvram_lock ();

	game_time = 0;
}

