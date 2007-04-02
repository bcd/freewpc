/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief This module implements the round-robin, non-realtime, non-preemptive
 * task scheduler under Linux.  This uses the GNU pth library.
 * It exposes the same task_ API functions, with similar semantics.
 */


/** Enable this to turn on verbose debugging of the task subsystem. */
//#define PTHDEBUG

bool task_dispatching_ok = TRUE;

U8 task_largest_stack = 0;

U8 task_count = 0;

U8 task_max_count = 0;

extern int linux_irq_multiplier;

#define PTH_USECS_PER_TICK (16000 / linux_irq_multiplier)

#define MAX_TASKS 32


/* Some WPC per-task data must be stored separately, outside of the pth
 * context.  The aux_task_data_t structure holds this. */
typedef struct
{
	pth_t pid;
	task_gid_t gid;
	PTR_OR_U16 arg;
	U8 flags;
	U8 thread_data[4];
} aux_task_data_t;

aux_task_data_t task_data_table[MAX_TASKS];


void task_dump (void)
{
	int i;

	printf ("\nPID         GID   ARG    FLAGS\n");
	for (i=0; i < MAX_TASKS; i++)
	{
		aux_task_data_t *td = &task_data_table[i];

		if (td->pid != 0)
		{
			printf ("%p%c   %d    %08X   %02X\n",
				td->pid, 
				(td->pid == task_getpid ()) ? '*' : ' ', 
				td->gid, td->arg, td->flags);
		}
	}
}


void task_inherit_thread_data (task_pid_t pid)
{
	int i, j;
		
	for (i=0; i < MAX_TASKS; i++)
	{
		if (task_data_table[i].pid == pid)
		{
			for (j=0; j < 4; i++)
				task_data_table[i].thread_data[j] = 0; /* TODO */
			return;
		}
	}
}


task_pid_t task_create_gid (task_gid_t gid, task_function_t fn)
{
	pth_t pid;
	pth_attr_t attr;
	int i;

#ifdef PTHDEBUG
	printf ("task_create_gid: gid=%d, fn=%p\n", gid, fn);
#endif

	attr = pth_attr_new ();
	pth_attr_set (attr, PTH_ATTR_JOINABLE, FALSE);
	pth_attr_set (attr, PTH_ATTR_CANCEL_STATE, PTH_CANCEL_ENABLE);

	pid = pth_spawn (attr, fn, 0);

	for (i=0; i < MAX_TASKS; i++)
		if (task_data_table[i].pid == 0)
		{
			task_data_table[i].pid = pid;
			task_data_table[i].gid = gid;
			task_data_table[i].flags = 0;
			task_data_table[i].arg = 0;
			task_data_table[i].thread_data[0] = 0;
			task_data_table[i].thread_data[1] = 0;
			task_data_table[i].thread_data[2] = 0;
			task_data_table[i].thread_data[3] = 0;
			break;
		}

	return (pid);
}

task_pid_t task_create_gid1 (task_gid_t gid, task_function_t fn) //2
{
	task_pid_t tp = task_find_gid (gid);
	if (tp) 
		return (tp);
	return task_create_gid (gid, fn);
}


task_pid_t task_recreate_gid (task_gid_t gid, task_function_t fn) //2
{
	task_kill_gid (gid);
	if (task_find_gid (gid))
		fatal (ERR_TASK_KILL_FAILED);
	return task_create_gid (gid, fn);
}

void task_setgid (task_gid_t gid)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
		if (task_data_table[i].pid == task_getpid ())
		{
			task_data_table[i].gid = gid;
			break;
		}
}

void task_sleep (task_ticks_t ticks)
{
#ifdef PTHDEBUG
	printf ("task_sleep(%d)\n", ticks);
#endif
	pth_nap (pth_time (0, ticks * PTH_USECS_PER_TICK));
}


void task_sleep_sec (int8_t secs)
{
	while (--secs >= 0)
		task_sleep (TIME_1S);
}


__naked__ __noreturn__ 
void task_exit (void)
{
	int i;
#ifdef PTHDEBUG
	printf ("task_exit: pid=%p\n", task_getpid ());
#endif
	for (i=0; i < MAX_TASKS; i++)
		if (task_data_table[i].pid == task_getpid ())
		{
			task_data_table[i].pid = 0;
			break;
		}
	for (;;)
		pth_exit (0);
}

task_pid_t task_find_gid (task_gid_t gid)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
	{
		if ((task_data_table[i].gid == gid)
			&& (task_data_table[i].pid != 0))
			return task_data_table[i].pid;
	}
	return NULL;
}

task_pid_t task_find_gid_data (task_gid_t gid, U8 off, U8 val)
{
	fatal (ERR_NOT_IMPLEMENTED_YET);
}

void task_kill_pid (task_pid_t tp)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
		if (task_data_table[i].pid == tp)
		{
			task_data_table[i].pid = 0;
			if (tp != 0)
				pth_abort (tp);
			return;
		}
}

bool task_kill_gid (task_gid_t gid)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
	{
		if ((task_data_table[i].gid == gid) &&
			 (task_data_table[i].pid != 0) &&
			 (task_data_table[i].pid != task_getpid ()))
			task_kill_pid (task_data_table[i].pid);
	}
}


void task_kill_all (void)
{
	int i;

	for (i=0; i < MAX_TASKS; i++)
	{
		if ((task_data_table[i].pid != 0) &&
			 (task_data_table[i].pid != task_getpid ()) &&
			 !(task_data_table[i].flags & TASK_PROTECTED))
			task_kill_pid (task_data_table[i].pid);
	}
}


void task_set_flags (U8 flags)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
	{
		if (task_data_table[i].pid == task_getpid ())
		{
			task_data_table[i].flags |= flags;
			break;
		}
	}
}


void task_clear_flags (U8 flags)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
	{
		if (task_data_table[i].pid == task_getpid ())
		{
			task_data_table[i].flags &= ~flags;
			break;
		}
	}
}


PTR_OR_U16 task_get_arg (void)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
	{
		if (task_data_table[i].pid == task_getpid ())
			return task_data_table[i].arg;
	}
	fatal (ERR_CANT_GET_HERE);
}


void task_set_arg (task_pid_t tp, PTR_OR_U16 arg)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
	{
		if (task_data_table[i].pid == tp)
		{
			task_data_table[i].arg = arg;
		}
	}
}


task_pid_t task_getpid (void)
{
	return pth_self ();
}


task_gid_t task_getgid (void)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
	{
		if (task_data_table[i].pid == task_getpid ())
			return task_data_table[i].gid;
	}
	return 255;
}


U8 task_get_thread_data (task_pid_t pid, U8 n)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
	{
		if (task_data_table[i].pid == pid)
			return task_data_table[i].thread_data[n];
	}
	fatal (ERR_CANT_GET_HERE);
}


void task_set_thread_data (task_pid_t pid, U8 n, U8 v)
{
	int i;
	for (i=0; i < MAX_TASKS; i++)
	{
		if (task_data_table[i].pid == pid)
		{
			task_data_table[i].thread_data[n] = v;
			return;
		}
	}
}


void task_set_rom_page (task_t *pid, U8 rom_page)
{
}


void task_init (void)
{
	memset (task_data_table, 0, sizeof (task_data_table));
	
	pth_init ();

	task_data_table[0].pid = task_getpid ();
	task_data_table[0].gid = GID_FIRST_TASK;
	task_data_table[0].flags = TASK_PROTECTED;
}

