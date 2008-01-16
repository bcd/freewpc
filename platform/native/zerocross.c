
#include <freewpc.h>
#include <simulation.h>

int cpu_cycles_per_sec;

int zc_period_in_cpu_cycles;

int zc_granularity_in_cpu_cycles;


int sim_zc_read (void)
{
}


void sim_zc_periodic (void *data __attribute__((unused)))
{
}


void sim_zc_init (void)
{
	sim_time_register (1, TRUE, sim_zc_periodic, NULL);
}


