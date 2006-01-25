
#ifndef _TIMER_H
#define _TIMER_H

task_t *timer_find_gid (task_gid_t gid);
task_t *timer_restart (task_gid_t gid, task_ticks_t ticks, task_function_t fn);
task_t *timer_start1 (task_gid_t gid, task_ticks_t ticks, task_function_t fn);
task_t *timer_start (task_gid_t gid, task_ticks_t ticks, task_function_t fn);
void timer_init (void);

#define timer_kill_gid 	task_kill_gid
#define timer_kill_pid 	task_kill_pid

#define timer_pause() \
do { \
	extern U8 pausable_timer_locks; pausable_timer_locks++; \
} while (0);

#define timer_resume() \
do { \
	extern U8 pausable_timer_locks; pausable_timer_locks--; \
} while (0);

__taskentry__ void freerunning_timer_function (void);
__taskentry__ void pausable_timer_function (void);

#endif /* _TIMER_H */

