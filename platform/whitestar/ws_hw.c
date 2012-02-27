
#include <freewpc.h>
#include <system/platform.h>

U8 ws_page_led_io;
U8 ws_aux_ctrl_io;

void puts_default (const char *s)
{
}

void switch_rtt (void)
{
}

void lamp_rtt (void)
{
}

void sol_update_rtt_0 (void)
{
}

void sol_update_rtt_1 (void)
{
}

void platform_init (void)
{
	ws_page_led_io = 0;
	ws_aux_ctrl_io = 0;
}


