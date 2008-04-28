/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
#include <coin.h>
#include <test.h>
#include <player.h>
#include <highscore.h>
#include <amode.h>

/**
 * \file
 * \brief The main game state machine logic.
 *
 * This module implements the standard state machine of a pinball game.
 * The lifecycle is generally:
 * <ol>
 * <li>Attract mode
 * <li>Start game
 * <li>Start player
 * <li>Start ball
 * <li>End ball
 * <li>End player
 * <li>End game
 * <li>High score entry
 * <li>Match
 * </ol>
 *
 * Bonus is not really a separate state, but is part of endball.
 *
 * There is experimental support here for timed game, aka Safecracker.
 *
 * The main inputs to this module are the start button, for initiating
 * a game, and the end-of-ball condition, which is signalled based on
 * trough entry (when only one ball is in play).
 */

/* TODO : this code isn't important enough to stay in the system page. */

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
__nvram__ U8 num_players;

/** The number of the player that is currently up. */
U8 player_up;

/** The number of the current ball in play */
U8 ball_up;

/** Nonzero if the current ball will automatically end after a certain
period of time.  The value indicates the number of seconds. */
U8 timed_game_timer;

U8 timed_game_suspend_count;

void start_ball (void);


#ifndef MACHINE_CUSTOM_AMODE
/** A default attract mode display effect to be used when the machine
doesn't define one */
void default_amode_deff (void)
{
#if (MACHINE_DMD == 1)
	FSTART_COLOR
		dmd_draw_fif (fif_freewpc_logo);
	FEND

	for (;;)
		task_sleep_sec (5);
#endif
}
#endif



/** Starts the attract mode */
void amode_start (void)
{
	deff_start (DEFF_AMODE);
#ifdef MACHINE_CUSTOM_AMODE
	leff_start (LEFF_AMODE);
#endif
	triac_enable (TRIAC_GI_MASK);
	far_task_recreate_gid (GID_DEVICE_PROBE, device_probe, COMMON_PAGE);
	callset_invoke (amode_start);
}


/** Stops the attract mode */
void amode_stop (void)
{
	deff_stop_all ();
	leff_stop_all ();
	lamp_all_off ();
	music_stop_all ();
	callset_invoke (amode_stop);
}

#ifdef DEBUGGER
/** Dump the game state to the debugger port */
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


/** Handles the end game condition.
 * This is called directly from the trough update function during
 * endball.  It is also called by test mode when it starts up. */
void end_game (void)
{
	if (in_game)
	{
		/* Kill the flippers in case still enabled */
		flipper_disable ();

		/* Stop the long-running score screen */
		deff_stop (DEFF_SCORES);

		/* If in test mode now (i.e. the game was aborted
		by pressing Enter), then skip over the end game effects. */
		if (!in_test)
		{
			high_score_check ();
			match_start ();
			callset_invoke (end_game);
		}

		/* Mark the game as complete */
		in_game = FALSE;
		player_up = 0;
		ball_up = 0;
		in_tilt = FALSE;
		ball_in_play = FALSE;
	}

	leff_stop_all ();
	task_sleep (TIME_33MS); /* not needed? */
	lamp_all_off ();
	deff_stop_all ();

	if (!in_test)
	{
		deff_start (DEFF_GAME_OVER);
		amode_start ();
	}
}


/**
 * Handle end-of-ball.  This is called from the ball device
 * subsystem whenever it detects that the number of balls in play
 * is zero.  Some error checking is done in case this is not
 * really end-of-ball.  Normally though, this function will
 * not return.
 */
void end_ball (void)
{
	/* Abort if not in the middle of a game.  We can get here
	 * in test mode/attract mode also, because the device subsystem
	 * tracks ball counts always.
	 */
	if (!in_game)
		return;

	/*
	 * If ball_in_play never set, then either the ball drained
	 * before touching any playfield switches, or the ball serve
	 * failed and it fell back into the trough.  Return the
	 * ball to the plunger lane in these cases, and don't
	 * count as end-of-ball.
	 */
#ifdef DEVNO_TROUGH
	if (!ball_in_play && !in_tilt)
	{
		device_request_kick (device_entry (DEVNO_TROUGH));
		return;
	}
#endif

	/* Here, we are committed to ending the ball */

	/* Change the running task group, so that we are no longer in
	 * the context of the trough device update. */
	task_setgid (GID_END_BALL);

	/* Notify everybody that wants to know about it */
	callset_invoke (end_ball);

	/* Disable the flippers if enabled by adjustment. */
	if (system_config.no_bonus_flips)
		flipper_disable ();

	/* Incrmeent audit for total number of balls played */
	audit_increment (&system_audits.balls_played);

	/* Stop the ball search monitor */
	ball_search_monitor_stop ();

	/* If the ball was not tilted, start bonus. */
	in_bonus = TRUE;
	if (!in_tilt)
		callset_invoke (bonus);

	/* Clear the tilt flag.  Note, this is not combined
	with the above to handle tilt while bonus is running. */
	if (in_tilt)
	{
		/* Wait for tilt bob to settle */
		while (free_timer_test (TIM_IGNORE_TILT))
			task_sleep (TIME_100MS);

		/* Cancel the tilt effects */
#ifdef DEFF_TILT
		deff_stop (DEFF_TILT);
#endif
#ifdef LEFF_TILT
		leff_stop (LEFF_TILT);
#endif
		in_tilt = FALSE;
	}

	/* Stop everything running except for this task.
	 * Any task that has protected itself is immune to this.
	 * Normally, this is not necessary. */
	task_kill_all ();
	/* TODO - task_kill_flags (TASK_GAME); */
	in_bonus = FALSE;

	/* If the player has extra balls stacked, then start the
	 * next ball without changing the current player up. */
	if (decrement_extra_balls ())
	{
#ifdef DEFF_SHOOT_AGAIN
		deff_start (DEFF_SHOOT_AGAIN);
#endif
#ifdef LEFF_SHOOT_AGAIN
		leff_start (LEFF_SHOOT_AGAIN);
#endif
		start_ball ();
		goto done;
	}

	/* If this is the last ball of the game for this player,
	 * then offer to buy an extra ball if enabled.  Also,
	 * if 1-coin buyin is enabled, offer this too. */
	if (ball_up == system_config.balls_per_game)
	{
		if (system_config.buy_extra_ball == YES)
		{
			SECTION_VOIDCALL (__common__, buyin_offer);
		}

		if (price_config.one_coin_buyin == YES)
		{
			SECTION_VOIDCALL (__common__, onecoin_buyin_offer);
		}
	}

	/* Advance to the next player in a multiplayer game.
	 * Save and restore the local player data. */
	if (num_players > 1)
	{
		player_save ();
		player_up++;

		if (player_up <= num_players)
		{
			player_restore ();
			start_ball ();
			goto done;
		}
		else
		{
			player_up = 1;
			player_restore ();
		}
	}

	/* If all players have had a turn, then increment the
	 * current ball number.
	 * In timed game, this step is skipped, as the game is
	 * automatically over at the end of the "first ball".
	 */
	if (config_timed_game == OFF)
	{
		ball_up++;
		if (ball_up <= system_config.balls_per_game)
		{
			start_ball ();
			goto done;
		}
	}

	/* After the max balls per game have been played, go into
	 * end game */
	end_game ();

done:
#ifdef DEBUGGER
	/* Dump the game state */
	dump_game ();
#endif

	/* End the endball task */
	task_exit ();
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
		score_update_request ();
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


/** Handle start-of-ball. */
void start_ball (void)
{
	in_tilt = FALSE;
	ball_in_play = FALSE;

	/* Since lamp effects from previous balls could have been killed,
	ensure that no lamps for leffs are allocated, causing incorrect
	lamp matrix draw.  Do this early, before the start_ball hook is
	invoked which might want to start up a leff. */
	leff_stop_all ();

	if (ball_up == 1)
	{
		callset_invoke (start_player);
		task_yield ();
	}

	callset_invoke (start_ball);
	callset_invoke (update_lamps);

	/* Reset the pointer to the current player's score */
	current_score = scores[player_up - 1];

	/* Enable the game scores on the display.  The first deff started
	 * is low in priority and is shown whenever there is nothing else
	 * going on.  The second deff runs briefly at high priority, to
	 * ensure that the scores are shown at least briefly at the start of
	 * ball (e.g., in case a skill shot deff gets started).
	 *
	 * If this is the final ball for the player, then
	 * display the 'goal', i.e. replay or extra ball target score;
	 * or the next high score level.
	 */
	deff_restart (DEFF_SCORES);
	deff_start (DEFF_SCORES_IMPORTANT);
	/* TODO : start a timer to a reminder to plunge the ball */
	if (ball_up == system_config.balls_per_game)
	{
		deff_start (DEFF_SCORE_GOAL);
		/* Chalk game played audits on the final ball */
		audit_increment (&system_audits.total_plays);
	}

	/* Serve a ball to the plunger, by requesting a kick from the
	 * trough device.  However, if a ball is detected in the plunger lane
	 * for whatever reason, then don't kick a new ball, just use the
	 * one that is there.  In that case, need to increment live ball count
	 * manually. */
#if defined(DEVNO_TROUGH) && defined(MACHINE_SHOOTER_SWITCH)
	if (!switch_poll_logical (MACHINE_SHOOTER_SWITCH))
	{
		device_request_kick (device_entry (DEVNO_TROUGH));
	}
	else
	{
		device_add_live ();
	}
#endif

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


/** Called when the ball is marked as 'in play'.  This happens on
most, but not all, playfield switch closures. */
void mark_ball_in_play (void)
{
	if (in_game && !ball_in_play)
	{
		ball_in_play = TRUE;		
		callset_invoke (ball_in_play);
	}
}


/** Adds an additional player to the current game in progress */
void add_player (void)
{
	remove_credit ();
	wpc_nvram_get ();
	num_players++;
	wpc_nvram_put ();
	callset_invoke (add_player);

	/* Acknowledge the new player by showing the scores briefly */
	deff_start (DEFF_SCORES_IMPORTANT);
	score_update_request ();
}


/** Starts a new game */
void start_game (void)
{
	if (!in_game)
	{
		audit_increment (&system_audits.games_started);
		in_game = TRUE;
		in_bonus = FALSE;
		in_tilt = FALSE;
		wpc_nvram_get ();
		num_players = 0;
		wpc_nvram_put ();
		scores_reset ();
		high_score_reset_check ();
	
		add_player ();
		player_up = 1;
		ball_up = 1;
	
		amode_stop ();
		callset_invoke (start_game);
		task_yield (); /* start_game can take awhile */

		/* Note: explicitly call this out last, after all other events
		for start_game have been handled */
		player_start_game ();
		start_ball ();
	}
}


/**
 * stop_game is called whenever a game is restarted.  It is
 * functionally equivalent to end_game aside from normal end
 * game features like match, high score check, etc.
 */
void stop_game (void)
{
	in_game = FALSE;
	in_bonus = FALSE;
	in_tilt = FALSE;
	deff_stop_all ();
	leff_stop_all ();
}


/** Perform final checks before allowing a game to start. */
bool verify_start_ok (void)
{
#ifndef DEVNO_TROUGH
	return FALSE;
#endif

	/* check enough credits */
	if (!has_credits_p ())
		return FALSE;

	/* check ball devices stable */
	if (!in_game && !device_check_start_ok ())
		return FALSE; 

	return TRUE;
}


CALLSET_ENTRY (game, sw_start_button)
{
	/* If in test mode, let test handle it completely. */
	if (in_test)
		return;

#ifdef MACHINE_START_SWITCH
	/* If not enough credits, inform the player.
	 * Also call machine hook, e.g. to make a sound.
	 * But don't do this during a game. */
	if (!in_game && !has_credits_p ())
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
	else if (num_players < system_config.max_players)
	{
		/* A game is already in progress.  If still at
		 * ball 1, and we haven't reached the maximum number
		 * of players, we can add a new player. */
		if (ball_up == 1)
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
					task_sleep_sec (1);
					if (!switch_poll_logical (MACHINE_START_SWITCH))
						break;
					task_sleep_sec (1);
					if (!switch_poll_logical (MACHINE_START_SWITCH))
						break;
					/* fallthru if start button held down */

				case GAME_RESTART_ALWAYS:
					stop_game ();
					start_game ();
					break;

				default:
				case GAME_RESTART_NEVER:
					break;
			}
		}
	}

#ifdef DEBUGGER
	dump_game ();
#endif
#endif /* MACHINE_START_SWITCH */
}


/** Initialize the game subsystem.  */
CALLSET_ENTRY (game, init)
{
	/* Make sure this value is sane */
	if ((num_players == 0) || (num_players > MAX_PLAYERS))
	{
		wpc_nvram_get ();
		num_players = 1;
		wpc_nvram_put ();
	}
	in_game = FALSE;
	in_bonus = FALSE;
	in_tilt = FALSE;
	ball_in_play = FALSE;
	player_up = 0;
}

