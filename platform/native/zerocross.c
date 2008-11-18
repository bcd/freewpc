
#include <freewpc.h>
#include <simulation.h>

#define SIM_ZC_NONE
//#define SIM_ZC_DOMESTIC
//#define SIM_ZC_EXPORT


/* Simulation of the zerocross circuit */

unsigned int sim_zc_active;

unsigned int sim_zc_timer_start;
unsigned int sim_zc_timer;

/** Called by the software when reading the zerocross register.
 * Returns nonzero if currently at a zerocrossing.
 * The register is clear-on-read.
 */
int sim_zc_read (void)
{
	int rc = sim_zc_active;
	sim_zc_active = 0;
	return rc;
}


void sim_zc_periodic (void *data __attribute__((unused)))
{
	if (--sim_zc_timer == 0)
	{
		sim_zc_active = 1;
		sim_zc_timer = sim_zc_timer_start;
		signal_update (SIGNO_ZEROCROSS, 1);
	}
	else
	{
		signal_update (SIGNO_ZEROCROSS, 0);
	}
}


void sim_zc_init (void)
{
	sim_zc_active = 0;

	/* Generate zerocross every 8ms -- close enough */
	sim_zc_timer = sim_zc_timer_start = 8;

	/* Register a callback every 1ms */
	sim_time_register (1, TRUE, sim_zc_periodic, NULL);
}

