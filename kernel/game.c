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
#include <test.h>

/**
 * \file
 * \brief The main game state machine logic.
 *
 * This module implements the standard state machine of a pinball game.
 * The lifecycle is generally:
 * 1. Attract mode
 * 2. Start game
 * 3. Start player
 * 4. Start ball
 * 5. End ball
 * 6. End player
 * 7. End game
 * 8. High score entry
 * 9. Match
 *
 * Bonus is not really a separate state, but is part of endball.
 *
 * There is experimental support here for timed game, aka Safecracker.
 *
 * The main inputs to this module are the start button, for initiating
 * a game, and the end_of_ball condition, which is signalled based on
 * trough entry (when only one ball is in play).
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

#ifdef CONFIG_TIMED_GAME
/** Nonzero if the current ball will automatically end after a certain
period of time.  The value indicates the number of seconds. */
U8 timed_game_timer;

U8 timed_game_suspend_count;
#endif

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
	task_recreate_gid (GID_DEVICE_PROBE, device_probe);
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
		in_tilt = 0;

		high_score_check ();
		match_start ();
	
		callset_invoke (end_game);

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


/**
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
	if (!ball_in_play && !in_tilt)
	{
		device_request_kick (device_entry (DEVNO_TROUGH));
		goto done;
	}

	/*
	 * Call the machine hook to verify that end_ball can
	 * proceed.  It may return false if we don't want to 
	 * consider this the end of the ball; in such cases,
	 * it must explicitly put another ball back into play.
	 * (Game code should check for ball saves here.)
	 */
	if (!callset_invoke_boolean (end_ball_check) && !in_tilt)
		goto done;

	/* OK, we're committing to ending the ball now. */
	callset_invoke (end_ball);

	/* First, disable the flippers if enabled by adjustment. */
	if (system_config.no_bonus_flips)
		flipper_disable ();

	/* Incrmeent audit for total number of balls played */
	audit_increment (&system_audits.balls_played);

	/* Stop the ball search monitor */
	ball_search_monitor_stop ();

	/* If the ball was not tilted, start bonus.
	 * The tilt flag is then cleared. */
	if (!in_tilt)
	{
		in_bonus = TRUE;
		callset_invoke (bonus);
		in_bonus = FALSE;
	}
	else
		in_tilt = FALSE;

	/* Stop everything running except for this task.
	 * Any task that has protected itself is immune to this.
	 * Normally, this is not necessary. */
	task_kill_all ();

	/* If the player has extra balls stacked, then start the
	 * next ball without changing the current player up. */
	if (decrement_extra_balls ())
	{
		start_ball ();
		goto done;
	}

	/* If this is the last ball of the game for this player,
	 * then offer to buy an extra ball if enabled */
	if ((ball_up == system_config.balls_per_game) && FALSE)
	{
		buyin_offer ();
	}

	/* Advance to the next player. */
	if (num_players > 1)
	{
		player_save ();
		player_up++;
		player_restore ();

		if (player_up <= num_players)
		{
			start_ball ();
			goto done;
		}
		else
		{
			player_up = 1;
		}
	}

#ifndef CONFIG_TIMED_GAME
	/* If all players have had a turn, then increment the
	 * current ball number.
	 * In timed game, this step is skipped, as the game is
	 * automatically over at the end of the *first ball".
	 */
	ball_up++;
	if (ball_up <= system_config.balls_per_game)
	{
		start_ball ();
		goto done;
	}
#endif

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


#ifdef CONFIG_TIMED_GAME
void timed_game_monitor (void)
{
	while ((timed_game_timer > 0) && !in_bonus && in_game)
	{
		/* Look for conditions in which the game timer should not run. */
		if (!ball_in_play
				|| timer_find_gid (GID_TIMED_GAME_PAUSED)
				|| (switch_poll_logical (MACHINE_SHOOTER_SWITCH) &&
						(live_balls <= 1))
				|| timed_game_suspend_count
				|| kickout_locks
				|| ball_search_timed_out ())
		{
			task_sleep (TIME_500MS);
			continue;
		}

		/* OK, drop the timer by one second. */
		task_sleep_sec (1);
		timed_game_timer--;
		score_change++;
		callset_invoke (timed_game_tick);
	}

	/* Start end game effects.  The ball should drain soon, and the
	 * normal end-of-game logic will kick in. */
	if (in_game && !in_bonus)
	{
#ifdef CONFIG_TIMED_GAME_OVER_DEFF
		deff_restart (CONFIG_TIMED_GAME_OVER_DEFF);
#endif
#ifdef CONFIG_TIMED_GAME_OVER_LEFF
		leff_start (CONFIG_TIMED_GAME_OVER_LEFF);
#endif
#ifdef CONFIG_TIMED_GAME_OVER_SOUND
		sound_send (CONFIG_TIMED_GAME_OVER_SOUND);
#endif

		/* Short grace period after the timer expires */
		task_sleep (TIME_500MS);

	}

	/* OK, the game is going to end soon... Disable the flippers. */
	flipper_disable ();
	task_exit ();
}


void timed_game_extend (U8 secs)
{
	if (timed_game_timer > 0)
	{
		timed_game_timer += secs;
#ifdef CONFIG_TIMED_GAME_MAX
		if (timed_game_timer > CONFIG_TIMED_GAME_MAX)
			timed_game_timer = CONFIG_TIMED_GAME_MAX;
#endif
	}
}


void timed_game_suspend (void)
{
	timed_game_suspend_count = 1;
}


void timed_game_resume (void)
{
	timed_game_suspend_count = 0;
}


void timed_game_pause (task_ticks_t delay)
{
	timer_restart_free (GID_TIMED_GAME_PAUSED, delay);
}

#endif /* CONFIG_TIMED_GAME */


void start_ball (void)
{
	db_puts ("In startball\n");
	in_tilt = FALSE;
	ball_in_play = FALSE;

	if (ball_up == 1)
		callset_invoke (start_player);
	callset_invoke (start_ball);
	lamp_update_all ();

	current_score = scores[player_up - 1];
	deff_restart (DEFF_SCORES);
	device_request_kick (device_entry (DEVNO_TROUGH));
	tilt_start_ball ();
	flipper_enable ();
	triac_enable (TRIAC_GI_MASK);
	ball_search_timeout_set (12);
	ball_search_monitor_start ();
#ifdef CONFIG_TIMED_GAME
	timed_game_timer = CONFIG_TIMED_GAME;
	timed_game_suspend_count = 0;
	task_create_gid1 (GID_TIMED_GAME_MONITOR, timed_game_monitor);
#endif
}

void mark_ball_in_play (void)
{
	if (in_game && !ball_in_play)
	{
		ball_in_play = TRUE;		
		callset_invoke (ball_in_play);
	}
}

void add_player (void)
{
	remove_credit ();
	num_players++;
	callset_invoke (add_player);
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
		callset_invoke (start_game);
	
		player_start_game ();
		start_ball ();
	}
}


/**
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
	if (!has_credits_p ())
		return FALSE;

	// check balls stable
	if (!device_check_start_ok ())
	{
		deff_start (DEFF_LOCATING_BALLS);
		return FALSE; 
	}

	return TRUE;
}


/**
 * Handle the start button.
 */
void sw_start_button_handler (void) __taskentry__
{
	/* If in test mode, let test handle it completely. */
	if (in_test)
	{
		test_start_button ();
		return;
	}

	/* If not enough credits, inform the player.
	 * Also call machine hook, e.g. to make a sound. */
	if (!has_credits_p ())
	{
		deff_start (DEFF_CREDITS);
		callset_invoke (start_without_credits);
		return;
	}

	/* See if a game is already in progress. */
	if (!in_game)
	{
		/* Nope, we might be able to start a new game.
		 * Go through some more checks first. */
		if (verify_start_ok ())
		{
			/* OK, we can start a game. */
			start_game ();
		}
		else
		{
			/* For some reason, game couldn't be started...
			 * verify_start_ok() should display an error to the
			 * player if possible, depending on what's wrong. */
		}
	}
	else
	{
		/* A game is already in progress.  If still at
		 * ball 1, and we haven't reached the maximum number
		 * of players, we can add a new player. */
		if ((ball_up == 1) && (num_players < system_config.max_players))
		{
			add_player ();
		}

		/* Nope, can't add a player.  Treat this as a
		 * request to restart a new game. */
		else if (verify_start_ok ())
		{
			switch (system_config.game_restart)
			{
				case GAME_RESTART_SLOW:
					/* TODO */
					break;
				case GAME_RESTART_ALWAYS:
					stop_game ();
					start_game ();
					break;
				default: case GAME_RESTART_NEVER:
					break;
			}
		}
	}

#ifdef DEBUGGER
	dump_game ();
#endif
}


// #define TEST_INITIALS_ENTRY

/**
 * Handle the extra-ball buy-in button.
 * Not all games have one of these.
 */
void sw_buy_in_button_handler (void) __taskentry__
{
#ifdef TEST_INITIALS_ENTRY
	initials_enter ();
#else
	inspector_buyin_button ();
#endif
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


/** Initialize the game subsystem.  */
void game_init (void)
{
	num_players = 1;
	in_game = FALSE;
	in_bonus = FALSE;
	in_tilt = FALSE;
	ball_in_play = FALSE;
}


