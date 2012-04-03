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

#ifndef _SYS_SOL_H
#define _SYS_SOL_H

typedef U8 solnum_t;

#ifndef PINIO_NUM_SOLS
#error "PINIO_NUM_SOLS undefined"
#endif

#define SOL_REG_COUNT MULTIPLEOF8(PINIO_NUM_SOLS)

#ifndef SOL_MIN_FLASHER
#define SOL_MIN_FLASHER 0
#endif

extern __fastram__ U8 sol_timers[];
extern U8 sol_duty_state[];
extern U8 sol_pulsing;

/** Duty cycle values.  Each '1' bit represents a
time quantum during which the coil on.  The more '1's,
the more powerful the pulse. */
#define SOL_DUTY_0      0x0
#define SOL_DUTY_12     0x40   /* 1/8 */
#define SOL_DUTY_25     0x22   /* 1/4 */
#define SOL_DUTY_37     0x92   /* 3/8 */
#define SOL_DUTY_50     0x55   /* 1/2 */
#define SOL_DUTY_62     0xB5   /* 5/8 */
#define SOL_DUTY_75     0x77   /* 3/4 */
#define SOL_DUTY_100    0xFF

/** The default solenoid timing */
#define SOL_TIME_DEFAULT   64
#define SOL_DUTY_DEFAULT   SOL_DUTY_100

/** The default flasher timing.  The TIME_ constants are not
 * used here; give the value in milliseconds. */
#define FLASHER_TIME_DEFAULT 24
#define FLASHER_DUTY_DEFAULT SOL_DUTY_100

/* Function prototypes */
void sol_req_start_specific (U8 sol, U8 mask, U8 time);
void sol_request_async (U8 sol);
void sol_request (U8 sol);
void sol_start_real (solnum_t sol, U8 cycle_mask, U8 ticks);
void sol_stop (solnum_t sol);
void sol_init (void);

/* sol_start is a wrapper function, because the 'time' value must be scaled
to the correct resolution.  Ticks are normally 1 per 16ms, but
we need 1 per 4ms for solenoids, so scale accordingly. */
__attribute_deprecated__
extern inline void sol_start (U8 sol, U8 mask, U8 time)
{
	sol_start_real (sol, mask, (4 * time));
}

/*
 * Pulse a flasher with a flexible time/duty cycle.
 */
extern inline void flasher_start (U8 sol, U8 mask, U8 time)
{
	sol_start_real (sol, mask, time / 4);
}

/*
 * Pulse a flasher for the default time.
 */
extern inline void flasher_pulse (U8 sol)
{
	flasher_start (sol, FLASHER_DUTY_DEFAULT, FLASHER_TIME_DEFAULT);
}


/*
 * Pulse a flasher for a shorter-than usual time.
 */
extern inline void flasher_pulse_short (U8 sol)
{
	sol_start_real (sol, FLASHER_DUTY_DEFAULT, FLASHER_TIME_DEFAULT / 2);
}


/** Retrieve the default pulse duration for a coil. */
extern inline U8 sol_get_time (solnum_t sol)
{
	extern const U8 sol_time_table[];
	return sol_time_table[sol];
}

/** Retrieve the default duty strength for a coil. */
extern inline U8 sol_get_duty (solnum_t sol)
{
	extern const U8 sol_duty_table[];
	return sol_duty_table[sol];
}

/** Return the memory variable that tracks the state
of a coil driver. */
extern inline U8 *sol_get_read_reg (const solnum_t sol)
{
	extern U8 sol_reg_readable[SOL_REG_COUNT];
	return &sol_reg_readable[sol / 8];
}


/** Return the bit position in a hardware register
or memory variable that corresponds to a particular
coil driver. */
extern inline U8 sol_get_bit (const solnum_t sol)
{
	return single_bit_set (sol % 8);
}


/*
 * Turn on a solenoid driver immediately.
 * It will remain on indefinitely.
 * The hardware may not be updated instantly, however, depending
 * on the platform driver.
 */
extern inline void sol_enable (const solnum_t sol)
{
	U8 *r = sol_get_read_reg (sol);
	*r |= sol_get_bit (sol);
}


/*
 * Turn off a solenoid driver immediately.
 * The hardware may not be updated instantly, however, depending
 * on the platform driver.
 */
extern inline void sol_disable (const solnum_t sol)
{
	U8 *r = sol_get_read_reg (sol);
	*r &= ~sol_get_bit (sol);
}


__effect__ void flasher_randomize (task_ticks_t delay, U16 secs);

#endif /* _SYS_SOL_H */
