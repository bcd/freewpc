
#include <freewpc.h>

void camera_to_slot_timer (void)
{
	task_sleep_sec (4);
	task_exit ();
}


void sw_camera_handler (void)
{
	score_add_current_const (0x30000);
	sound_send (SND_ODD_CHANGE_BEGIN);

	task_recreate_gid (GID_SLOT_DISABLED_BY_CAMERA, camera_to_slot_timer);
}


CALLSET_ENTRY(camera, start_ball)
{
}


DECLARE_SWITCH_DRIVER (sw_camera)
{
	.fn = sw_camera_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

