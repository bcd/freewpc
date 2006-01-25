
#include <freewpc.h>

/** Index of the panel which is currently slow flashing (next to
 * be awarded) or fast flashing (running) */
U8 door_index;

/** Number of door panels that have been started */
U8 door_panels_started;

/** Number of door panels that have been completed */
U8 door_panels_completed;

U8 door_active_lamp;


/** Names of all the door panels, in order */
const char *door_panel_names[] = {
	"TOWN SQUARE MADNESS",
	"LIGHT EXTRA BALL",
	"SUPER SLOT",
	"CLOCK MILLIONS",
	"THE SPIRAL",
	"BATTLE THE POWER",
	"10 MILLION",
	"GREED",
	"THE CAMERA",
	"THE HITCHHIKER",
	"CLOCK CHAOS",
	"SUPER SKILL SHOT",
	"FAST LOCK",
	"LIGHT GUMBALL",
	"RETURN TO THE ZONE",
};


extern inline const U8 *door_get_lamps (void)
{
	return lampset_lookup (LAMPSET_DOOR_PANELS);
}


extern inline const U8 door_get_flashing_lamp (void)
{
	return ((door_get_lamps ())[door_index]);
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


void door_award_deff (void)
{
	dmd_alloc_low_clean ();
	printf ("%s", door_panel_names[door_index]);
	font_render_string_center (&font_5x5, 64, 10, sprintf_buffer);
	printf ("%d DOOR PANELS", door_panels_started);
	font_render_string_center (&font_term6, 64, 21, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (4);
	deff_exit ();
}


void door_award_flashing (void)
{
	door_active_lamp = door_get_flashing_lamp ();
	lamp_on (door_active_lamp);
	door_panels_started++;
	deff_start (DEFF_DOOR_AWARD);
	task_sleep_sec (1);
	door_advance_flashing ();
}


CALLSET_ENTRY(door, start_game)
{
	door_index = 0;
	door_panels_started = 0;
	door_panels_completed = 0;
}

CALLSET_ENTRY(door, start_ball)
{
	lamp_on (LM_PIANO_PANEL);
	lamp_on (LM_SLOT_MACHINE);
	door_set_flashing (door_index);
}

