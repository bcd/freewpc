/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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
 */

#define PTHDEBUG

bool task_dispatching_ok = TRUE;

U8 task_largest_stack = 0;

U8 task_count = 0;

U8 task_max_count = 0;

#define PTH_USECS_PER_TICK 16000

#define MAX_TASKS 256

pth_key_t pth_key_arg;
pth_key_t pth_key_flags;


pth_t pth_gid_table[MAX_TASKS];


void task_dump (void)
{
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
		if (pth_gid_table[i] == 0)
		{
			pth_gid_table[i] = pid;
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
	fatal (ERR_NOT_IMPLEMENTED_YET);
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
#ifdef PTHDEBUG
	printf ("task_exit: pid=%p\n", task_getpid ());
#endif
	pth_exit (0);
}

task_pid_t task_find_gid (task_gid_t gid)
{
	fatal (ERR_NOT_IMPLEMENTED_YET);
}

task_pid_t task_find_gid_data (task_gid_t gid, U8 off, U8 val)
{
	fatal (ERR_NOT_IMPLEMENTED_YET);
}

void task_kill_pid (task_pid_t tp)
{
	pth_abort (tp);
}

bool task_kill_gid (task_gid_t gid)
{
	fatal (ERR_NOT_IMPLEMENTED_YET);
}


void task_kill_all (void)
{
	fatal (ERR_NOT_IMPLEMENTED_YET);
}


void task_set_flags (U8 flags)
{
	fatal (ERR_NOT_IMPLEMENTED_YET);
}


void task_clear_flags (U8 flags)
{
	fatal (ERR_NOT_IMPLEMENTED_YET);
}


U16 task_get_arg (void)
{
	fatal (ERR_NOT_IMPLEMENTED_YET);
}


void task_set_arg (task_pid_t tp, U16 arg)
{
	fatal (ERR_NOT_IMPLEMENTED_YET);
}


task_pid_t task_getpid (void)
{
	return pth_self ();
}


task_gid_t task_getgid (void)
{
	return 0;
}


U8 task_get_thread_data (task_pid_t pid, U8 n)
{
	fatal (ERR_NOT_IMPLEMENTED_YET);
}


void task_set_thread_data (task_pid_t pid, U8 n, U8 v)
{
}


void task_init (void)
{
	pth_init ();
	pth_key_create (&pth_key_arg, 0);
	pth_key_create (&pth_key_flags, 0);

	memset (pth_gid_table, 0, sizeof (pth_gid_table));
}

