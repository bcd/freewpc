
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
	0x00, 0x00, 0x00, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
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
	0x00, 0x00, 0x00, 0x6F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void afm_init (void)
{
	#include <init.callset>
}

CALLSET_ENTRY (unused, init) {}
CALLSET_ENTRY (unused, start_game) {}
CALLSET_ENTRY (unused, end_game) {}
CALLSET_ENTRY (unused, start_ball) {}


/* For testing the deff declaration syntax. */
DECLARE_DEFF(DESIGN_CREDITS, D_NORMAL, 100)
{
	deff_exit ();
}


void afm_start_game (void)
{
	#include <start_game.callset>
	task_sleep_sec (2);
}


void afm_end_game (void)
{
	#include <end_game.callset>
}

void afm_start_ball (void)
{
	#include <start_ball.callset>
}

void afm_ball_in_play (void)
{
	/* start ballsaver if enabled */
}

bool afm_end_ball (void)
{
	sound_reset ();
	return TRUE;
}


void afm_add_player (void)
{
}


void afm_any_pf_switch (void)
{
}


void afm_bonus (void)
{
	deff_start (DEFF_BONUS);
	leff_start (LEFF_BONUS);
	task_sleep_sec (1);
	while (deff_get_active () == DEFF_BONUS)
		task_sleep (TIME_33MS);
	leff_stop (LEFF_BONUS);
}


void afm_tilt (void)
{
}


void afm_tilt_warning (void)
{
}


void afm_start_without_credits (void)
{
}

void afm_coin_added (void)
{
}

machine_hooks_t afm_hooks = {
	.start_game = afm_start_game,
	.end_game = afm_end_game,
	.start_ball = afm_start_ball,
	.ball_in_play = afm_ball_in_play,
	.end_ball = afm_end_ball,
	.add_player = afm_add_player,
	.init = afm_init,
	.bonus = afm_bonus,
	.tilt = afm_tilt,
	.tilt_warning = afm_tilt_warning,
	.any_pf_switch = afm_any_pf_switch,
	.start_without_credits = afm_start_without_credits,
	.coin_added = afm_coin_added,
};

