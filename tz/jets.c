
#include <freewpc.h>

int jet_sound_index;

sound_code_t jet_sounds[] = { SND_HORN1, SND_HORN2, SND_HORN3 };

void sw_jet_sound (void)
{
	jet_sound_index++;
	if (jet_sound_index >= 3)
		jet_sound_index = 0;

	sound_send (jet_sounds[jet_sound_index]);
	flasher_pulse (FLASH_JETS);
	task_sleep (TIME_100MS * 2);
	task_exit ();
}

void sw_jet_handler (void)
{
	score_add_current_const (0x1230);
	task_create_gid1 (GID_JET_SOUND, sw_jet_sound);

	extern void door_advance_flashing (void);
	door_advance_flashing ();
}

DECLARE_SWITCH_DRIVER (sw_left_jet)
{
	.fn = sw_jet_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

DECLARE_SWITCH_DRIVER (sw_right_jet)
{
	.fn = sw_jet_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

DECLARE_SWITCH_DRIVER (sw_bottom_jet)
{
	.fn = sw_jet_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

