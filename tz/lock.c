
#include <freewpc.h>
#include <mach/switch.h>
#include <mach/coil.h>


void sw_lock_handler (void)
{
	db_puts ("Lock switch handler\n");
	device_sw_handler (1);
}

DECLARE_SWITCH_DRIVER (sw_lock)
{
	.fn = sw_lock_handler,
};

		
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


void lock_init (void)
{
	device_register (1, &lock_props);
}

