/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
 * An audit variable.  Audits are 16-bit integers stored in
 * non-volatile memory.  TODO - this isn't large enough.
 * We really need multibyte BCD vars here.
 */
typedef U16 audit_t;

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
	audit_t total_plays;
	audit_t total_free_plays;
	audit_t replays; /* done */
	audit_t specials; /* done */
	audit_t match_credits; /* done */
	audit_t hstd_credits; /* done */
	audit_t tickets_awarded;
	audit_t extra_balls_awarded; /* done */
	audit_t total_game_time;
	audit_t minutes_on; /* done */
	audit_t balls_played; /* done */
	audit_t tilts; /* done */
	audit_t left_drains; /* done */
	audit_t right_drains; /* done */
	audit_t center_drains;
	audit_t power_ups; /* done */
	audit_t slam_tilts; /* done */
	audit_t plumb_bob_tilts; /* done */
	audit_t fatal_errors; /* done */
	audit_t non_fatal_errors; /* done */
	audit_t left_flippers; /* done */
	audit_t right_flippers; /* done */
	audit_t lockup1_addr; /* done */
	audit_t lockup1_pid_lef; /* done */
	audit_t nplayer_games[4];
	audit_t exec_lockups;
	audit_t trough_rescues;
	audit_t chase_balls;
} std_audits_t;

extern __nvram__ std_audits_t system_audits;

void audit_reset (void);
void audit_increment (audit_t *aud);
void audit_add (audit_t *aud, U8 val);
void audit_assign (audit_t *aud, audit_t val);

#endif /* _STDADJ_H */
