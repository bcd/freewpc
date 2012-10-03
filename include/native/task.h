/*
 * Copyright 2012 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _SYS_TASK_H
#define _SYS_TASK_H

extern bool task_dispatching_ok;

/** The maximum number of tasks that can be running at once.
 * Space for this many task structures is statically allocated. */
#define NUM_TASKS 48

#define TASK_DURATION_INF 0x0
#define TASK_DURATION_LIVE 0x1
#define TASK_DURATION_BALL 0x3
#define TASK_DURATION_GAME 0x7
#define TASK_DURATION_TEST 0x8

#include <sys/time.h>
#define USECS_PER_TICK (16000 / linux_irq_multiplier)
#if defined(CONFIG_PTH)
#include <pth.h>
typedef pth_t task_pid_t;
#elif defined(CONFIG_PTHREADS)
#undef __noreturn__
#include <pthread.h>
#define __noreturn__ __attribute__((noreturn))
typedef pthread_t task_pid_t;
#else
typedef int task_pid_t;
#endif

typedef unsigned int task_gid_t;
typedef unsigned int task_ticks_t;
typedef void (*task_function_t) (void);
#define task_set_rom_page(pid, page)

typedef struct
{
	pthread_t pid;
	task_gid_t gid;
	PTR_OR_U16 arg;
	U8 duration;
	unsigned char class_data[32];
} aux_task_data_t;

extern aux_task_data_t task_data_table[NUM_TASKS];



/********************************/
/*     Function Prototypes      */
/********************************/

#ifdef CONFIG_SIM
void ui_write_task (int, task_gid_t);
#else
extern inline void ui_write_task (int t, task_gid_t g) {}
#endif /* CONFIG_SIM */
void task_dump (void);
void task_init (void);
void task_create (void);
task_pid_t task_create_gid (task_gid_t, task_function_t fn);
task_pid_t task_create_gid1 (task_gid_t, task_function_t fn);
task_pid_t task_recreate_gid (task_gid_t, task_function_t fn);
task_pid_t task_getpid (void);
task_gid_t task_getgid (void);
void task_setgid (task_gid_t gid);
void task_sleep (task_ticks_t ticks);
void task_sleep_sec1 (U8 secs);
void task_set_periodic (task_ticks_t ticks);
void task_sleep_periodic (void);
__noreturn__ void task_exit (void);
task_pid_t task_find_gid (task_gid_t);
task_pid_t task_find_gid_next (task_pid_t first, task_gid_t gid);
void task_kill_pid (task_pid_t tp);
bool task_kill_gid (task_gid_t);
void task_kill_all (void);
void task_duration_expire (U8);
void task_set_duration (task_pid_t pid, U8 cond);
void task_add_duration (U8 cond);
void task_remove_duration (U8 cond);
U16 task_get_arg (void);
void task_set_arg (task_pid_t tp, U16 arg);
void *task_get_pointer_arg (void);
void task_set_pointer_arg (task_pid_t tp, void *arg);
__noreturn__ void task_dispatcher (task_pid_t tp);
task_pid_t task_getpid (void);
task_gid_t task_getgid (void);
void do_periodic (void);

/** Create a new task that has the same group ID as the current one. */
#define task_create_peer(fn)		task_create_gid (task_getgid (), fn)

/** Create a new task with group ID zero (anonymous) */
#define task_create_anon(fn)		task_create_gid (0, fn)

/** Kill all of the other tasks that have the same group ID as
 * the current one. */
#define task_kill_peers()			task_kill_gid (task_getgid ())

/** Yield control to another task, but do not impose a minimum sleep time. */
#if defined(CONFIG_PTH)
#define task_yield() pth_yield(0)
#elif defined(CONFIG_PTHREADS)
#define task_yield() sched_yield()
#else
#define task_yield() task_sleep (0)
#endif

/** Sleep for an integer number of seconds */
extern inline void task_sleep_sec (U8 n)
{
	if (n <= 4)
		task_sleep (TIME_1S * n);
	else
		task_sleep_sec1 (n);
}


/** Go into an infinite waiting loop, without exiting. */
extern inline void task_suspend (void)
{
	/* Use 4s since that is about the maximum time that the task API
	supports in one call. */
	for (;;)
		task_sleep (TIME_4S);
}


/**
 * Note that a task may have run for a long period of time at this point.
 *
 * This call is intended to keep the software watchdog from expiring,
 * in places where it might because a task is doing a long computation.
 */
extern inline void task_runs_long (void)
{
	/* Manually reset the watchdog flag */
	extern U8 task_dispatching_ok;
	task_dispatching_ok = TRUE;
}


#define task_create_gid_while(gid, fn, cond) \
	({ task_pid_t pid = task_create_gid (gid, fn); \
		task_set_duration (pid, cond); \
		pid; })

#define task_recreate_gid_while(gid, fn, cond) \
	({ task_pid_t pid = task_recreate_gid (gid, fn); \
		task_set_duration (pid, cond); \
		pid; })

#define task_create_gid1_while(gid, fn, cond) \
	({ task_pid_t pid = task_create_gid1 (gid, fn); \
		task_set_duration (pid, cond); \
		pid; })


extern inline task_pid_t
far_task_create_gid (task_gid_t gid, task_function_t fn, U8 page)
{
	task_pid_t pid = task_create_gid (gid, fn);
	task_set_rom_page (pid, page);
	return pid;
}

extern inline task_pid_t
far_task_create_gid1 (task_gid_t gid, task_function_t fn, U8 page)
{
	task_pid_t pid = task_create_gid1 (gid, fn);
	task_set_rom_page (pid, page);
	return pid;
}

extern inline task_pid_t
far_task_recreate_gid (task_gid_t gid, task_function_t fn, U8 page)
{
	task_pid_t pid = task_recreate_gid (gid, fn);
	task_set_rom_page (pid, page);
	return pid;
}

/** A macro for defining a block of code that runs periodically.
 * The timing is better than if you just called task_sleep() in the loop,
 * because it accounts for the time it takes the code block itself
 * to finish.
 */
#define task_do_periodic(time) \
	for (task_set_periodic (time);; task_sleep_periodic (), task_set_periodic (time))

extern void *task_get_class_data (task_pid_t);
extern void task_set_class_data (task_pid_t, size_t);

#define task_class_data(tp, type) ((type *)task_get_class_data (tp))

#define task_init_class_data(tp, type) ({ task_set_class_data (tp, sizeof (type)); task_class_data (tp, type); })

/** Returns the pointer to the class data for the current task. */
#define task_current_class_data(type) task_class_data(task_getpid (), type)

/** Copies the class data from the currently running task to a recently
 * created task.  The new task is said to inherit the data from the
 * current one. */
#define task_inherit_class_data(tp, type) \
do { \
	type *src = task_current_class_data (type); \
	type *dst = task_init_class_data (tp, type); \
	memcpy (dst, src, sizeof (type)); \
} while (0)

#endif /* _SYS_TASK_H */
