/*
 * Copyright 2006, 2008 by Brian Dominy <brian@oddchange.com>
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

/* corvette/config.c */
void corvette_init(void);
void unused_init(void);
void unused_start_game(void);
void unused_end_game(void);
void unused_start_ball(void);
void corvette_start_game(void);
void corvette_end_game(void);
void corvette_start_ball(void);
bool corvette_end_ball(void);
void corvette_add_player(void);
void corvette_any_pf_switch(void);
void corvette_bonus(void);
void corvette_tilt(void);
void corvette_tilt_warning(void);
void corvette_start_without_credits(void);
void corvette_coin_added(void);
