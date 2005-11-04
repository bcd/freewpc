
#include <freewpc.h>

void deadend_to_slot_timer (void)
{
	task_sleep_sec (4);
	task_exit ();
}


void sw_deadend_handler (void)
{
	score_add_current_const (0x75000);
	sound_send (SND_ODD_CHANGE_BEGIN);

	task_recreate_gid (GID_SLOT_DISABLED_BY_DEAD_END, deadend_to_slot_timer);
}


void CALLSET(deadend, start_ball) (void)
{
}


DECLARE_SWITCH_DRIVER (sw_deadend)
{
	.fn = sw_deadend_handler,
};

