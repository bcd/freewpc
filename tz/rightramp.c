
#include <freewpc.h>



void sw_right_ramp_task (void)
{
	task_sleep_sec (2);
	sol_on (SOL_RIGHT_RAMP_DIV);
	task_sleep (TIME_100MS * 2);
	sol_off (SOL_RIGHT_RAMP_DIV);
	task_exit ();
}


void sw_right_ramp_handler (void)
{
	if (in_game && !in_tilt)
	{
		static U8 score[] = { 0x00, 0x00, 0x50, 0x00 };
		score_add_current (score);
	}

	if (!task_find_gid (GID_RIGHT_RAMP_ENTERED))
	{
		if (in_game && !in_tilt)
			sound_send (SND_RIGHT_RAMP_DEFAULT_ENTER);
		task_create_gid (GID_RIGHT_RAMP_ENTERED, sw_right_ramp_task);
	}
}


void sw_right_ramp_init (void)
{
}


DECLARE_SWITCH_DRIVER (sw_right_ramp)
{
	.fn = sw_right_ramp_handler,
	.flags = SW_PLAYFIELD,
};


