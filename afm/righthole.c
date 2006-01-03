
#include <freewpc.h>
#include <mach/switch.h>
#include <mach/coil.h>


void sw_righthole_handler (void)
{
}


DECLARE_SWITCH_DRIVER (sw_righthole)
{
	.fn = sw_righthole_handler,
	.devno = SW_DEVICE_DECL(1),
};

void righthole_kick_sound (void)
{
	task_exit ();
}


void righthole_enter (device_t *dev)
{
	mark_ball_in_play ();
	score_add_current_const (0x2500);
}


void righthole_kick_attempt (device_t *dev)
{
	db_puts ("Sending righthole kick sound\n");
	if (in_game && !in_tilt)
	{
	}
}


device_ops_t righthole_ops = {
	.enter = righthole_enter,
	.kick_attempt = righthole_kick_attempt,
};

device_properties_t righthole_props = {
	.ops = &righthole_ops,
	.name = "RIGHT POPPER",
	.sol = SOL_RIGHT_POPPER,
	.sw_count = 1,
	.init_max_count = 0,
	.sw = { SW_RIGHT_POPPER },
};


CALLSET_ENTRY (righthole, init)
{
	device_register (1, &righthole_props);
}

