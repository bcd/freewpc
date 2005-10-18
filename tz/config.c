
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

	lock_init ();
	slot_init ();
	rocket_init ();
}

void tz_start_game (void)
{
	sound_send (SND_YOU_UNLOCK_THIS_DOOR);
}


void tz_add_player (void)
{
	if (num_players > 1)
		sound_send (SND_PLAYER_ONE + num_players - 1);
}


void tz_any_pf_switch (void)
{
}


machine_hooks_t tz_hooks = {
	.start_game = tz_start_game,
	.add_player = tz_add_player,
	.init = tz_init,
	.any_pf_switch = tz_any_pf_switch,
};

