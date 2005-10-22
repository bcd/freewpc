
#include <freewpc.h>

int jet_sound_index;

sound_code_t jet_sounds[] = { SND_HORN1, SND_HORN2, SND_HORN3 };

void sw_jet_sound (void)
{
	jet_sound_index++;
	if (jet_sound_index >= 3)
		jet_sound_index = 0;

	sound_send (jet_sounds[jet_sound_index]);
	task_sleep (TIME_100MS * 2);
	task_exit ();
}

void sw_jet_handler (void)
{
	static U8 score[] = { 0x00, 0x00, 0x12, 0x30 };
	score_add_current (score);
	task_create_gid1 (GID_JET_SOUND, sw_jet_sound);
}

DECLARE_SWITCH_DRIVER (sw_left_jet)
{
	.fn = sw_jet_handler,
	.flags = SW_PLAYFIELD,
};

DECLARE_SWITCH_DRIVER (sw_right_jet)
{
	.fn = sw_jet_handler,
	.flags = SW_PLAYFIELD,
};

DECLARE_SWITCH_DRIVER (sw_bottom_jet)
{
	.fn = sw_jet_handler,
	.flags = SW_PLAYFIELD,
};

