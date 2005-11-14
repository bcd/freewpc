
#include <freewpc.h>


/** An array in which each 1 bit represents an opto switch
 * and 0 represents a normal switch.  This is used for
 * determining the logical level of a switch (open/closed)
 * rather than its actual level.
 *
 * The first byte is for the dedicated switches, next is
 * column 1, etc. to column 8.
 */
const uint8_t mach_opto_mask[] = {
	0x00, 0x00, 0x28, 0x0, 0x0, 0x10, 0x0, 0x3E, 0x5D, 0xFF,
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
	0x00, 0x70, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x98, 0x00,
};

void tz_init (void)
{
	extern void lock_init (void);
	extern void slot_init (void);
	extern void rocket_init (void);
	extern void tz_clock_init (void);

	lock_init ();
	slot_init ();
	rocket_init ();
	tz_clock_init ();
}

void tz_start_game (void)
{
	sound_send (SND_DONT_TOUCH_DOOR_1);

	lamp_flash_on (LM_PANEL_TSM);
	lamp_flash_on (LM_SPIRAL_2M);
	lamp_on (LM_LEFT_INLANE1);
	lamp_on (LM_LEFT_INLANE2);
	lamp_on (LM_RIGHT_INLANE);
	lamp_on (LM_LEFT_JET);
	lamp_on (LM_LOWER_JET);
	lamp_on (LM_RIGHT_JET);

	#include <start_game.callset>
	task_sleep_sec (2);
}


void tz_end_game (void)
{
	if (!in_test)
		music_set (MUS_POWERBALL_MANIA);
}

void tz_start_ball (void)
{
	extern void enable_skill_shot ();

	music_set (MUS_MULTIBALL_LIT_PLUNGER);
	enable_skill_shot ();
	#include <start_ball.callset>
}

void tz_ball_in_play (void)
{
	extern void disable_skill_shot ();

	music_set (MUS_MULTIBALL_LIT);
	disable_skill_shot ();
	/* start ballsaver if enabled */
}

void tz_end_ball (void)
{
	sound_reset ();
}


void tz_add_player (void)
{
#ifdef CONFIG_TZONE_IP
	if (num_players > 1)
		sound_send (SND_PLAYER_ONE + num_players - 1);
#endif
}


void tz_any_pf_switch (void)
{
}


void tz_bonus (void)
{
	deff_start (DEFF_BONUS);
	leff_start (LEFF_BONUS);
	task_sleep_sec (1);
	while (deff_get_active () == DEFF_BONUS)
		task_sleep (TIME_33MS);
	leff_stop (LEFF_BONUS);
}


void tz_tilt (void)
{
	sound_send (SND_TILT);
	task_sleep_sec (3);
	sound_send (SND_OH_NO);
}


void tz_tilt_warning (void)
{
	sound_send (SND_TILT_WARNING);
}


void tz_start_without_credits (void)
{
}

void tz_coin_added (void)
{
}

machine_hooks_t tz_hooks = {
	.start_game = tz_start_game,
	.end_game = tz_end_game,
	.start_ball = tz_start_ball,
	.ball_in_play = tz_ball_in_play,
	.end_ball = tz_end_ball,
	.add_player = tz_add_player,
	.init = tz_init,
	.bonus = tz_bonus,
	.tilt = tz_tilt,
	.tilt_warning = tz_tilt_warning,
	.any_pf_switch = tz_any_pf_switch,
	.start_without_credits = tz_start_without_credits,
	.coin_added = tz_coin_added,
};

