
#include <freewpc.h>
#include <simulation.h>


/** The maximum number of IRQ times that must elapse to
 * signal a watchdog reset */
#define WDOG_EXPIRY 64


/** The current value of the watchdog timer */
volatile int cycles_until_reset = WDOG_EXPIRY;


/** The lowest value of the watchdog register, for debugging */
int lowest_cycles = WDOG_EXPIRY;


void watchdog_expired (void)	
{
	simlog (SLC_DEBUG, "Watchdog expired\n");
	linux_shutdown ();
}


static void sim_watchdog_periodic (void *data __attribute__((unused)))
{
	if (--cycles_until_reset < 0)
		watchdog_expired ();
}


void sim_watchdog_reset (void)
{
	if (cycles_until_reset < lowest_cycles)
	{
		simlog (SLC_DEBUG, "Watchdog reset with %d cycles to go", cycles_until_reset);
		lowest_cycles = cycles_until_reset;
	}
	cycles_until_reset = WDOG_EXPIRY;
}


void sim_watchdog_init (void)
{
	sim_time_register (8, TRUE, sim_watchdog_periodic, NULL);
}

