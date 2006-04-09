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

#ifndef _GAME_H
#define _GAME_H

#define MAX_PLAYERS 4
#define MAX_BALLS_PER_GAME 3

extern uint8_t in_game;
extern uint8_t in_bonus;
extern uint8_t in_tilt;
extern uint8_t in_test;
extern uint8_t ball_in_play;
extern uint8_t num_players;
extern uint8_t player_up;
extern uint8_t ball_up;
extern uint8_t extra_balls;

#define in_live_game		(in_game && !in_tilt)

void start_ball (void);
void end_game (void);
void end_ball (void);
void start_ball (void);
void mark_ball_in_play (void);
void add_player (void);
void start_game (void);
void stop_game (void);
bool verify_start_ok (void);
void sw_start_button_handler (void) __taskentry__;
void sw_buy_in_button_handler (void) __taskentry__;

void sw_tilt_handler (void) __taskentry__;
void sw_slam_tilt_handler (void) __taskentry__;
void tilt_start_ball (void);

void game_init (void);


#endif /* GAME_H */

