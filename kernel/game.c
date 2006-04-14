
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

#include <freewpc.h>

/**
 * \file
 * \brief The main game state machine logic.
 *
 * This module implements the standard state machine of a pinball game.
 */


/** Nonzero if a game is currently in progress. */
__fastram__ U8 in_game;

/** Nonzero if the current game is currently in bonus count */
__fastram__ U8 in_bonus;

/** Nonzero if the current game is currently in tilt mode */
__fastram__ U8 in_tilt;

/** Nonzero if the current ball is considered 'in play', which
 * means that ball loss is allowed to end the ball.  When zero,
 * ball loss is not allowed and will result in a new ball being
 * delivered to the plunger lane. */
U8 ball_in_play;

/** The number of players in the current game */
U8 num_players;

/** The number of the player that is currently up. */
U8 player_up;

/** The number of the current ball in play */
U8 ball_up;


void start_ball (void);


#ifndef MACHINE_CUSTOM_AMODE
void default_amode_deff (void)
{
	dmd_alloc_low_high ();
	dmd_draw_image2 (freewpc_logo_bits);
	dmd_show2 ();
	for (;;)
		task_sleep_sec (5);
}
#endif


void game_over_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 16, "GAME OVER");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}


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

#ifdef DEBUGGER
void dump_game (void)
{
	extern U8 extra_balls;

	dbprintf ("Game : %d    Bonus: %d    Tilt: %d\n",
		in_game, in_bonus, in_tilt);
	dbprintf ("In Play : %d\n", ball_in_play);
	dbprintf ("Player Up : %d of %d\n", player_up, num_players);
	dbprintf ("Ball : %d    EBs : %d\n", ball_up, extra_balls);
}
#endif


void end_game (void)
{
	if (in_game)
	{
		in_game = 0;
		ball_up = 0;

		high_score_check ();
		/* TODO : do match sequence */
	
		call_hook (end_game);

		/*
		 * Make sure all effects and flippers are killed before
		 * going back to the attract mode.
		 */
		lamp_all_off ();
		flipper_disable ();
		deff_stop_all ();
		leff_stop_all ();
		deff_start (DEFF_GAME_OVER);
		amode_start ();
	}
}


/*
 * Handle end-of-ball.  This is called from the ball device
 * subsystem whenever it detects that the number of balls in play
 * is zero.
 */
void end_ball (void)
{
	/* Abort if not in the middle of a game.  We can get here
	 * in test mode/attract mode also, because the device subsystem
	 * tracks ball counts always.
	 */
	if (!in_game)
		goto done;

	/*
	 * If ball_in_play never set, then either the ball drained
	 * before touching any playfield switches, or the ball serve
	 * failed and it fell back into the trough.  Return the
	 * ball to the plunger lane in these cases.
	 */
	if (!ball_in_play)
	{
		device_request_kick (device_entry (DEV_TROUGH));
		goto done;
	}

	/*
	 * Call the machine hook to verify that end_ball can
	 * proceed.  It may return false if we don't want to 
	 * consider this the end of the ball; in such cases,
	 * it must explicitly put another ball back into play.
	 * (Game code should check for ball saves here.)
	 */
	if (!call_boolean_hook (end_ball))
		goto done;

	/* OK, we're committing to ending the ball now.
	 * First, disable the flippers. */
	flipper_disable ();

	/* If the ball was not tilted, start bonus.
	 * The tilt flag is then cleared. */
	if (!in_tilt)
	{
		in_bonus = TRUE;
		call_hook (bonus);
		in_bonus = FALSE;
	}
	else
		in_tilt = FALSE;

	/* Stop all processes & effects that shouldn't be
	 * running anymore. */
	deff_stop_all ();
	leff_stop_all ();

	/* If the player has extra balls stacked, then start the
	 * next ball without changing the current player up. */
	if (decrement_extra_balls ())
	{
		start_ball ();
		goto done;
	}

	/* Advance to the next player. */
	player_up++;
	if (player_up <= num_players)
	{
		start_ball ();
		goto done;
	}

	/* If all players have had a turn, then increment the
	 * current ball number. */
	player_up = 1;
	ball_up++;
	if (ball_up <= system_config.balls_per_game)
	{
		start_ball ();
		goto done;
	}

	/* After the max balls per game have been played, go into
	 * end game */
	end_game ();

	/* On exit from the function, dump the game stats */
done:
#ifdef DEBUGGER
	dump_game ();
#endif
	return;
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
		audit_increment (&system_audits.games_started);
		in_game = TRUE;
		in_bonus = FALSE;
		in_tilt = FALSE;
		num_players = 0;
		scores_reset ();
		high_score_reset_check ();
	
		add_player ();
		player_up = 1;
		ball_up = 1;
		clear_extra_balls ();
	
		deff_start (DEFF_SCORES);
		amode_stop ();
		call_hook (start_game);
	
		player_start_game ();
		start_ball ();
	}
}

/*
 * stop_game is called whenever a game is restarted.
 */
void stop_game (void)
{
	deff_stop_all ();
	leff_stop_all ();
}


/* Perform final checks before allowing a game to start. */
bool verify_start_ok (void)
{
	// check enough credits
	return (has_credits_p ());

	// check balls stable
	// check game not already in progress
}


/*
 * Handle the start button.
 */
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
			else
			{
				/* Nothing to do if trying to add more players
				 * than the system supports. */
			}
		}
		else if (verify_start_ok ())
		{
			stop_game ();
			start_game ();
		}
	}

#ifdef DEBUGGER
	dump_game ();
#endif
}


/*
 * Handle the extra-ball buy-in button.
 * Not all games have one of these.
 */
void sw_buy_in_button_handler (void) __taskentry__
{
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


/*
 * Initialize the game subsystem.
 */
void game_init (void)
{
	num_players = 1;
	in_game = FALSE;
	in_bonus = FALSE;
	in_tilt = FALSE;
	ball_in_play = FALSE;
}


