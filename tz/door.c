
#include <freewpc.h>

/** Index of the panel which is currently slow flashing (next to
 * be awarded) or fast flashing (running) */
U8 door_index;

/** Number of door panels that have been started */
U8 door_panels_started;

/** Number of door panels that have been completed */
U8 door_panels_completed;


void door_lamp_flash (U8 index)
{
}

void door_lamp_solid (U8 index)
{
}

void door_start_game (void)
{
	door_index = 0;
	door_panels_started = 0;
	door_panels_completed = 0;
	door_lamp_flash (door_index);
}

void door_start_ball (void)
{
}

