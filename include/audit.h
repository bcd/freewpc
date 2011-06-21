/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

#ifndef _AUDIT_H
#define _AUDIT_H

/**
 * An audit variable.  Most audits are 16-bit integers stored in
 * non-volatile memory.
 */
typedef U16 audit_t;

/**
 * Some audits need more than 16-bits.  These are called
 * long audits and are stored as 3-byte binary-coded decimal.
 */
typedef U8 long_audit_t[3];


/*
 * A timestamp audit.  The 16-bit variable is not large enough to
 * hold some time durations, so we expand to an hr/min/sec format.
 * Storing them in this way also makes it easier to display them,
 * without requiring more complicated division operations.
 */
typedef struct
{
	U16 hr;
	U8 min;
	U8 sec;
} timestamp_t;


/**
 * The table of standard WPC system audits.
 * Items marked with 'done' are actually being incremented.
 */
typedef struct
{
	audit_t coins_added[4]; /* done */
	audit_t paid_credits; /* done */
	audit_t service_credits; /* done */
	audit_t games_started; /* done */
	audit_t total_plays; /* done */
	audit_t total_free_plays;
	audit_t replays; /* done */
	audit_t specials; /* done */
	audit_t match_credits; /* done */
	audit_t hstd_credits; /* done */
	audit_t tickets_awarded; /* done */
	audit_t extra_balls_awarded; /* done */
	audit_t __unused_total_game_time; /* TODO */
	audit_t minutes_on; /* done */
	audit_t balls_played; /* done */
	audit_t tilts; /* done */
	audit_t left_drains; /* done */
	audit_t right_drains; /* done */
	audit_t center_drains; /* done */
	audit_t power_ups; /* done */
	audit_t slam_tilts; /* done */
	audit_t plumb_bob_tilts; /* done */
	audit_t fatal_errors; /* done */
	audit_t non_fatal_errors; /* done */
	audit_t left_flippers; /* done */
	audit_t right_flippers; /* done */
	audit_t lockup1_addr; /* done */
	audit_t lockup1_pid_lef; /* done */
	audit_t nplayer_games[4]; /* done */
	audit_t exec_lockups; /* done */
	audit_t trough_rescues;
	audit_t chase_balls;
	timestamp_t total_game_time; /* done */
} std_audits_t;


typedef struct
{
MACHINE_FEATURE_AUDIT_MEMBERS
} feature_audits_t;


extern __nvram__ std_audits_t system_audits;
extern __nvram__ feature_audits_t feature_audits;

void audit_reset (void);
void audit_increment (audit_t *aud);
void audit_add (audit_t *aud, U8 val);
void audit_assign (audit_t *aud, audit_t val);

__test2__ void timestamp_format (timestamp_t *t);
__test2__ void timestamp_clear (timestamp_t *t);
__test2__ bool timestamp_validate (timestamp_t *t);
__test2__ void timestamp_add (timestamp_t *dst, const timestamp_t *src);
__test2__ void timestamp_add_sec (timestamp_t *t, U16 seconds);
__test2__ void timestamp_copy (timestamp_t *dst, const timestamp_t *src);
__test2__ void timestamp_divide (timestamp_t *t, U16 n);
__test2__ void timestamp_format_per_ball (const timestamp_t *t);
__test2__ void timestamp_format_per_credit (const timestamp_t *t);

#endif /* _STDADJ_H */
