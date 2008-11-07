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

#define SOL_REG_COUNT (SOL_COUNT / 8)

/* TODO - these are WPC specific */
#define SOL_BASE_HIGH 0
#define SOL_BASE_LOW 8
#define SOL_BASE_GENERAL 16
#define SOL_BASE_AUXILIARY 24
#define SOL_BASE_FLIPTRONIC 32
#define SOL_BASE_EXTENDED 40

extern __fastram__ U8 sol_timers[];
extern U8 sol_duty_state[];


/** Duty cycle values.  Each '1' bit represents a
time quantum during which the coil on.  The more '1's,
the more powerful the pulse. */
#define SOL_DUTY_0      0x0
#define SOL_DUTY_12     0x40
#define SOL_DUTY_25     0x22
#define SOL_DUTY_37     0x92
#define SOL_DUTY_50     0x55
#define SOL_DUTY_62     0xB5
#define SOL_DUTY_75     0x77
#define SOL_DUTY_100    0xFF

/** The default duty cycle is kept small.  This
can be overwritten in the machine config file. */
#define SOL_DUTY_DEFAULT   SOL_DUTY_25

/** The default pulse time is reasonable for most
coils. */
#define SOL_TIME_DEFAULT   TIME_133MS

#define FLASHER_DUTY_DEFAULT SOL_DUTY_50
#define FLASHER_TIME_DEFAULT TIME_66MS

/* Function prototypes */
void sol_request_async (U8 sol);
void sol_request (U8 sol);
__attribute__((deprecated)) void sol_start_real (solnum_t sol, U8 cycle_mask, U8 ticks);
void sol_stop (solnum_t sol);
void sol_init (void);

/* sol_start is a macro because the 'time' value must be scaled
to the correct resolution.  Ticks are normally 1 per 16ms, but
we need 1 per 4ms for solenoids, so scale accordingly. */
#define sol_start(sol,mask,time) \
	sol_start_real (sol, mask, (4 * time))

#define flasher_pulse(id) sol_pulse(id)

#define sol_pulse sol_request_async

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


/** Return the hardware register that can be written
to enable/disable a coil driver. */
extern inline U8 *sol_get_write_reg (solnum_t sol)
{
	switch (sol / 8)
	{
		case 0:
			return (U8 *)WPC_SOL_HIGHPOWER_OUTPUT;
		case 1:
			return (U8 *)WPC_SOL_LOWPOWER_OUTPUT;
		case 2:
			return (U8 *)WPC_SOL_FLASH1_OUTPUT;
		case 3:
			return (U8 *)WPC_SOL_FLASH2_OUTPUT;
		case 4:
#if (MACHINE_WPC95 == 1)
			return (U8 *)WPC95_FLIPPER_COIL_OUTPUT;
#else
			return (U8 *)WPC_FLIPTRONIC_PORT_A;
#endif
		case 5:
			return (U8 *)WPC_EXTBOARD1;
		default:
			fatal (ERR_SOL_REQUEST);
			return NULL;
	}
}


/** Return the bit position in a hardware register
or memory variable that corresponds to a particular
coil driver. */
extern inline U8 sol_get_bit (const solnum_t sol)
{
	return 1 << (sol % 8);
}


/** Return nonzero if a solenoid's enable line is
 * inverted; i.e. writing a 0 turns it on and
 * writing a 1 turns it off.
 */
extern inline U8 sol_inverted (const solnum_t sol)
{
	return (sol >= 32) && (sol < 40);
}


/** Turn on a solenoid immediately. */
extern inline void sol_enable (const solnum_t sol)
{
	U8 *r = sol_get_read_reg (sol);
	U8 *w = sol_get_write_reg (sol);
	*w = (*r |= sol_get_bit (sol)) ^ (sol_inverted (sol) ? 0xFF : 0x00);
}


/** Turn off a solenoid immediately. */
extern inline void sol_disable (const solnum_t sol)
{
	U8 *r = sol_get_read_reg (sol);
	U8 *w = sol_get_write_reg (sol);
	*w = (*r &= ~sol_get_bit (sol)) ^ (sol_inverted (sol) ? 0xFF : 0x00);
}

#endif /* _SYS_SOL_H */
