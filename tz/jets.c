
#include <freewpc.h>

sound_code_t jet_sound;

void sw_jet_handler (void)
{
	if ((jet_sound > SND_HORN4) || (jet_sound < SND_HORN1))
		jet_sound = SND_HORN1;
	else
		jet_sound++;
	sound_send (jet_sound);
	task_sleep (TIME_100MS * 2);
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

