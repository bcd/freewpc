
#include <freewpc.h>

void piano_to_slot_timer (void)
{
	task_sleep_sec (4);
	task_exit ();
}


void sw_piano_handler (void)
{
	score_add_current_const (0x5130);
	sound_send (SND_ODD_CHANGE_BEGIN);

	task_recreate_gid (GID_SLOT_DISABLED_BY_PIANO, piano_to_slot_timer);
}


CALLSET_ENTRY(piano, start_ball)
{
}


DECLARE_SWITCH_DRIVER (sw_piano)
{
	.fn = sw_piano_handler,
};

