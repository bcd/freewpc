
#include <freewpc.h>
#include "system/platform.h"
#include "native/gpio.h"
#include "native/log.h"

/* RTTX(name=triac_rtt   freq=1) */
/* RTTX(name=pinio_active_led_toggle   freq=64) */
/* RTTX(name=sound_write_rtt   freq=2) */

#define GPIO_SOL_0 GPID(1,6)
#define GPIO_SOL_1 GPID(1,7)
#define GPIO_SOL_2 GPID(1,2)
#define GPIO_SOL_3 GPID(1,3)

U8 last_lamps;
U8 last_sols;

#ifndef CONFIG_SIM
void writeb (IOPTR ptr, U8 val)
{
	switch (ptr)
	{
		case IO_LAMP:
			if (val != last_lamps)
			{
				gpio_write_led (0, val & 1);
				gpio_write_led (1, val & 2);
				gpio_write_led (2, val & 4);
				gpio_write_led (3, val & 8);
				last_lamps = val;
			}
			break;
		case IO_SOL:
			if (val != last_sols)
			{
				gpio_write (GPIO_SOL_0, val & 1);
				gpio_write (GPIO_SOL_1, val & 2);
				gpio_write (GPIO_SOL_2, val & 4);
				gpio_write (GPIO_SOL_3, val & 8);
				last_sols = val;
			}
			break;
	}
}

U8 readb (IOPTR ptr)
{
	U8 val = 0;
	//printf ("readb(%X) -> %X\n", ptr, val);
	return val;
}
#endif

/* RTT(name=switch_rtt freq=2) */
void switch_rtt (void)
{
	platform_switch_input (0, readb (IO_SWITCH));
	platform_switch_debounce (0);
	int c = posix_input_poll (0);
	if (c > 0)
	{
		if (c == ' ')
			print_log ("space found\n");
	}
}

/* RTT(name=lamp_rtt freq=16) */
void lamp_rtt (void)
{
	writeb (IO_LAMP, platform_lamp_compute (0));
}

/* RTT(name=sol_update_rtt_0 freq=1) */
void sol_update_rtt_0 (void)
{
	pinio_write_solenoid_set (0, *sol_get_read_reg (0));
}

void sol_update_rtt_1 (void)
{
}

void platform_init (void)
{
	/* Request GPIOs */
	gpio_request_output (GPIO_SOL_0);
	gpio_request_output (GPIO_SOL_1);
	gpio_request_output (GPIO_SOL_2);
	gpio_request_output (GPIO_SOL_3);

	posix_input_config (0);

	last_lamps = last_sols = 0;
}

void amode_leff (void)
{
	for (;;)
	{
		leff_on (LM_SHOOT_AGAIN);
		task_sleep (TIME_166MS);
		leff_off (LM_SHOOT_AGAIN);
		leff_on (LM_TARGET);
		task_sleep (TIME_166MS);
		leff_off (LM_TARGET);
		task_sleep (TIME_166MS);
	}
}

