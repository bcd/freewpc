/*
 * Copyright 2006, 2007, 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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

/* Round to the next multiple of 4, because the IRQ-level function
 * has an unrolled loop that requires this. */
#define MAX_FREE_TIMERS (4 * ((MAX_TIMERS + 3) / 4))

task_pid_t timer_restart (task_gid_t gid, U16 ticks, task_function_t fn);
task_pid_t timer_start1 (task_gid_t gid, U16 ticks, task_function_t fn);
task_pid_t timer_start (task_gid_t gid, U16 ticks, task_function_t fn);
void timer_init (void);

void timer_lock (void);
void timer_unlock (void);
bool system_timer_pause (void);
void timer_pause_second (void);

void __free_timer_restart (U8 tid, U8 ticks);
void __free_timer_start (U8 tid, U8 ticks);
void __free_timer_stop (U8 tid);
U8 __free_timer_test (U8 tid);
void free_timer_init (void);

#define free_timer_restart(tid, ticks)  __free_timer_restart (__addrval (&tid), ticks)
#define free_timer_start(tid, ticks)    __free_timer_start (__addrval (&tid), ticks)
#define free_timer_stop(tid)            __free_timer_stop(__addrval (&tid))
#define free_timer_test(tid)            __free_timer_test(__addrval (&tid))

#define timer_kill_gid 	task_kill_gid
#define timer_kill_pid 	task_kill_pid
#define timer_find_gid	task_find_gid
#define timer_test_and_kill_gid	task_kill_gid

void freerunning_timer_function (void);

#define timer_restart_free(g,t)	timer_restart(g,t,freerunning_timer_function)
#define timer_start1_free(g,t)	timer_start1(g,t,freerunning_timer_function)
#define timer_start_free(g,t)		timer_start(g,t,freerunning_timer_function)

#endif /* _TIMER_H */

