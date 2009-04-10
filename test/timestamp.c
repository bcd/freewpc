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
void timestamp_divide (timestamp_t *t, U16 n)
{
}

void timestamp_format_per_ball (timestamp_t *t)
{
	timestamp_format (t);
}

void timestamp_format_per_credit (timestamp_t *t)
{
	timestamp_format (t);
}

CALLSET_ENTRY (timestamp, init)
{
	/* TODO - validate instead */
	wpc_nvram_get ();
#if 0
	system_audits.total_game_time.hr = 1;
	system_audits.total_game_time.min = 2;
	system_audits.total_game_time.sec = 3;
#endif
	timestamp_clear (&system_audits.total_game_time);
	csum_area_update (&audit_csum_info);
	wpc_nvram_put ();
}


CALLSET_ENTRY (timestamp, end_player)
{
	extern U16 game_time;
	dbprintf ("%lds for this player.\n", game_time);

	wpc_nvram_get ();
	timestamp_add_sec (&system_audits.total_game_time, game_time);
	csum_area_update (&audit_csum_info);
	wpc_nvram_put ();

	game_time = 0;
}

