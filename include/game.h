#ifndef _GAME_H
#define _GAME_H

#define MAX_PLAYERS 4
#define MAX_BALLS_PER_GAME 3

extern uint8_t in_game;
extern uint8_t in_bonus;
extern uint8_t in_tilt;
extern uint8_t ball_in_play;
extern uint8_t num_players;
extern uint8_t player_up;
extern uint8_t ball_up;
extern uint8_t extra_balls;

void start_ball (void);
void end_game (void);
void end_ball (void);
void start_ball (void);
void add_player (void);
void start_game (void);
void stop_game (void);
int verify_start_ok (void);
void sw_start_button (void) __taskentry__;
void sw_buy_in_button (void) __taskentry__;
void sw_tilt (void) __taskentry__;
void sw_slam_tilt (void) __taskentry__;
void game_init (void);


#endif /* GAME_H */

