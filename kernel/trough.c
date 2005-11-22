
#include <freewpc.h>
#include <mach/switch.h>
#include <mach/coil.h>


void sw_trough_handler (void)
{
}


DECLARE_SWITCH_DRIVER (sw_trough)
{
	.fn = sw_trough_handler,
	.devno = SW_DEVICE_DECL(DEV_TROUGH),
};


void sw_outhole_handler (void)
{
	sol_on (SOL_OUTHOLE);
	task_sleep_sec (1);
	sol_off (SOL_OUTHOLE);
}

DECLARE_SWITCH_DRIVER (sw_outhole)
{
	.fn = sw_outhole_handler,
};

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


void trough_init (void)
{
	device_register (DEV_TROUGH, &trough_props);
}

