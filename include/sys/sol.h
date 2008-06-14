/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SYS_SOL_H
#define _SYS_SOL_H

typedef U8 solnum_t;

#define SOL_COUNT 48

#define SOL_BASE_HIGH 0
#define SOL_BASE_LOW 8
#define SOL_BASE_GENERAL 16
#define SOL_BASE_AUXILIARY 24
#define SOL_BASE_FLIPTRONIC 32
#define SOL_BASE_EXTENDED 40

extern __fastram__ U8 sol_timers[];
extern U8 sol_duty_state[];


/** Duty cycle values.  The first value is the on time,
the second value is the off time */
#define SOL_DUTY_0		0x0
#define SOL_DUTY_12		0x40
#define SOL_DUTY_25		0x22
#define SOL_DUTY_50		0x55
#define SOL_DUTY_75		0x77
#define SOL_DUTY_100		0xFF

#define SOL_DUTY_DEFAULT   SOL_DUTY_25
#define SOL_TIME_DEFAULT   TIME_133MS

/* Function prototypes */
void sol_start_real (solnum_t sol, U8 cycle_mask, U8 ticks);
void sol_stop (solnum_t sol);
void sol_init (void);

/* sol_start is a macro because the 'time' value must be scaled
to the correct resolution.  Ticks are normally 1 per 16ms, but
we need 1 per 4ms for solenoids, so scale accordingly. */
#define sol_start(sol,mask,time) \
	sol_start_real (sol, mask, (4 * time))

/* Standard flasher pulse is full strength, for 33ms */
#define flasher_pulse(id) sol_start (id, SOL_DUTY_100, TIME_33MS)

/* Standard solenoid pulse is full strength for 100ms */
#define sol_pulse(id)      sol_start(id, SOL_DUTY_100, TIME_100MS)

#endif /* _SYS_SOL_H */
