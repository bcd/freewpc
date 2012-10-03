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

#include <freewpc.h>


bool task_dispatching_ok = TRUE;

U8 task_largest_stack = 0;

U8 task_count = 0;

U8 task_max_count = 0;


/* Some WPC per-task data must be stored separately, outside of the pth
 * context.  The aux_task_data_t structure holds this. */
aux_task_data_t task_data_table[NUM_TASKS];



void idle_profile_rtt (void)
{
}


/* Lookup the aux structure for a given PID */
aux_task_data_t *aux_task_find_pid (task_pid_t pid)
{
	aux_task_data_t *auxp = task_data_table;
	while (auxp < &task_data_table[NUM_TASKS])
	{
		if (auxp->pid == pid)
			return auxp;
		auxp++;
	}
	return NULL;
}


void task_setgid (task_gid_t gid)
{
	aux_task_data_t *auxp = aux_task_find_pid (task_getpid ());
	if (auxp)
	{
		auxp->gid = gid;
	}
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
	return PID_NONE;
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
	return PID_NONE;
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


void ntask_init (void)
{
	memset (task_data_table, 0, sizeof (task_data_table));
	task_data_table[0].pid = task_getpid ();
	task_data_table[0].gid = GID_FIRST_TASK;
	task_data_table[0].duration = TASK_DURATION_INF;
}


