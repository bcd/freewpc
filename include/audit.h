/*
 * Copyright 2006-2012 by Brian Dominy <brian@oddchange.com>
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
} time_audit_t;


struct histogram
{
	/* A name for the histogram */
	const char *label;

	/* The number of items in the histogram */
	U8 count;

	/* An array of values which defines the ranges of each
	entry in the histogram.  The Nth entry is an upper bound
	for the Nth range.  The last entry in this array must
	be 0xFFFF. */
	U16 *values;

	/* A pointer to the first audit entry */
	audit_t *audits;

	const char *render;
	const char *render_max;
};


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
	time_audit_t total_game_time; /* done */
	audit_t hist_score[13];
	audit_t hist_game_time[13];
} std_audits_t;


typedef struct
{
MACHINE_FEATURE_AUDIT_MEMBERS
} feature_audits_t;


typedef struct
{
	struct date totals_cleared;
	struct date clock_last_set; /* done */
	struct date audits_cleared; /* done */
	struct date coins_cleared; /* done */
	struct date factory_reset; /* done */
	struct date last_game_start; /* done */
	struct date last_replay; /* done */
	struct date last_hstd_reset;
	struct date champion_reset;
	struct date last_printout; /* done */
	struct date last_service_credit; /* done */
} std_timestamps_t;

extern __nvram__ std_audits_t system_audits;
extern __nvram__ feature_audits_t feature_audits;
extern __nvram__ std_timestamps_t system_timestamps;

void audit_reset (void);
void audit_increment (audit_t *aud);
void audit_add (audit_t *aud, U8 val);
void audit_assign (audit_t *aud, audit_t val);

__test2__ void time_audit_format (time_audit_t *t);
__test2__ void time_audit_clear (time_audit_t *t);
__test2__ bool time_audit_validate (time_audit_t *t);
__test2__ void time_audit_add (time_audit_t *dst, const time_audit_t *src);
__test2__ void time_audit_add_sec (time_audit_t *t, U16 seconds);
__test2__ void time_audit_copy (time_audit_t *dst, const time_audit_t *src);
__test2__ void time_audit_divide (time_audit_t *t, U16 n);
__test2__ void time_audit_format_per_ball (const time_audit_t *t);
__test2__ void time_audit_format_per_credit (const time_audit_t *t);

__test2__ void score_histogram_add (U16);
__test2__ void game_time_histogram_add (U16);
__test2__ void histogram_browser_draw_1 (void);
__test2__ void histogram_browser_init_1 (void);

#endif /* _STDADJ_H */
