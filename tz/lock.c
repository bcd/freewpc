
#include <freewpc.h>


void sw_lock_handler (void)
{
	// device_sw_handler (1);
}

DECLARE_SWITCH_DRIVER (sw_lock)
{
	.fn = sw_lock_handler,
	.devno = SW_DEVICE_DECL(1),
};

		
void lock_enter (device_t *dev)
{
	score_add_current_const (0x7500);
}

void lock_to_loop_timer (void)
{
	task_sleep_sec (3);
	task_exit ();
}

void lock_kick_attempt (device_t *dev)
{
	sound_send (SND_LOCK_KICKOUT);
	task_recreate_gid (GID_LOOP_DISABLED_BY_LOCK_EXIT, lock_to_loop_timer);
}


device_ops_t lock_ops = {
	.enter = lock_enter,
	.kick_attempt = lock_kick_attempt,
};

device_properties_t lock_props = {
	.ops = &lock_ops,
	.name = "LOCK",
	.sol = SOL_LOCK_RELEASE,
	.sw_count = 3,
	.init_max_count = 0,
	.sw = { SW_LOCK_UPPER, SW_LOCK_CENTER, SW_LOCK_LOWER },
};


CALLSET_ENTRY (lock, init)
{
	device_register (1, &lock_props);
}

