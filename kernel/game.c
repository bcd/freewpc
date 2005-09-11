
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

void dump_game (void)
{
	db_puts ("Game : "); db_puti (in_game);
	db_puts ("   Bonus : "); db_puti (in_bonus);
	db_puts ("   Tilt : "); db_puti (in_tilt);
	db_putc ('\n');
	db_puts ("In Play : "); db_puti (ball_in_play); db_putc ('\n');
	db_puts ("Player Up : "); db_puti (player_up);
	db_puts (" of "); db_puti (num_players); db_putc ('\n');
	db_puts ("Ball : "); db_puti (ball_up); db_putc ('\n');
	db_puts ("EBs : "); db_puti (extra_balls); db_putc ('\n');
}

void scores_deff (void) __taskentry__
{
	for (;;)
	{
		dmd_alloc_low_clean ();

		seg_write_string (SEG_ADDR (0,0,0), "PLAYER");
		seg_write_uint8 (SEG_ADDR (0,0,8), player_up);
		seg_write_string (SEG_ADDR (0,1,0), "BALL");
		seg_write_uint8 (SEG_ADDR (0,1,5), ball_up);

		dmd_show_low ();
		task_sleep (TIME_100MS * 5);

#if 0
		dmd_alloc_low_clean ();
		dmd_show_low ();
		task_sleep (TIME_100MS * 2);
#endif
	}
}


void end_game (void)
{
	in_game = 0;
	ball_up = 0;

	// check high scores
	// do match sequence
	// return to attract mode

	deff_stop (DEFF_SCORES);
}

void end_ball (void)
{
	db_puts ("In endball\n");

	if (!in_game)
		goto done;

	if (extra_balls > 0)
	{
		extra_balls--;
		start_ball ();
		goto done;
	}

	player_up++;
	if (player_up <= num_players)
	{
		start_ball ();
		goto done;
	}

	player_up = 1;
	ball_up++;
	if (ball_up <= MAX_BALLS_PER_GAME)
	{
		start_ball ();
		goto done;
	}

	end_game ();

done:
	dump_game ();
}

void start_ball (void)
{
	db_puts ("In startball\n");
	device_request_kick (device_entry (DEV_TROUGH));
}

void add_player (void)
{
	remove_credit ();
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
	start_ball ();
	deff_start (DEFF_SCORES);
}

void stop_game (void)
{
	deff_stop (DEFF_SCORES);
}

bool verify_start_ok (void)
{
	// check enough credits
	return (has_credits_p ());

	// check balls stable
	// check game not already in progress
}

void sw_start_button (void) __taskentry__
{
	extern void test_start_button (void);

	if (0) /* in_test_mode */
	{
		test_start_button ();
		return;
	}

	if (!has_credits_p)
	{
		return;
	}

	if (!in_game)
	{
		if (verify_start_ok ())
		{
			start_game ();
		}
		else
		{
			db_puts ("Can't start game now\n");
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

	dump_game ();
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


