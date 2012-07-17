/*
 * Copyright 2006, 2007, 2008, 2009, 2010, 2011 by Brian Dominy <brian@oddchange.com>
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
#include <search.h>
#include <eb.h>

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

/* Equivalent to (win_top != NULL), but as a byte, this can
 * be tested with a single instruction.
 * IDEA: these two variables could be overlapped into a union. */
__fastram__ enum test_mode in_test;

/** Nonzero if the current ball has reached valid playfield; i.e.
 * ball loss is allowed to end the ball.  When zero,
 * ball loss is not allowed and will result in a new ball being
 * delivered to the plunger lane. */
U8 valid_playfield;

/** The IDs of up to two playfield switches that have activated
 * at ball start and were marked 'novalid'. */
switchnum_t novalid_sw[2];

/** The number of players in the current game */
__permanent__ U8 num_players;

/** The number of the player that is currently up. */
U8 player_up;

/** The number of the current ball in play */
U8 ball_up;

/** Nonzero if the current ball will automatically end after a certain
period of time.  The value indicates the number of seconds. */
U8 timed_game_timer;

U8 timed_game_suspend_count;

void start_ball (void);


/** Starts the attract mode */
void amode_start (void)
{
	deff_start (DEFF_AMODE);
#ifdef MACHINE_CUSTOM_AMODE
	leff_start (LEFF_AMODE);
#endif
#ifdef CONFIG_GI
	gi_enable (PINIO_GI_STRINGS);
#endif
	flipper_disable ();
	far_task_recreate_gid (GID_DEVICE_PROBE, device_probe, COMMON_PAGE);
	task_yield ();
	callset_invoke (amode_start);
}


/** Stops the attract mode */
void amode_stop (void)
{
	deff_stop_all ();
	leff_stop_all ();
	lamp_all_off ();
	effect_update_request ();
	callset_invoke (amode_stop);
}

#ifdef DEBUGGER
/** Dump the game state to the debugger port */
void dump_game (void)
{
	extern U8 extra_balls;

	dbprintf ("Game : %d    Bonus: %d    Tilt: %d\n",
		in_game, in_bonus, in_tilt);
	dbprintf ("Valid : %d\n", valid_playfield);
	dbprintf ("Player Up : %d of %d\n", player_up, num_players);
	dbprintf ("Ball : %d    EBs : %d\n", ball_up, extra_balls);
}
#endif


/** Handles the end game condition.
 * This is called directly from the trough update function during
 * endball.  It is also called by test mode when it starts up. */
static void end_game_task (void)
{
	U8 was_in_game = in_game;

	/* Common stop/end game logic */
	stop_game ();

	/* From here, it is possible to restart a new game and
	interrupt all of the following effects */

	if (was_in_game)
	{
		/* Kill the flippers in case still enabled */
		flipper_disable ();

		/* If in test mode now (i.e. the game was aborted
		by pressing Enter), then skip over the end game effects. */
		if (!in_test)
		{
			deff_start_sync (DEFF_SCORES_IMPORTANT);
			high_score_check ();
			callset_invoke (extra_initials_check);
			match_start ();
			log_event (SEV_INFO, MOD_GAME, EV_STOP, 0);
			callset_invoke (end_game);
		}
	}

	task_sleep (TIME_33MS); /* not needed? */
	lamp_all_off ();

	/* Return to attract mode, unless the game was aborted to
	enter test mode */
	if (!in_test)
	{
		deff_start (DEFF_GAME_OVER);
		amode_start ();
	}
	task_exit ();
}

void end_game (void)
{
	/* To limit stack size, spawn this in a separate
	 * task context.  We are already nested pretty deeply here, and
	 * end game effects will need to sleep to do synchronous deffs.  I've
	 * observed stack overflow here when running the stress test.
	 * Ensure that this task doesn't get killed due to any duration
	 * change - notably entering test mode. */
	task_remove_duration (TASK_DURATION_GAME);
	task_create_gid1 (GID_END_GAME, end_game_task);
	task_sleep (TIME_16MS);
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
	 * If valid_playfield never set, then either the ball drained
	 * before touching any playfield switches, or the ball serve
	 * failed and it fell back into the trough.  Return the
	 * ball to the plunger lane in these cases, and don't
	 * count as end-of-ball.
	 */
	if (!valid_playfield && !in_tilt)
	{
		serve_ball ();
		return;
	}

	/* Here, we are committed to ending the ball.
	Do not sleep from here on out, except while in_bonus = TRUE (below).
	This flag protects the function from being re-entered due to
	spurious switch activity while endball is running. */

	/* Change the running task group, so that we are no longer in
	 * the context of the trough device update. */
	task_setgid (GID_END_BALL);

	/* Notify everybody that wants to know about it */
	callset_invoke (end_ball);

	/* Disable the flippers if enabled by adjustment. */
	if (system_config.no_bonus_flips)
		flipper_disable ();

	/* Increment audit for total number of balls played */
	audit_increment (&system_audits.balls_played);

	/* Stop the ball search monitor */
	ball_search_monitor_stop ();

	/* If the ball was not tilted, start bonus. */
	in_bonus = TRUE;
	music_disable ();
	if (!in_tilt)
		callset_invoke (bonus);
	callset_invoke (bonus_complete);

	/* Stop tasks that should run only until end-of-ball. */
	task_remove_duration (TASK_DURATION_BALL);
	task_duration_expire (TASK_DURATION_BALL);
	in_bonus = FALSE;

	/* If the player has extra balls stacked, then start the
	 * next ball without changing the current player up. */
	if (decrement_extra_balls ())
	{
#ifdef DEFF_SHOOT_AGAIN
		deff_start_sync (DEFF_SHOOT_AGAIN);
#endif
#ifdef LEFF_SHOOT_AGAIN
		leff_start (LEFF_SHOOT_AGAIN);
#endif
		callset_invoke (shoot_again);
		start_ball ();
		goto done;
	}

	/* If this is the last ball of the game for this player,
	 * then offer to buy an extra ball if enabled.  Also
	 * save away the per-player audits. */
	if ((ball_up == system_config.balls_per_game) || config_timed_game)
	{
		if (!callset_invoke_boolean (buyin_offer))
		{
			callset_invoke (buyin_start_ball);
			start_ball ();
			goto done;
		}
		else
		{
			callset_invoke (end_player);
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

	/* Once all players have had a turn, then increment the
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

	/* After the max balls per game have been played, go into end game */
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
	extern __common__ bool ballsave_test_active (void);

	while ((timed_game_timer > 0) && !in_bonus && in_game)
	{
		while (ballsave_test_active ())
			task_sleep (TIME_500MS);
		while (multi_ball_play ())
			task_sleep (TIME_2S);
		timer_pause_second ();
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
		task_sleep_sec (1);
	}

	/* OK, the game is going to end soon... Disable the flippers. */
	flipper_disable ();

	/* Just in case something goes wrong, force endball after
	 * some time */
	task_sleep_sec (20);
	end_ball ();
	task_exit ();
}

#ifndef CONFIG_TIMED_GAME_MAX
#define CONFIG_TIMED_GAME_MAX 240
#endif

void timed_game_extend (U8 secs)
{
	if (config_timed_game && timed_game_timer > 0)
	{
		timed_game_timer += secs;
		if ((timed_game_timer > CONFIG_TIMED_GAME_MAX) || (timed_game_timer < secs))
			timed_game_timer = CONFIG_TIMED_GAME_MAX;
#ifdef DEFF_TIMED_GAME_EXTENDED
			deff_start (DEFF_TIMED_GAME_EXTENDED);
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
	valid_playfield = FALSE;
	music_enable ();
	novalid_sw[0] = novalid_sw[1] = 0;

	/* Since lamp effects from previous balls could have been killed,
	ensure that no lamps for leffs are allocated, causing incorrect
	lamp matrix draw.  Do this early, before the start_ball hook is
	invoked which might want to start up a leff. */
	leff_stop_all ();

	/* On the first ball for each player, generate the
	start_player event, which initializes all player-local variables. */
	if (ball_up == 1 && extra_balls_earned_this_bip == 0)
	{
		callset_invoke (start_player);
		task_yield ();
	}

	/* At the start of ball 2, any partial credits are cleared */
	units_clear ();

	/* Reset the pointer to the current player's score */
	current_score = scores[player_up - 1];

	callset_invoke (start_ball);

	/* Ensure that the scores are shown briefly at the start of
	 * ball, via a high priority display effect.
	 *
	 * If this is the final ball for the player, then also
	 * display the 'goal', i.e. replay or extra ball target score;
	 * or the next high score level.
	 */
	deff_start (DEFF_SCORES_IMPORTANT);
	if ((ball_up == system_config.balls_per_game) || config_timed_game)
	{
		deff_start (DEFF_SCORE_GOAL);

		/* Chalk game played audits at the start of the final ball
		for the first player */
		if (player_up == 1)
		{
			audit_add (&system_audits.total_plays, num_players);
			audit_increment (&system_audits.nplayer_games[num_players-1]);
		}
	}

	/* Serve a ball to the plunger, by requesting a kick from the
	 * trough device.  However, if a ball is detected on the shooter switch
	 * for whatever reason, then don't kick a new ball, just use the
	 * one that is there.  In that case, need to increment live ball count
	 * manually. */
#if defined(MACHINE_SHOOTER_SWITCH)
	if (switch_poll_logical (MACHINE_SHOOTER_SWITCH))
		device_add_live ();
	else
#endif
		serve_ball ();

	flipper_enable ();
#ifdef CONFIG_GI
	gi_enable (PINIO_GI_STRINGS);
#endif
	ball_search_timeout_set (12);
	ball_search_monitor_start ();

	/* If timed game support is built-in and enabled, then
	start a task to monitor the game time. */
#ifdef CONFIG_TIMED_GAME
	if (config_timed_game)
	{
		extern U8 switch_stress_enable;
		timed_game_timer = CONFIG_TIMED_GAME;
		timed_game_suspend_count = 0;
		if (switch_stress_enable == NO)
			task_create_gid1 (GID_TIMED_GAME_MONITOR, timed_game_monitor);
	}
#endif
}


/** Called when the ball is marked as 'in play'.  This happens on
most, but not all, playfield switch closures.  After valid
playfield is set, end-of-ball is allowed to happen.  Prior to
that, end-of-ball just causes a re-serve from the trough. */
void set_valid_playfield (void)
{
	if (in_game && !valid_playfield)
	{
		valid_playfield = TRUE;		
		callset_invoke (valid_playfield);
		effect_update_request ();
	}
}


/** Called prior to valid playfield, when a switch closure
occurs that does not automatically set valid playfield. */
void try_validate_playfield (U8 swno)
{
	/* If this switch has already been seen since ball start,
	   then ignore it. */
	if (novalid_sw[0] == swno)
		return;
	else if (novalid_sw[1] == swno)
		return;

	/* If the total number of other switches seen that are 'novalid'
	   is less than 2, then add this switch to the tracker. */
	else if (novalid_sw[0] == 0)
		novalid_sw[0] = swno;
	else if (novalid_sw[1] == 0)
		novalid_sw[1] = swno;

	/* Here, this is the third different 'novalid' switch we've
	   seen, so go ahead and declare valid playfield anyway. */
	else
		set_valid_playfield ();
}


/** Adds an additional player to the current game in progress */
void add_player (void)
{
	remove_credit ();
	num_players++;
	audit_increment (&system_audits.games_started);
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
		task_kill_gid (GID_END_GAME);
		in_game = TRUE;
		in_bonus = FALSE;
		in_tilt = FALSE;
		num_players = 0;
		scores_reset ();
		high_score_reset_check ();
	
		add_player ();
		player_up = 1;
		ball_up = 1;
	
		amode_stop ();
		log_event (SEV_INFO, MOD_GAME, EV_START, 0);

		/* Going through all of the start game hooks can take a while,
		 * so sleep before/after to keep the system scheduling OK. */
		task_sleep (TIME_33MS);
		timestamp_update (&system_timestamps.last_game_start);
		callset_invoke (start_game);
		task_sleep (TIME_33MS);

		/* Note: explicitly call this out last, after all other events
		for start_game have been handled */
		player_start_game ();
		start_ball ();
	}
}


/**
 * stop_game is called whenever a game is restarted, or test mode
 * is entered.  It is functionally equivalent to end_game aside
 * from normal end game features like match, high score check, etc.
 */
void stop_game (void)
{
	/* If a game was aborted before it completed, then end_player
	   was not necessarily thrown for all players. */
	if (in_game && ball_up <= system_config.balls_per_game)
	{
		do
		{
			callset_invoke (end_player);
			player_up--;
		} while (player_up > 0);
	}

	in_game = FALSE;
	in_bonus = FALSE;
	in_tilt = FALSE;
	player_up = 0;
	ball_up = 0;
	valid_playfield = FALSE;
	callset_invoke (stop_game);
	deff_stop_all ();
	leff_stop_all ();

	/* Forcibly stop tasks that should run only during a game.
	Do this last, so that the stop_game event handlers gives modules
	a chance to do cleanup before this happens. */
	task_remove_duration (TASK_DURATION_GAME);
	task_duration_expire (TASK_DURATION_GAME);
}


/** Perform final checks before allowing a game to start. */
static bool verify_start_ok (void)
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

#ifdef MACHINE_TZ
	/* Don't allow the game to start if we are still
	 * loading balls into the gumball */
	extern bool gumball_enable_from_trough;
	if (gumball_enable_from_trough)
		return FALSE;
#endif

	/* Give other modules a chance to decide if game start
	is OK.  Return FALSE whenever start should be denied. */
	return callset_invoke_boolean (game_start_allowed);
}


CALLSET_ENTRY (game, start_button_handler)
{
#ifdef MACHINE_START_SWITCH
	/* If not enough credits to start game/add player,
	 * return.  Alert the player of this outside of a game. */
	if (!has_credits_p ())
	{
		if (!in_game)
		{
			deff_start (DEFF_CREDITS);
			callset_invoke (start_without_credits);
		}
		return;
	}

	/* If a game is already in progress and is tilted, do not allow
	further players to be added.  Also do not allow when in test mode. */
	if (in_test || in_tilt)
		return;

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

void validate_num_players (void)
{
	/* Make sure this value is sane */
	if ((num_players == 0) || (num_players > MAX_PLAYERS))
	{
		num_players = 1;
	}
}


/**
 * Provide a default background effect when nothing else
 * is happening.  In a game, this would be the default
 * score screen; otherwise, it is the attract mode effect.
 */
CALLSET_ENTRY (game, display_update)
{
	if (in_game)
	{
		deff_start_bg (DEFF_SCORES, PRI_SCORES);
	}
	else if (sys_init_complete)
	{
		deff_start_bg (DEFF_AMODE, PRI_AMODE);
	}
}


/** Because num_players is tracked across resets in
 * protected memory, it needs an initializer at factory
 * reset time.
 */
CALLSET_ENTRY (game, factory_reset)
{
	num_players = 1;
}


/** Initialize the game subsystem.  */
CALLSET_ENTRY (game, init)
{
	validate_num_players ();
	in_game = FALSE;
	in_bonus = FALSE;
	in_tilt = FALSE;
	valid_playfield = FALSE;
	player_up = 0;
	ball_up = 0;
}

