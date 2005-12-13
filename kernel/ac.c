
#include <freewpc.h>


U8 ac_last_zerocross_reading;
U8 ac_zerocross_same_count;
bool ac_zerocross_broken;

void ac_rtt (void)
{
#if 0
	U8 ac_current_zerocross_reading = wpc_read_ac_zerocross ();

	if (ac_current_zerocross_reading == ac_last_zerocross_reading)
	{
		ac_zerocross_same_count++;
		if (ac_zerocross_same_count >= 32)
		{
			ac_zerocross_broken = TRUE;
		}
	}
	else
	{
		ac_zerocross_broken = FALSE;
	}

	ac_last_zerocross_reading = ac_current_zerocross_reading;
#endif
}


void ac_idle_task (void)
{
	/** If zerocross is working, we are in a real machine.
	 * Pinmame doesn't support this yet. */
	if (!ac_zerocross_broken)
	{
		fatal (ERR_REAL_HARDWARE);
	}
}


void ac_init (void)
{
	ac_last_zerocross_reading = 0;
	ac_zerocross_same_count = 0;
	ac_zerocross_broken = TRUE;
}

