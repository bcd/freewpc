
#include <freewpc.h>

/*
 * Timers are special cases of tasks; they just all run the same
 * function, counting down the time, then exiting.
 */

#define TIMER_GRAN	(TIME_100MS * 5)


static __taskentry__ void timer_function (void)
{
	U16 ticks = task_get_arg ();
	while (ticks > 0)
	{
		task_sleep (TIMER_GRAN);
		ticks -= TIMER_GRAN;
	}
	task_exit ();
}


bool timer_kill_gid (task_gid_t gid)
{
	return task_kill_gid (gid);
}


void timer_kill_pid (task_t *tp)
{
	task_kill_pid (tp);
}


task_t *timer_find_gid (task_gid_t gid)
{
	return task_find_gid (gid);
}


void timer_restart (task_gid_t gid, task_ticks_t ticks)
{
	task_t *tp = task_recreate_gid (gid, timer_function);
	return (tp);
}


task_t *timer_start1 (task_gid_t gid, task_ticks_t ticks)
{
	task_t *tp = task_create_gid1 (gid);
}


task_t *timer_start (task_gid_t gid, task_ticks_t ticks)
{
	return task_create_gid ();
}


void timer_init (void)
{
}

