
#include <freewpc.h>

/* TBD: when multiple leffs are supported, this function should
 * become a true leff. */
static void ballsave_monitor (void)
{
	int i;
	lamp_leff_allocate (LM_SHOOT_AGAIN);
	for (i=0; i < 40; i++)
	{
		leff_on (LM_SHOOT_AGAIN);
		task_sleep (TIME_100MS);
		leff_off (LM_SHOOT_AGAIN);
		task_sleep (TIME_100MS);
	}
	lamp_leff_free (LM_SHOOT_AGAIN);
	task_sleep_sec (2);
	task_exit ();
}

void ballsave_enable (void)
{
	task_recreate_gid (GID_BALLSAVER, ballsave_monitor);
}

void ballsave_disable (void)
{
	task_kill_gid (GID_BALLSAVER);
}

bool ballsave_test_active (void)
{
	return task_find_gid (GID_BALLSAVER) ? TRUE : FALSE;
}


void ballsave_launch (void)
{
	extern void autofire_add_ball (void);

	autofire_add_ball ();
	deff_start (DEFF_BALL_SAVE);
	ballsave_disable ();
}


