
#include <freewpc.h>
#include <mach/switch.h>
#include <mach/coil.h>


void sw_slot_handler (void)
{
	device_sw_handler (2);
}


DECLARE_SWITCH_DRIVER (sw_slot)
{
	.fn = sw_slot_handler,
};

void slot_kick_sound (void)
{
	task_sleep (TIME_100MS);
	sound_send (SND_SLOT_KICKOUT_2);
	task_exit ();
}

void slot_kick_attempt (device_t *dev)
{
	db_puts ("Sending slot kick sound\n");
	if (in_game && !in_tilt)
	{
		sound_send (SND_SLOT_KICKOUT_1);
		task_sleep (TIME_100MS * 7);
		task_create_gid (0, slot_kick_sound);
	}
}


device_ops_t slot_ops = {
	.kick_attempt = slot_kick_attempt,
};

device_properties_t slot_props = {
	.ops = &slot_ops,
	.name = "SLOT KICKOUT",
	.sol = SOL_SLOT,
	.sw_count = 1,
	.init_max_count = 0,
	.sw = { SW_SLOT },
};


void slot_init (void)
{
	device_register (2, &slot_props);
}

