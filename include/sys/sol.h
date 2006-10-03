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

#ifndef _SYS_SOL_H
#define _SYS_SOL_H

typedef U8 solnum_t;

#define SOL_COUNT 48

#define SOL_ARRAY_WIDTH	((SOL_COUNT + 8) / 8)

extern __fastram__ U8 sol_rt_state[SOL_ARRAY_WIDTH];

/** Number of stages in a duty cycle */
#define SOL_CYCLES 8

/** Duty cycle values */
#define SOL_DUTY_0		0x0
#define SOL_DUTY_12_88	0x40
#define SOL_DUTY_25_75	0x22
#define SOL_DUTY_50_50	0x55
#define SOL_DUTY_75_25	0x77
#define SOL_DUTY_100		0xFF

#define sol_on(id)     sol_modify(id, SOL_DUTY_100)
#define sol_off(id)    sol_modify(id, SOL_DUTY_0)
#define sol_pulse(id)  sol_modify_pulse(id, SOL_DUTY_100)

#define sol_serve(id)  sol_modify_pulse(MACHINE_BALL_SERVE_SOLENOID, SOL_DUTY_12_88)

void sol_rtt (void);
void sol_modify (solnum_t sol, U8 cycle_mask);
void sol_modify_pulse (solnum_t sol, U8 cycle_mask);
void sol_init (void);
void flasher_pulse (solnum_t n);
void flasher_rtt (void);
void flasher_init (void);

#endif /* _SYS_SOL_H */
