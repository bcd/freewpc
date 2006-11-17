/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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
 * non-volatile memory.
 */
typedef uint16_t audit_t;

/**
 * The table of standard WPC system audits.
 */
typedef struct
{
	audit_t coins_added[4];
	audit_t paid_credits;
	audit_t service_credits;
	audit_t games_started;
	audit_t total_plays;
	audit_t total_free_plays;
	audit_t replays;
	audit_t specials;
	audit_t match_credits;
	audit_t hstd_credits;
	audit_t tickets_awarded;
	audit_t extra_balls_awarded;
	audit_t total_game_time;
	audit_t minutes_on;
	audit_t balls_played;
	audit_t tilts;
	audit_t left_drains;
	audit_t right_drains;
	audit_t center_drains;
	audit_t power_ups;
	audit_t slam_tilts;
	audit_t plumb_bob_tilts;
	audit_t fatal_errors;
	audit_t non_fatal_errors;
	audit_t left_flippers;
	audit_t right_flippers;
} std_audits_t;

extern __nvram__ std_audits_t system_audits;

void audit_reset (void);
void audit_increment (audit_t *aud);
void audit_add (audit_t *aud, U8 val);
void audit_init (void);

#endif /* _STDADJ_H */
