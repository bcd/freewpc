
#include <freewpc.h>

U16 burnin_cycles;

timestamp_t burnin_duration;


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
			task_sleep (TIME_500MS);
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

#if (MACHINE_FLIPTRONIC == 1)
void burnin_flipper_thread (void)
{
	for (;;)
	{
		fliptronic_ball_search ();
		task_sleep_sec (8);
	}
}
#endif

void burnin_draw (void)
{
	timestamp_format (&burnin_duration);
	font_render_string_left (&font_mono5, 4, 20, sprintf_buffer);
}

void burnin_timestamp_thread (void)
{
	for (;;)
	{
		timestamp_add_sec (&burnin_duration, 1);
		SECTION_VOIDCALL (__test__, window_redraw);
		task_sleep_sec (1);
	}
}

void burnin_thread (void)
{
	for (;;)
	{
		task_create_peer (burnin_lamp_thread);
		task_create_peer (burnin_sound_thread);
		task_create_peer (burnin_gi_thread);
		task_create_peer (burnin_flasher_thread);
		task_create_peer (burnin_timestamp_thread);
#if (MACHINE_FLIPTRONIC == 1)
		task_create_peer (burnin_flipper_thread);
#endif
		task_sleep_sec (60);
		task_kill_peers ();
		burnin_cycles++;
	}
}

void burnin_init (void)
{
	burnin_cycles = 0;
	timestamp_clear (&burnin_duration);
	task_create_gid (GID_WINDOW_THREAD, burnin_thread);
	flipper_enable ();
}

void burnin_exit (void)
{
	lamp_all_off ();
	triac_leff_free (TRIAC_GI_MASK);
	flipper_disable ();
}

