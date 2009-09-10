
#include <freewpc.h>

U8 lamplist;

void flashleff_entry (void)
{
	U8 *flasherp = task_current_class_data (U8);
	for (;;)
	{
		flasher_start (*flasherp, 0x1, TIME_500MS);
		task_sleep (TIME_300MS);
		flasher_start (*flasherp, 0x11, TIME_400MS);
		task_sleep (TIME_200MS);
		flasher_start (*flasherp, 0x55, TIME_200MS);
		task_sleep (TIME_100MS);
		flasher_start (*flasherp, 0x11, TIME_400MS);
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

static void amode_leff1 (void)
{
	register U8 my_lamplist = lamplist;
	lamplist_set_apply_delay (TIME_66MS);
	for (;;)
		lamplist_apply (my_lamplist, leff_toggle);
}


void amode_leff (void)
{
	triac_leff_enable (TRIAC_GI_MASK);
	for (lamplist = LAMPLIST_MARTIANS; lamplist <= LAMPLIST_R_LOOP_ALL; lamplist++)
	{
		leff_create_peer (amode_leff1);
		task_sleep (TIME_166MS);
	}
	task_exit ();
}

