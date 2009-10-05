
#include <freewpc.h>

U16 burnin_cycles;

void burnin_sound_thread (void)
{
	for (;;)
	{
		task_sleep_sec (1);
	}
}

void burnin_gi_thread (void)
{
	triac_leff_allocate (TRIAC_GI_MASK);
	for (;;)
	{
		U8 gi;
		for (gi = (1 << 0); gi <= (1 << 4); gi <<= 1)
		{
			triac_leff_enable (gi);
			task_sleep_sec (1);
			triac_leff_disable (gi);
		}
	}
}

void burnin_flasher_thread (void)
{
	for (;;)
	{
		U8 sol;
		for (sol=0; sol < NUM_POWER_DRIVES; sol++)
		{
			if (MACHINE_SOL_FLASHERP (sol))
			{
				flasher_pulse (sol);
				task_sleep (TIME_200MS);
			}
		}
	}
}

void burnin_lamp_thread (void)
{
	SECTION_VOIDCALL (__test__, all_lamp_test_thread);
}

void burnin_thread (void)
{
	for (;;)
	{
		task_create_peer (burnin_lamp_thread);
		task_create_peer (burnin_sound_thread);
		task_create_peer (burnin_gi_thread);
		task_create_peer (burnin_flasher_thread);
		task_sleep_sec (60);
		task_kill_peers ();
		burnin_cycles++;
	}
}

void burnin_init (void)
{
	burnin_cycles = 0;
	task_create_gid (GID_WINDOW_THREAD, burnin_thread);
}

void burnin_exit (void)
{
	lamp_all_off ();
	triac_leff_free (TRIAC_GI_MASK);
}

