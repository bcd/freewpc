
#include <freewpc.h>


#define MAX_ACTIVE_FLASHERS 4


typedef struct {
	solnum_t flasher;
	U8 duration;
} flasher_pulse_t;


flasher_pulse_t flashers_active[MAX_ACTIVE_FLASHERS];


void flasher_pulse (solnum_t n)
{
	U8 i;
	for (i=0; i < MAX_ACTIVE_FLASHERS; i++)
		if (flashers_active[i].flasher == 0)
		{
			flashers_active[i].flasher = n;
			flashers_active[i].duration = TIME_66MS;
			return;
		}
}


void flasher_rtt (void)
{
	U8 i;
	for (i=0; i < MAX_ACTIVE_FLASHERS; i++)
	{
		if (flashers_active[i].flasher != 0)
		{
			sol_on (flashers_active[i].flasher);
			if (--flashers_active[i].duration == 0)
			{
				sol_off (flashers_active[i].flasher);
				flashers_active[i].flasher = 0;
			}
		}
	}
}


void flasher_init (void)
{
	U8 i;
	for (i=0; i < MAX_ACTIVE_FLASHERS; i++)
	{
		flashers_active[i].flasher = 0;
	}
}


