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

#include <unistd.h>
#include <freewpc.h>
#include <native/log.h>

/**
 * \file
 * \brief This module implements the round-robin, non-realtime, non-preemptive
 * task scheduler under Linux.  This uses the GNU pth library.
 * It exposes the same task_ API functions, with similar semantics.
 */

/* TODO:
   - Create tasks in advance to minimize repeated overhead of spawning.
*/

/** Enable this to turn on verbose debugging of the task subsystem. */
//#define PTHREAD_DEBUG

#ifdef PTHREAD_DEBUG
#include <sys/errno.h>
#define pthread_debug(args...) print_log(args)
#else
#define pthread_debug(args...)
#endif

bool task_dispatching_ok = TRUE;

U8 task_largest_stack = 0;

U8 task_count = 0;

U8 task_max_count = 0;

#ifdef CONFIG_SIM
extern int linux_irq_multiplier;
#else
#define linux_irq_multiplier 1
#endif

#ifdef CONFIG_SIM
extern void ui_write_task (int, task_gid_t);
#endif

/* Some WPC per-task data must be stored separately, outside of the pth
 * context.  The aux_task_data_t structure holds this. */
typedef struct
{
	pthread_t pid;
	task_gid_t gid;
	PTR_OR_U16 arg;
	U8 duration;
	unsigned char class_data[32];
} aux_task_data_t;

aux_task_data_t task_data_table[NUM_TASKS];

pthread_attr_t attr_default;
pthread_attr_t attr_task;
pthread_attr_t attr_input;
pthread_attr_t attr_interrupt;


void task_dump (void)
{
}


void idle_profile_rtt (void)
{
}


/**
 * The main function for creating a new task.
 */
task_pid_t task_create_gid (task_gid_t gid, task_function_t fn)
{
	pthread_t pid;
	pthread_attr_t *attr;
	struct sched_param param;
	int i;
	int rc;

	pthread_debug ("task_create_gid: gid=%d, fn=%p\n", gid, fn);

	if (gid == GID_LINUX_REALTIME) /* time tracking */
		attr = &attr_interrupt;
	else if (gid == GID_LINUX_INTERFACE) /* user input */
		attr = &attr_input;
	else
		attr = &attr_task;

	/* TODO - inside of calling the function directly, call a global
	 * function and pass it a pointer to the task_data_table entry
	 * as an argument. */
	rc = pthread_create (&pid, attr, fn, 0);
	if (rc != 0)
	{
		pthread_debug ("pthread_create failed, errno=%u\n", errno);
		fatal (ERR_NO_FREE_TASKS);
	}

	for (i=0; i < NUM_TASKS; i++)
		if (task_data_table[i].pid == 0)
		{
			task_data_table[i].pid = pid;
			task_data_table[i].gid = gid;
			task_data_table[i].duration = TASK_DURATION_INF;
			task_data_table[i].arg.u16 = 0;
			task_data_table[i].duration = TASK_DURATION_BALL;
#ifdef CONFIG_SIM
			ui_write_task (i, gid);
#endif
			pthread_debug ("pthread_create: index=%d, pid=%u\n", i, (unsigned)pid);
			return (pid);
		}

	fatal (ERR_NO_FREE_TASKS);
}


void task_setgid (task_gid_t gid)
{
	int i;
	for (i=0; i < NUM_TASKS; i++)
		if (task_data_table[i].pid == task_getpid ())
		{
			task_data_table[i].gid = gid;
			break;
		}
}

void task_sleep (task_ticks_t ticks)
{
	usleep (ticks * USECS_PER_TICK);
}


void task_sleep_sec1 (U8 secs)
{
	usleep (TIME_1S * secs * USECS_PER_TICK);
}

__noreturn__
void task_exit (void)
{
	int i;

	pthread_debug ("task_exit: pid=%u\n", (unsigned)task_getpid ());
	for (i=0; i < NUM_TASKS; i++)
		if (task_data_table[i].pid == task_getpid ())
		{
			task_data_table[i].pid = 0;
#ifdef CONFIG_SIM
			ui_write_task (i, 0);
#endif
			pthread_debug ("pthread_exit: index=%d\n", i);
			for (;;)
				pthread_exit (0);
		}
	fatal (ERR_TASK_KILL_FAILED);
}

task_pid_t task_find_gid (task_gid_t gid)
{
	int i;
	for (i=0; i < NUM_TASKS; i++)
	{
		if ((task_data_table[i].gid == gid)
			&& (task_data_table[i].pid != 0))
			return task_data_table[i].pid;
	}
	return 0;
}


task_pid_t task_find_gid_next (task_pid_t last, task_gid_t gid)
{
	int i;
	int ok_to_return = 0;
	for (i=0; i < NUM_TASKS; i++)
	{
		if ((task_data_table[i].gid == gid) && (task_data_table[i].pid != 0))
		{
			if (ok_to_return)
				return task_data_table[i].pid;
			else if (task_data_table[i].pid == last)
				ok_to_return = 1;
		}
	}
	return 0;
}


void task_kill_pid (task_pid_t tp)
{
	int i;

	pthread_debug ("task_kill_pid: pid=%u\n", (unsigned)tp);

	for (i=0; i < NUM_TASKS; i++)
		if (task_data_table[i].pid == tp)
		{
			if (tp != 0)
				pthread_cancel (tp);
			task_data_table[i].pid = 0;
#ifdef CONFIG_SIM
			ui_write_task (i, 0);
#endif
			return;
		}
}

bool task_kill_gid (task_gid_t gid)
{
	int i;
	bool rc = FALSE;

	for (i=0; i < NUM_TASKS; i++)
	{
		if ((task_data_table[i].gid == gid) &&
			 (task_data_table[i].pid != 0) &&
			 (task_data_table[i].pid != task_getpid ()))
		{
			task_kill_pid (task_data_table[i].pid);
			rc = TRUE;
		}
	}
	return (rc);
}


void task_duration_expire (U8 cond)
{
	int i;

	for (i=0; i < NUM_TASKS; i++)
	{
		if ((task_data_table[i].pid != 0) &&
			 (task_data_table[i].duration & cond))
			task_kill_pid (task_data_table[i].pid);
	}
}

void task_set_duration (task_pid_t tp, U8 cond)
{
	int i;
	for (i=0; i < NUM_TASKS; i++)
	{
		if (task_data_table[i].pid == tp)
		{
			task_data_table[i].duration = cond;
			break;
		}
	}
}


void task_add_duration (U8 flags)
{
	int i;
	for (i=0; i < NUM_TASKS; i++)
	{
		if (task_data_table[i].pid == task_getpid ())
		{
			task_data_table[i].duration |= flags;
			break;
		}
	}
}


void task_remove_duration (U8 flags)
{
	int i;
	for (i=0; i < NUM_TASKS; i++)
	{
		if (task_data_table[i].pid == task_getpid ())
		{
			task_data_table[i].duration &= ~flags;
			break;
		}
	}
}


U16 task_get_arg (void)
{
	int i;
	for (i=0; i < NUM_TASKS; i++)
	{
		if (task_data_table[i].pid == task_getpid ())
			return task_data_table[i].arg.u16;
	}
	fatal (ERR_CANT_GET_HERE);
}


void *task_get_pointer_arg (void)
{
	int i;
	for (i=0; i < NUM_TASKS; i++)
	{
		if (task_data_table[i].pid == task_getpid ())
			return task_data_table[i].arg.ptr;
	}
	fatal (ERR_CANT_GET_HERE);
}


void task_set_arg (task_pid_t tp, U16 arg)
{
	int i;
	for (i=0; i < NUM_TASKS; i++)
	{
		if (task_data_table[i].pid == tp)
		{
			task_data_table[i].arg.u16 = arg;
		}
	}
}


void task_set_pointer_arg (task_pid_t tp, void *arg)
{
	int i;
	for (i=0; i < NUM_TASKS; i++)
	{
		if (task_data_table[i].pid == tp)
		{
			task_data_table[i].arg.ptr = arg;
		}
	}
}


task_pid_t task_getpid (void)
{
	return pthread_self ();
}


task_gid_t task_getgid (void)
{
	int i;
	for (i=0; i < NUM_TASKS; i++)
	{
		if (task_data_table[i].pid == task_getpid ())
			return task_data_table[i].gid;
	}
	return 255;
}


void *task_get_class_data (task_pid_t pid)
{
	int i;

	for (i=0; i < NUM_TASKS; i++)
		if (task_data_table[i].pid == pid)
			return task_data_table[i].class_data;
	printf ("task_get_class_data for pid %u failed\n", (unsigned)pid);
	fatal (0xFD);
}

void task_set_class_data (task_pid_t pid, size_t size)
{
}


void task_init (void)
{
	memset (task_data_table, 0, sizeof (task_data_table));

	struct sched_param sched_param;
	sched_param.sched_priority = 1;
	sched_setscheduler (0, SCHED_FIFO, &sched_param);

	task_data_table[0].pid = task_getpid ();
	task_data_table[0].gid = GID_FIRST_TASK;
	task_data_table[0].duration = TASK_DURATION_INF;

	pthread_attr_init (&attr_default);

	attr_task = attr_default;
	pthread_attr_setdetachstate (&attr_task, PTHREAD_CREATE_DETACHED);
	pthread_attr_setinheritsched (&attr_task, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy (&attr_task, SCHED_FIFO);
	sched_param.sched_priority = 1;
	pthread_attr_setschedparam (&attr_task, &sched_param);

	attr_input = attr_task;
	sched_param.sched_priority = 2;
	pthread_attr_setschedparam (&attr_input, &sched_param);

	attr_interrupt = attr_task;
	sched_param.sched_priority = 8;
	pthread_attr_setschedparam (&attr_interrupt, &sched_param);
}
