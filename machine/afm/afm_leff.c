
#include <freewpc.h>


void flashleff_entry (void)
{
	U8 *flasherp = task_current_class_data (U8);
	for (;;)
	{
		sol_start (*flasherp, 0x1, TIME_500MS);
		task_sleep (TIME_300MS);
		sol_start (*flasherp, 0x11, TIME_400MS);
		task_sleep (TIME_200MS);
		sol_start (*flasherp, 0x55, TIME_200MS);
		task_sleep (TIME_100MS);
		sol_start (*flasherp, 0x11, TIME_400MS);
		task_sleep (TIME_200MS);
	}
	task_exit ();
}


static inline void flashleff_spawn (const U8 flasher)
{
	task_pid_t tp;
	tp = task_create_peer (flashleff_entry);
	*(task_init_class_data (tp, U8)) = flasher;
	task_sleep (TIME_100MS);
}


void flashfest_leff (void)
{
	flashleff_spawn (FLASH_SAUCER_FLASH_1);
	flashleff_spawn (FLASH_SAUCER_FLASH_2);
	flashleff_spawn (FLASH_SAUCER_FLASH_3);
	flashleff_spawn (FLASH_SAUCER_FLASH_4);
	flashleff_spawn (FLASH_SAUCER_FLASH_5);
	flashleff_spawn (FLASH_SAUCER_FLASH_6);
	task_kill_peers ();
	leff_exit ();
}

