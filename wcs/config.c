
#include <freewpc.h>

/** Filename: mach/config.c
 * Machine-specific functions.  These are all callbacks
 * from the main kernel code to handle various events
 * in game-specific ways.
 */


/** An array in which each 1 bit represents an opto switch
 * and 0 represents a normal switch.  This is used for
 * determining the logical level of a switch (open/closed)
 * rather than its actual level.
 *
 * The first byte is for the dedicated switches, next is
 * column 1, etc. to column 8.
 */
const uint8_t mach_opto_mask[] = {
	0x00, 0x00, 0x08, 0x3F, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00,
};


/** An array in which each 0 bit indicates a 'normal'
 * playfield switch where the switch is serviced only
 * when it transitions from inactive->active.  When set
 * to a 1, the switch is also serviced on active->inactive
 * transitions.  These are called edge switches since
 * they 'trigger' on any edge transition.
 *
 * At a minimum, container switches need to be declared
 * as edge switches, since they must be handled whenever
 * they change state (the device count goes up or down).
 */
const uint8_t mach_edge_switches[] = {
	0x00, 0x00, 0x00, 0x1F, 0x12, 0x38, 0x00, 0xC0, 0x00, 0x00,
};

void funhouse_init (void)
{
	#include <init.callset>
}

CALLSET_ENTRY (unused, init) {}
CALLSET_ENTRY (unused, start_game) {}
CALLSET_ENTRY (unused, end_game) {}
CALLSET_ENTRY (unused, start_ball) {}

void funhouse_start_game (void)
{
	#include <start_game.callset>
	task_sleep_sec (2);
}


void funhouse_end_game (void)
{
	#include <end_game.callset>
	if (!in_test)
		music_set (MUS_POWERBALL_MANIA);
}

void funhouse_start_ball (void)
{
	music_set (MUS_MULTIBALL_LIT_PLUNGER);
	#include <start_ball.callset>
}

void funhouse_ball_in_play (void)
{
	music_set (MUS_MULTIBALL_LIT);
	/* start ballsaver if enabled */
}

bool funhouse_end_ball (void)
{
	sound_reset ();
	return TRUE;
}


void funhouse_add_player (void)
{
}


void funhouse_any_pf_switch (void)
{
}


void funhouse_bonus (void)
{
	deff_start (DEFF_BONUS);
	leff_start (LEFF_BONUS);
	task_sleep_sec (1);
	while (deff_get_active () == DEFF_BONUS)
		task_sleep (TIME_33MS);
	leff_stop (LEFF_BONUS);
}


void funhouse_tilt (void)
{
}


void funhouse_tilt_warning (void)
{
}


void funhouse_start_without_credits (void)
{
}

void funhouse_coin_added (void)
{
}

machine_hooks_t funhouse_hooks = {
	.start_game = funhouse_start_game,
	.end_game = funhouse_end_game,
	.start_ball = funhouse_start_ball,
	.ball_in_play = funhouse_ball_in_play,
	.end_ball = funhouse_end_ball,
	.add_player = funhouse_add_player,
	.init = funhouse_init,
	.bonus = funhouse_bonus,
	.tilt = funhouse_tilt,
	.tilt_warning = funhouse_tilt_warning,
	.any_pf_switch = funhouse_any_pf_switch,
	.start_without_credits = funhouse_start_without_credits,
	.coin_added = funhouse_coin_added,
};

