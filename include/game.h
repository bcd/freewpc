/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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

extern __fastram__ U8 in_game;
extern __fastram__ U8 in_bonus;
extern __fastram__ U8 in_tilt;
extern U8 valid_playfield;
extern __nvram__ U8 num_players;
extern U8 player_up;
extern U8 ball_up;
extern U8 timed_game_timer;
extern U8 tournament_mode_enabled;

#define in_live_game		(in_game && !in_tilt)

void start_ball (void);
void end_game (void);
void end_ball (void);
void start_ball (void);
void set_valid_playfield (void);
void try_validate_playfield (U8 swno);
void add_player (void);
void start_game (void);
void stop_game (void);
void match_start (void);
void amode_start (void);

#ifdef CONFIG_TIMED_GAME
void timed_game_extend (U8 secs);
void timed_game_suspend (void);
void timed_game_resume (void);
void timed_game_pause (task_ticks_t delay);
#else
#define timed_game_extend(secs)
#define timed_game_suspend()
#define timed_game_resume()
#define timed_game_pause(delay)
#endif

#endif /* GAME_H */

