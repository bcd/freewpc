
#include <freewpc.h>

/* These are the portable task functions, which are common to all
   implementations of the task layer. */

task_pid_t task_create_gid1 (task_gid_t gid, task_function_t fn)
{
	task_pid_t tp = task_find_gid (gid);
	if (tp) 
		return (tp);
	return task_create_gid (gid, fn);
}


task_pid_t task_recreate_gid (task_gid_t gid, task_function_t fn)
{
	task_kill_gid (gid);
#ifdef PARANOID
	if (task_find_gid (gid))
		fatal (ERR_TASK_KILL_FAILED);
#endif
	return task_create_gid (gid, fn);
}

