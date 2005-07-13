
#include <freewpc.h>

#define MAX_PLAYERS 4
#define MAX_BALLS_PER_GAME 3

uint8_t in_game;
uint8_t in_bonus;
uint8_t in_tilt;
uint8_t ball_in_play;
uint8_t num_players;
uint8_t player_up;
uint8_t ball_up;
uint8_t extra_balls;

void start_ball (void);


void end_game (void)
{
	in_game = 0;
	// check high scores
	// do match sequence
	// return to attract mode
}

void end_ball (void)
{
	if (extra_balls > 0)
	{
		extra_balls--;
		start_ball ();
		return;
	}

	player_up++;
	if (player_up <= num_players)
	{
		start_ball ();
		return;
	}

	player_up = 1;
	ball_up++;
	if (ball_up <= MAX_BALLS_PER_GAME)
	{
		start_ball ();
		return;
	}

	end_game ();
}

void start_ball (void)
{
}

void add_player (void)
{
	num_players++;
}

void start_game (void)
{
	in_game = TRUE;
	in_bonus = FALSE;
	in_tilt = FALSE;
	num_players = 0;

	add_player ();
	player_up = 1;
	ball_up = 1;
	extra_balls = 0;
}

void stop_game (void)
{
}

int verify_start_ok (void)
{
	// check enough credits
	return (has_credits_p ());

	// check balls stable
	// check game not already in progress
}

void sw_start_button (void) __taskentry__
{
	extern void test_start (void);
	test_start ();
	task_exit ();

	if (!in_game)
	{
		if (verify_start_ok ())
		{
			start_game ();
		}
	}
	else
	{
		if (ball_up == 1)
		{
			if (num_players < MAX_PLAYERS)
				add_player ();
		}
		else if (verify_start_ok ())
		{
			stop_game ();
			start_game ();
		}
	}
	task_exit ();
}

void sw_buy_in_button (void) __taskentry__
{
	task_exit ();
}


void sw_tilt (void) __taskentry__
{
	task_exit ();
}


void sw_slam_tilt (void) __taskentry__
{
	task_exit ();
}


void game_init (void)
{
	num_players = 1;
	in_game = FALSE;
	in_bonus = FALSE;
	in_tilt = FALSE;
}


