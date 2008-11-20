
#include <freewpc.h>
#include <simulation.h>


U8 hwtimer_value;

void hwtimer_periodic (void)
{
	if (hwtimer_value & 0x80)
	{
	}
	else if (hwtimer_value > 0)
	{
		--hwtimer_value;
		if (hwtimer_value == 0)
		{
			hwtimer_value = 0x80;
		}
		else
		{
			sim_time_register (1, FALSE, hwtimer_periodic, NULL);
		}
	}
}


U8 hwtimer_read (void)
{
	return hwtimer_value;
}

void hwtimer_write (U8 val)
{
	hwtimer_value = val;
	if (val != 0)
	{
		simlog (SLC_DEBUG, "Hardware timer started.");
		sim_time_register (1, FALSE, hwtimer_periodic, NULL);
	}
}

void hwtimer_init (void)
{
	hwtimer_write (0);
}

