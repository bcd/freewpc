
#include <freewpc.h>
#include <mach/sound.h>

CALLSET_ENTRY (misc, sw_left_sling)
{
	sound_send (SND_SLING);
	score (SC_110);
}


CALLSET_ENTRY (misc, sw_right_sling)
{
	sound_send (SND_SLING);
	score (SC_110);
}

CALLSET_ENTRY (misc, sw_left_jet)
{
	sound_send (SND_DATABASE_BEEP1);
	score (SC_5K);
}

CALLSET_ENTRY (misc, sw_right_jet)
{
	sound_send (SND_DATABASE_BEEP2);
	score (SC_5K);
}

CALLSET_ENTRY (misc, sw_bottom_jet)
{
	sound_send (SND_DATABASE_BEEP3);
	score (SC_5K);
}
