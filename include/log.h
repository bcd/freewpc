/*
 * Copyright 2008-2010 by Brian Dominy <brian@oddchange.com>
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

#ifndef _LOG_H
#define _LOG_H

struct log_event
{
	/* The number of 100ms ticks since the last event was logged.
	This can be as large as 250, for 25 seconds.  A value of 0xFF
	means that more than 25 seconds elapsed. */
	U8 timestamp;

	/* The module ID (upper 4-bits) and event ID (lower 4-bits) */
	U16 module_event;

	/* An event-specific 8-bit argument */
	U8 arg;
};

#define MAX_LOG_EVENTS 128

extern void log_init (void);
extern void log_event1(U16 module_event, U8 arg);
extern __permanent__ U16 prev_log_callset;

/* Logging has been disabled by default, as this feature has not
been used and is not very useful.  It can be turned back on via
CONFIG_LOG if anyone cares. */
#ifdef CONFIG_LOG
#define log_event(severity, module, event, arg) \
	log_event1(make_module_event (module, event), arg)
#else
#define log_event(severity, module, event, arg)
#endif

#define make_module_event(module, event) (((U16)(module) << 8UL) | event)
#define module_part(module_event) ((U8)((module_event) >> 8))
#define event_part(module_event) ((U8)((module_event) & 0xFF))

#define log_set_min_severity(severity)

#define log_set_wrapping(wrap_flag)

#define SEV_ERROR 0
#define SEV_WARN 1
#define SEV_INFO 2
#define SEV_DEBUG 3

#define MIN_SEVERITY SEV_DEBUG


#define EV_START 0
#define EV_STOP 1
#define EV_EXIT 2

#define MOD_DEFF 0 /* done */
	#define EV_DEFF_START EV_START
	#define EV_DEFF_STOP EV_STOP
	#define EV_DEFF_EXIT EV_EXIT
	#define EV_DEFF_RESTART 4
	#define EV_DEFF_TIMEOUT 5

#define MOD_LAMP 1
	#define EV_LEFF_START 0 /* done */
	#define EV_LEFF_STOP 1 /* done */
	#define EV_LEFF_EXIT 2 /* done */
	#define EV_LAMP_ON 4
	#define EV_LAMP_OFF 5
	#define EV_LAMP_FLASH 6
	#define EV_BIT_ON 7 /* done */
	#define EV_BIT_OFF 8 /* done */
	#define EV_BIT_TOGGLE 9 /* done */
	#define EV_GLOBAL_BIT_ON 10
	#define EV_GLOBAL_BIT_OFF 11

#define MOD_SOUND 2
	#define EV_SOUND_START 0
	#define EV_SOUND_STOP 1
	#define EV_SOUND_RESULT 4

#define MOD_TASK 3
	#define EV_TASK_START 0 /* done */
	#define EV_TASK_KILL 1 /* done */
	#define EV_TASK_EXIT 2 /* done */
	#define EV_TASK_RESTART 3
	#define EV_TASK_START1 4
	#define EV_TASK_100MS 5

#define MOD_SWITCH 4
	#define EV_SW_SCHEDULE 0 /* done */
	#define EV_SW_BLIP 4
	#define EV_SW_SHORT 5

#define MOD_TRIAC 5 /* done */
	#define EV_TRIAC_ON 0
	#define EV_TRIAC_OFF 1

#define MOD_GAME 6
	#define EV_GAME_START 0 /* done */
	#define EV_GAME_END 1 /* done */

	#define EV_PLAYER_ADD 4
	#define EV_BALL_START 5
	#define EV_BALL_VALID 6
	#define EV_BALL_END 7
	#define EV_PLAYER_END 8
	#define EV_TILT 9
	#define EV_SLAM_TILT 10

#define MOD_SYSTEM 7
	#define EV_SYSTEM_INIT 0 /* done */
	#define EV_SYSTEM_NONFATAL 4 /* done */
	#define EV_SYSTEM_FATAL 5 /* done */

#define MOD_PRICING 8
	#define EV_PRICING_ADD_CREDIT 0

#define MOD_SOL 9
	#define EV_SOL_START 0 /* done */
	#define EV_SOL_STOP 1 /* done */
	#define EV_DEV_ENTER 4
	#define EV_DEV_KICK 5
	#define EV_DEV_LOCK 6
	#define EV_DEV_UNLOCK 7

#endif /* _LOG_H */
