
#include <freewpc.h>

U8 in_game;
U8 in_bonus;
U8 in_tilt;
U8 ball_in_play;
U8 num_players;
U8 player_up;
U8 ball_up;
U8 extra_balls;

void start_ball (void);


#ifndef MACHINE_CUSTOM_AMODE
void default_amode_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_alloc_high_clean ();
	font_render_string_center (&font_5x5, 64, 16, "NO ATTRACT MODE");
	dmd_show_low ();
	for (;;)
	{
		dmd_show_other ();
		task_sleep_sec (1);
	}
}
#endif

void amode_start (void)
{
	deff_start (DEFF_AMODE);
#ifdef MACHINE_CUSTOM_AMODE
	leff_start (LEFF_AMODE);
#endif
	lamp_start_update ();
}


void amode_stop (void)
{
	deff_stop (DEFF_AMODE);
#ifdef MACHINE_CUSTOM_AMODE
	leff_stop (LEFF_AMODE);
#endif
	lamp_all_off ();
	music_set (MUS_OFF);
}

void dump_game (void)
{
	dbprintf ("Game : %d    Bonus: %d    Tilt: %d\n",
		in_game, in_bonus, in_tilt);
	dbprintf ("In Play : %d\n", ball_in_play);
	dbprintf ("Player Up : %d of %d\n", player_up, num_players);
	dbprintf ("Ball : %d    EBs : %d\n", ball_up, extra_balls);
}


void end_game (void)
{
	if (in_game)
	{
		in_game = 0;
		ball_up = 0;

		// check high scores
		// do match sequence
		// return to attract mode
	
		call_hook (end_game);
		lamp_all_off ();
		flipper_disable ();
		deff_stop_all ();
		leff_stop_all ();
		amode_start ();
	}
}

void end_ball (void)
{
	if (!in_game)
		goto done;

	if (!ball_in_play)
	{
		device_request_kick (device_entry (DEV_TROUGH));
		goto done;
	}

	if (!call_boolean_hook (end_ball))
		goto done;
	
	flipper_disable ();

	if (!in_tilt)
	{
		in_bonus = TRUE;
		call_hook (bonus);
		in_bonus = FALSE;
	}
	else
		in_tilt = FALSE;

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
	if (ball_up <= system_config.balls_per_game)
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
	in_tilt = FALSE;
	ball_in_play = FALSE;
	call_hook (start_ball);
	player_change ();
	current_score = scores[player_up - 1];
	deff_restart (DEFF_SCORES);
	device_request_kick (device_entry (DEV_TROUGH));
	tilt_start_ball ();
	flipper_enable ();
	triac_enable (TRIAC_GI_MASK);
}

void mark_ball_in_play (void)
{
	if (in_game && !ball_in_play)
	{
		ball_in_play = TRUE;		
		call_hook (ball_in_play);
	}
}

void add_player (void)
{
	remove_credit ();
	num_players++;
	call_hook (add_player);
}

void start_game (void)
{
	if (!in_game)
	{
		in_game = TRUE;
		in_bonus = FALSE;
		in_tilt = FALSE;
		num_players = 0;
		scores_reset ();
	
		add_player ();
		player_up = 1;
		ball_up = 1;
		extra_balls = 0;
	
		deff_start (DEFF_SCORES);
		amode_stop ();
		call_hook (start_game);
	
		player_start_game ();
		start_ball ();
	}
}

void stop_game (void)
{
	deff_stop_all ();
	leff_stop_all ();
}

bool verify_start_ok (void)
{
	// check enough credits
	return (has_credits_p ());

	// check balls stable
	// check game not already in progress
}

void sw_start_button_handler (void) __taskentry__
{
	extern void test_start_button (void);

	if (in_test)
	{
		call_far (TEST_PAGE, test_start_button ());
		return;
	}

	if (!has_credits_p ())
	{
		deff_start (DEFF_CREDITS);
		call_hook (start_without_credits);
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
			if (num_players < system_config.max_players)
			{
				add_player ();
			}
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

void sw_buy_in_button_handler (void) __taskentry__
{
	task_exit ();
}

/*
 * Switches declared by the game module:
 * start & buy-in buttons
 */
DECLARE_SWITCH_DRIVER (sw_start_button)
{
	.fn = sw_start_button_handler,
	.flags = SW_IN_TEST,
};

DECLARE_SWITCH_DRIVER (sw_buyin_button)
{
	.fn = sw_buy_in_button_handler,
};


void game_init (void)
{
	num_players = 1;
	in_game = FALSE;
	in_bonus = FALSE;
	in_tilt = FALSE;
	ball_in_play = FALSE;
}


