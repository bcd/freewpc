
#include <freewpc.h>

/*
 * Timers are special cases of tasks; they just all run the same
 * function, counting down the time, then exiting.
 *
 * TIMER_GRAN is the minimum granularity of all timers.
 */

#define TIMER_FREERUNNING_GRAN	(TIME_100MS * 5)
#define TIMER_PAUSABLE_GRAN		(TIME_100MS * 2)


U8 pausable_timer_locks;


__taskentry__ void freerunning_timer_function (void)
{
	U16 ticks = task_get_arg ();
	while (ticks > 0)
	{
		task_sleep (TIMER_FREERUNNING_GRAN);
		ticks -= TIMER_FREERUNNING_GRAN;
	}
	task_exit ();
}


__taskentry__ void pausable_timer_function (void)
{
	U16 ticks = task_get_arg ();
	while (ticks > 0)
	{
		task_sleep (TIMER_PAUSABLE_GRAN);
		if (pausable_timer_locks != 0)
			continue;
		ticks -= TIMER_PAUSABLE_GRAN;
	}
	task_exit ();
}


task_t *timer_find_gid (task_gid_t gid)
{
	return task_find_gid (gid);
}


task_t *timer_restart (task_gid_t gid, task_ticks_t ticks, task_function_t fn)
{
	task_t *tp = task_recreate_gid (gid, fn);
	task_set_arg (tp, (U16)ticks);
	return (tp);
}


task_t *timer_start1 (task_gid_t gid, task_ticks_t ticks, task_function_t fn)
{
	task_t *tp = task_create_gid1 (gid, fn);
	task_set_arg (tp, (U16)ticks);
	return (tp);
}


task_t *timer_start (task_gid_t gid, task_ticks_t ticks, task_function_t fn)
{
	task_t *tp = task_create_gid (gid, fn);
	task_set_arg (tp, (U16)ticks);
	return (tp);
}


void timer_init (void)
{
	pausable_timer_locks = 0;
}

