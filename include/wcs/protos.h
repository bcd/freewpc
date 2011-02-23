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

/* wcs/config.c */
void wcs_init(void);
void unused_init(void);
void unused_start_game(void);
void unused_end_game(void);
void unused_start_ball(void);
void wcs_start_game(void);
void wcs_end_game(void);
void wcs_start_ball(void);
bool wcs_end_ball(void);
void wcs_add_player(void);
void wcs_any_pf_switch(void);
void wcs_bonus(void);
void wcs_tilt(void);
void wcs_tilt_warning(void);
void wcs_start_without_credits(void);
void wcs_coin_added(void);
/* wcs/goal_popper.c */
void goal_popper_kick_sound(void);
void goal_popper_enter(device_t *dev);
void goal_popper_kick_attempt(device_t *dev);
void goal_popper_init(void);
/* wcs/tv_popper.c */
void tv_popper_kick_sound(void);
void tv_popper_enter(device_t *dev);
void tv_popper_kick_attempt(device_t *dev);
void tv_popper_init(void);
