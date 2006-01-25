
#include <freewpc.h>

__local__ int multiballs_started;


void piano_to_slot_timer (void)
{
	task_sleep_sec (4);
	task_exit ();
}


void sw_piano_handler (void)
{
	extern void door_award_flashing (void);

	task_recreate_gid (GID_SLOT_DISABLED_BY_PIANO, piano_to_slot_timer);

	score_add_current_const (0x5130);
	sound_send (SND_ODD_CHANGE_BEGIN);
	device_multiball_set (3);
}


CALLSET_ENTRY(piano, start_ball)
{
}


DECLARE_SWITCH_DRIVER (sw_piano)
{
	.fn = sw_piano_handler,
};

