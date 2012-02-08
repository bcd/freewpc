
#include <freewpc.h>
#include <system/platform.h>

void switch_rtt (void)
{
	platform_switch_input (0, readb (IO_SWITCH));
	platform_switch_debounce (0);
}

void lamp_rtt (void)
{
	writeb (IO_LAMP, platform_lamp_compute (0));
}

void sol_update_rtt_0 (void)
{
	pinio_write_solenoid_set (0, *sol_get_read_reg (0));
}

void sol_update_rtt_1 (void)
{
}

