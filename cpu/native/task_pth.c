/*
 * Copyright 2006-2012 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>
#include <printf.h>

/**
 * \file
 * \brief This module implements the round-robin, non-realtime, non-preemptive
 * task scheduler under Linux.  This uses the GNU pth library.
 * It exposes the same task_ API functions, with similar semantics.
 */

/** Enable this to turn on verbose debugging of the task subsystem. */
//#define CONFIG_DEBUG_TASK


extern int linux_irq_multiplier;

#define PTH_USECS_PER_TICK (16000 / linux_irq_multiplier)


void task_dump (void)
{
#ifdef DEBUGGER
	int i;
	dbprintf ("PID         GID   ARG    FLAGS\n");
	for (i=0; i < NUM_TASKS; i++)
	{
		aux_task_data_t *td = &task_data_table[i];

		if (td->pid != 0)
		{
			dbprintf ("%p%c   %d    %08X   %02X\n",
				td->pid, 
				(td->pid == task_getpid ()) ? '*' : ' ', 
				td->gid, td->arg.u16, td->duration);
		}
	}
#endif
}



/**
 * The main function for creating a new task.
 */
task_pid_t task_create_gid (task_gid_t gid, task_function_t fn)
{
	pth_t pid;
	pth_attr_t attr;
	int i;

#ifdef CONFIG_DEBUG_TASK
	printf ("task_create_gid: gid=%d, fn=%p\n", gid, fn);
#endif

	/* Set the task attributes:
	 * - Not joinable : all tasks are independent
	 * - cancellable : task kill is permitted
	 * - priority : make certain tasks that the simulator itself
	 *   uses higher priority, and all others equal in priority.
	 */
	attr = pth_attr_new ();
	pth_attr_set (attr, PTH_ATTR_JOINABLE, FALSE);
	pth_attr_set (attr, PTH_ATTR_CANCEL_STATE, PTH_CANCEL_ENABLE);
	if (gid == GID_LINUX_REALTIME) /* time tracking */
		pth_attr_set (attr, PTH_ATTR_PRIO, PTH_PRIO_STD + 2);
	else if (gid == GID_LINUX_INTERFACE) /* user input */
		pth_attr_set (attr, PTH_ATTR_PRIO, PTH_PRIO_STD + 1);

	/* TODO - inside of calling the function directly, call a global
	 * function and pass it a pointer to the task_data_table entry
	 * as an argument. */
	pid = pth_spawn (attr, fn, 0);

	for (i=0; i < NUM_TASKS; i++)
		if (task_data_table[i].pid == 0)
		{
			task_data_table[i].pid = pid;
			task_data_table[i].gid = gid;
			task_data_table[i].arg.u16 = 0;
			task_data_table[i].duration = TASK_DURATION_BALL;
			ui_write_task (i, gid);
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
	pth_nap (pth_time (0, ticks * PTH_USECS_PER_TICK));
}


void task_sleep_sec1 (U8 secs)
{
	pth_nap (pth_time (0, secs * TIME_1S * PTH_USECS_PER_TICK));
}


__noreturn__ 
void task_exit (void)
{
	int i;
#ifdef CONFIG_DEBUG_TASK
	printf ("task_exit: pid=%p\n", task_getpid ());
#endif
	for (i=0; i < NUM_TASKS; i++)
		if (task_data_table[i].pid == task_getpid ())
		{
			task_data_table[i].pid = 0;
			ui_write_task (i, 0);
			for (;;)
				pth_exit (0);
		}
	fatal (ERR_TASK_KILL_FAILED);
}


void task_kill_pid (task_pid_t tp)
{
       int i;

#ifdef CONFIG_DEBUG_TASK
       printf ("task_kill_pid: pid=%p\n", tp);
#endif

       for (i=0; i < NUM_TASKS; i++)
               if (task_data_table[i].pid == tp)
               {
                       task_data_table[i].pid = 0;
                       ui_write_task (i, 0);
                       if (tp != 0)
                               pth_abort (tp);
                       return;
               }
}

task_pid_t task_getpid (void)
{
	return pth_self ();
}


void task_init (void)
{
	memset (task_data_table, 0, sizeof (task_data_table));
	
	pth_init ();

	task_data_table[0].pid = task_getpid ();
	task_data_table[0].gid = GID_FIRST_TASK;
	task_data_table[0].duration = TASK_DURATION_INF;
}

