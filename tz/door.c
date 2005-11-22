
#include <freewpc.h>

/** Index of the panel which is currently slow flashing (next to
 * be awarded) or fast flashing (running) */
U8 door_index;

/** Number of door panels that have been started */
U8 door_panels_started;

/** Number of door panels that have been completed */
U8 door_panels_completed;


extern inline const U8 *door_get_lamps (void)
{
	return lampset_lookup (LAMPSET_DOOR_PANELS);
}


void door_set_flashing (U8 id)
{
	const U8 *door_lamps = door_get_lamps ();
	lamp_flash_off (door_lamps[door_index]);
	door_index = id;
	lamp_flash_on (door_lamps[door_index]);
}


void door_advance_flashing (void)
{
	U8 new_door_index = door_index + 1;
	if (new_door_index > 14)
		new_door_index = 0;
	door_set_flashing (new_door_index);
}


CALLSET_ENTRY(door, start_game)
{
	door_index = 0;
	door_panels_started = 0;
	door_panels_completed = 0;
}

CALLSET_ENTRY(door, start_ball)
{
	door_set_flashing (door_index);
}

