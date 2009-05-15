/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SYS_ERRNO_H
#define _SYS_ERRNO_H


#define ERR_NO_FREE_TASKS 			1   /* Too many tasks are running */
#define ERR_TASK_STACK_OVERFLOW 	2   /* A process overflowed its stack */
#define ERR_TASK_KILL_CURRENT 	3   /* Task tried to commit suicide */
#define ERR_TASK_REGISTER_SAVE 	4
#define ERR_NMI 						5   /* NMI was taken */
#define ERR_SWI 						6   /* SWI was taken */
#define ERR_SWI2 						7   /* SWI2 was taken */
#define ERR_SWI3 						8   /* SWI3 was taken */
#define ERR_IDLE_CANNOT_SLEEP 	9
#define ERR_IDLE_CANNOT_EXIT 		10
#define ERR_INVALID_LAMP_NUM		11
#define ERR_NO_FREE_TIMERS			12
#define ERR_DEFF_QUEUE_FULL		13
#define ERR_TASK_KILL_FAILED		14
#define ERR_NOT_IMPLEMENTED_YET	15
#define ERR_UNPRINTABLE_CHAR		16
#define ERR_TASK_LOCKUP				18
#define ERR_REAL_HARDWARE			19
#define ERR_NVRAM_UNLOCKED			20
#define ERR_SOL_CKSUM_ERROR      21
#define ERR_UNLOCK_EMPTY_DEVICE  23
#define ERR_LOCK_FULL_DEVICE     24
#define ERR_INVALID_LEFF_CONFIG  25
#define ERR_FAILED_KICK				26
#define ERR_KICK_TOO_MANY        27
#define ERR_KICK_CAUSED_INCREASE 28
#define ERR_IDLE_BALL_LOST       29
#define ERR_DEVICE_NO_CHANGE     30
#define ERR_CANT_GET_HERE        31
#define ERR_INVALID_IO_ADDR      32
#define ERR_SCORE_NOT_IN_GAME    33
#define ERR_NO_MORE_SLOW_TIMERS  34
#define ERR_WINDOW_STACK_OVERFLOW 35
#define ERR_LIBC_ABORT           36
#define ERR_MALLOC               37
#define ERR_MUST_BE_LEFF         38
#define ERR_INVALID_MATRIX       39
#define ERR_TASK_STACK_UNDERFLOW 40
#define ERR_SWITCH_SLOW_SERVICE  41
#define ERR_SWITCH_QUEUE_CORRUPT 42
#define ERR_SOL_REQUEST          43
#define ERR_VIRTUAL_DEVICE_COUNT 44

#ifndef __ASSEMBLER__

typedef U8 errcode_t;

__noreturn__ void fatal (errcode_t errcode);
void nonfatal (errcode_t errcode);

#endif /* __ASSEMBLER__ */

#endif /* SYS_ERRNO_H */
