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

#define timer_restart_free(g,t)	timer_restart(g,t,freerunning_timer_function)
#define timer_start1_free(g,t)	timer_start1(g,t,freerunning_timer_function)
#define timer_start_free(g,t)	t	imer_start(g,t,freerunning_timer_function)

#endif /* _TIMER_H */

