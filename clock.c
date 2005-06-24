
#include <wpc.h>
#include <sys/sol.h>
#include <mach/coil.h>

/*
 * Twilight Zone Clock Driver
 */

void tz_clock_init (void) {}

void tz_clock_reset (void) {}


void tz_clock_start_forward (void)
{
	sol_off (SOL_CLOCK_REVERSE);
	sol_on (SOL_CLOCK_FORWARD);
}


void tz_clock_start_backward (void)
{
	sol_off (SOL_CLOCK_FORWARD);
	sol_on (SOL_CLOCK_REVERSE);
}


void tz_clock_stop (void)
{
	sol_off (SOL_CLOCK_FORWARD);
	sol_off (SOL_CLOCK_REVERSE);
}

