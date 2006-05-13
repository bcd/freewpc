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

#ifndef _HOOK_H
#define _HOOK_H

/* machine_hooks is a struct of game-specific
 * functions that are called at the appropriately
 * places.  The machine config code should declare
 * an object of this type and define the macro
 * MACHINE_HOOKS to the same name, so that the
 * system can find it.
 *
 * If a particular hook need not be implemented,
 * then it can be left as NULL.
 *
 * All of the game hooks need to be in the same
 * ROM page.
 */
typedef struct machine_hooks
{
	void (*init) (void);
	bool (*start_game_attempt) (void);
	void (*start_game) (void);
	void (*start_player) (void);
	void (*add_player) (void);
	void (*start_ball) (void);
	void (*ball_in_play) (void);
	void (*ball_drain) (void);
	bool (*end_ball) (void);
	void (*bonus) (void);
	void (*abort_game) (void);
	void (*end_game) (void);
	void (*amode_start) (void);
	void (*amode_stop) (void);
	void (*tilt_warning) (void);
	void (*tilt) (void);
	void (*slam_tilt) (void);
	void (*replay) (void);
	void (*high_score) (void);
	void (*grand_champion) (void);
	void (*coin_added) (void);
	void (*credit_added) (void);
	void (*start_without_credits) (void);
	void (*volume_change) (void);
	void (*ball_search) (void);
	void (*any_pf_switch) (void);
	void (*lamp_refresh) (void);
} machine_hooks_t;

#define INHERIT_FROM_DEFAULT_HOOKS \
	.init = null_function, \
	.start_game_attempt = null_true_function, \
	.start_game = null_function, \
	.start_player = null_function, \
	.add_player = null_function, \
	.start_ball = null_function, \
	.ball_in_play = null_function, \
	.ball_drain = null_function, \
	.end_ball = null_true_function, \
	.bonus = null_function, \
	.abort_game = null_function, \
	.end_game = null_function, \
	.amode_start = null_function, \
	.amode_stop = null_function, \
	.tilt_warning = null_function, \
	.tilt = null_function, \
	.slam_tilt = null_function, \
	.replay = null_function, \
	.high_score = null_function, \
	.grand_champion = null_function, \
	.coin_added = null_function, \
	.credit_added = null_function, \
	.start_without_credits = null_function, \
	.volume_change = null_function, \
	.ball_search = null_function, \
	.any_pf_switch = null_function, \
	.lamp_refresh = null_function


#ifdef MACHINE_HOOKS

extern machine_hooks_t MACHINE_HOOKS;

#define call_hook(hook)	(MACHINE_HOOKS.hook) ()

#define call_boolean_hook(hook) \
((MACHINE_HOOKS.hook) ? (MACHINE_HOOKS.hook) () : TRUE)

#else

/* If no hooks are defined, then calling a hook is trivial */
#define call_hook(hook)
#define call_boolean_hook(hook)

#endif


#endif /* _HOOK_H */

