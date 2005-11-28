
#include <freewpc.h>

U8 greed_sound_index;

U8 greed_sounds[] = { 
	SND_GREED_DEFAULT_1,
	SND_GREED_DEFAULT_2,
	SND_GREED_DEFAULT_3,
	SND_GREED_DEFAULT_4,
};


void sw_greed_handler (void)
{
	greed_sound_index++;
	if (greed_sound_index >= 4)
		greed_sound_index = 0;
	sound_send (greed_sounds[greed_sound_index]);
	score_add_current_const (0x500);
	task_exit ();
}


DECLARE_SWITCH_DRIVER (sw_greed)
{
	.fn = sw_greed_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

