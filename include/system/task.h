/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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

#ifdef CONFIG_NATIVE
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

#else /* !CONFIG_NATIVE */

#include <env.h>

/** Values for the 'state' field in the task structure */
#define BLOCK_FREE 0x0

/* Says that the block is in use */
#define BLOCK_USED  0x1

/* Says that the block is used by the 'malloc' function */
#define BLOCK_MALLOC 0x2

/* Says that the block is used by the task scheduler */
#define BLOCK_TASK 0x4

/* Says that the block is used to hold a task stack */
#define BLOCK_STACK	0x8

/* Says that the task is in the blocked state */
#define TASK_BLOCKED 0x10


/** Define the size of the saved process stack. */
#define TASK_STACK_SIZE 40



/** Type for the group ID (gid) */
typedef U8 task_gid_t;

/** Type for the number of task ticks.  1 task tick is the
 * smallest amount that a task can sleep. */
typedef U8 task_ticks_t;

/** Type for the entry point to a new task */
typedef void (*task_function_t) (void);

/** Type for the task structure.
 * For proper saving/restoring, only the registers that are live
 * across function calls need to be kept here.  The 6809 compiler
 * allows functions to trash D and X, so they aren't here -- just
 * U and Y.
 *
 * When adding/removing fields to this structure, adjust
 * TASK_STACK_SIZE above accordingly so that the total size is
 * suitably aligned.
 */
typedef struct task_struct
{
	/** The execution state of the task.  It can be BLOCK_FREE, if the
	 * task entry isn't being used at all; BLOCK_USED for a running/waiting
	 * task; or TASK_BLOCKED for a sleeping task. */
	U8				state;

	/** The index of the next block in the same chain.  Not used yet.
	 * A NULL is indicated by a -1, hence it is signed. */
	S8          chain;

	/** The task group ID.  This is a compile-time assigned value
	 * used to identify the task.   Multiple running tasks can also
	 * share the same group ID; operations on a group will affect
	 * *all* tasks with that ID, in most cases. */
	task_gid_t	gid;

	/** The saved PC for the task, while it is blocked */
	U16			pc;

	/** The saved Y register for the task, while it is blocked */
	U16			y;

	/** The saved U register for the task, while it is blocked */
	U16			u;

	/** The saved ROM page register for the task, while it is blocked */
	U8				rom_page;

	/** The amount of stack space, in bytes, that has been saved into
	 * the task's stack area */
	U8				stack_size;

	/** The time at which the task should be awakened, when it is blocked.
	More correctly, the system time must be greater than this value for the
	task to run again.  It will not run when the two are equal. */
	U16			wakeup;

	/** The task argument pointer.  This is a primitive way of passing
	 * initialization data to a new task.  The creator of the task can
	 * assign this argument pointer, after creating the task but before
	 * the next schedule. */
	/* TODO - few tasks need an arg, and this is wasting RAM for those
	 * that don't.  Better to push any arguments onto the new task's stack
	 * and declare the task function to accept those arguments, with
	 * an ellipsis to prevent assuming they are in registers. */
	PTR_OR_U16	arg;

	/** The index of an auxiliary memory block used to store
	 * additional stack data.  This is used when the stack space
	 * within the task block itself is not large enough.  When set
	 * to -1, it means there is no auxiliary storage. */
	U8				aux_stack_block;

	/** Says how long the task is allowed to exist, before being
	 * stopped automatically due to some external event. */
	U8				duration;

	/** Not currently used */
	U8				reserved;

	/** The task stack save area.  This is NOT used as the live stack
	 * area; the live stack is copied here when the task blocks.
	 * Because of this, tasks can use a much larger stack size if needed,
	 * as long as they don't try to block while holding that much space.
	 * Practically, this means that you shouldn't sleep in a deeply
	 * nested set of function calls. */
	U8				stack[TASK_STACK_SIZE];
} task_t;



/********************************/
/*     Inline Macros            */
/********************************/

extern inline task_t *task_getpid (void)
{
	extern task_t *task_current;
	return task_current;
}

extern inline task_gid_t task_getgid (void)
{
	return task_getpid ()->gid;
}

extern inline void task_set_rom_page (task_t *pid, U8 rom_page)
{
	pid->rom_page = rom_page;
}

/** A process ID, or PID, is just a pointer to the task block.
 * PIDs are rarely used as they are dynamic in value. */
typedef task_t *task_pid_t;

#endif /* CONFIG_NATIVE */


/********************************/
/*     Function Prototypes      */
/********************************/

#ifndef CONFIG_NATIVE
task_t *block_allocate (void);
void malloc_chunk_dump (task_t *task);
#endif
#ifdef CONFIG_MALLOC
void block_free (task_t *tp);
#endif

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
#ifdef CONFIG_NATIVE
task_pid_t task_getpid (void);
task_gid_t task_getgid (void);
#endif
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

#ifdef CONFIG_NATIVE

extern void *task_get_class_data (task_pid_t);
extern void task_set_class_data (task_pid_t, size_t);

#define task_class_data(tp, type) ((type *)task_get_class_data (tp))

#define task_init_class_data(tp, type) ({ task_set_class_data (tp, sizeof (type)); task_class_data (tp, type); })

#else /* !CONFIG_NATIVE */

#define TASK_DECL_ARGS(args...) args, ...

#define TASK_GET_ARG(arg) arg

#define task_top_of_stack(tp)	(&tp->stack[TASK_STACK_SIZE - tp->stack_size])

#define task_push_arg(tp, arg) \
do { \
	tp->stack_size += sizeof (arg); \
	*((typeof (arg) *)task_top_of_stack(tp)) = arg; \
} while (0)

#define task_push_args_done(tp) task_push_arg(tp, ((U16)task_exit))


/** Returns the pointer to the class data for a task.
 * This is a structure located at the top of the task's
 * stack in a fixed location, where it is visible to other
 * tasks as well.
 * tp identifies the task.  type is the C structure that says
 * how the area should be viewed.
 */
#define task_class_data(tp, type) ((type *)(&(tp)->stack[0]))

/** Initializes the class data area for a new task. */
#define task_init_class_data(tp, type) ({ task_class_data (tp, type); })

#endif /* CONFIG_NATIVE */

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
