
#include <freewpc.h>
#include <mach/switch.h>
#include <mach/coil.h>


void sw_trough (void)
{
	device_sw_handler (DEV_TROUGH);
}


void trough_enter (device_t *dev)
{
	device_remove_live ();
}


void trough_kick_success (device_t *dev)
{
	device_add_live ();
}

void trough_full (device_t *dev)
{
	db_puts ("Trough is full!\n");
}

void sw_outhole (void)
{
	sol_on (SOL_OUTHOLE);
	task_sleep_sec (1);
	sol_off (SOL_OUTHOLE);
}

device_ops_t trough_ops = {
	.enter = trough_enter,
	.kick_success = trough_kick_success,
	.full = trough_full,
};

device_properties_t trough_props = {
	.ops = &trough_ops,
	.name = "TROUGH",
	.sol = SOL_BALL_SERVE,
	.sw_count = 3,
	.init_max_count = 3,
	.sw = { SW_LEFT_TROUGH, SW_CENTER_TROUGH, SW_RIGHT_TROUGH },
};


void sw_lock (void)
{
	db_puts ("Lock switch handler\n");
	device_sw_handler (1);
}

void lock_enter (device_t *dev)
{
}

void lock_kick_attempt (device_t *dev)
{
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



void sw_slot (void)
{
	device_sw_handler (2);
}

void slot_kick_sound (void)
{
	task_sleep (TIME_100MS * 5);
	sound_send (SND_SLOT_KICKOUT_1);
	sound_send (SND_SLOT_KICKOUT_2);
	task_exit ();
}

void slot_kick_attempt (device_t *dev)
{
	db_puts ("Sending slot kick sound\n");
	task_create_gid (0, slot_kick_sound);
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



void sw_rocket (void)
{
	device_sw_handler (3);
}

void rocket_kick_sound (void)
{
	sound_send (SND_ROCKET_KICK_DONE);
	task_exit ();
}

void rocket_kick_attempt (device_t *dev)
{
	db_puts ("Sending rocket kick sound\n");
	sound_send (SND_ROCKET_KICK_REVVING);
	task_sleep (TIME_100MS * 7);
	task_create_gid (0, rocket_kick_sound);
}



device_ops_t rocket_ops = {
	.kick_attempt = rocket_kick_attempt,
};

device_properties_t rocket_props = {
	.ops = &rocket_ops,
	.name = "ROCKET KICKER",
	.sol = SOL_ROCKET,
	.sw_count = 1,
	.init_max_count = 0,
	.sw = { SW_ROCKET },
};




void trough_init (void)
{
	device_register (DEV_TROUGH, &trough_props);
	device_register (1, &lock_props);
	device_register (2, &slot_props);
	device_register (3, &rocket_props);
}

