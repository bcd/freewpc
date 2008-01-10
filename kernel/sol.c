/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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
#include <rtsol.h>


/**
 * \file
 * \brief Handles updating the solenoids.
 * The state of the solenoids is maintained in memory; the
 * actual I/O is written at IRQ time to refresh the hardware.
 *
 * Each solenoid set (of 8) is serviced during each successive 1ms;
 * thus the effective service time per solenoid is once every 4ms.
 * Each has a timer that allows it to remain active for up to 255
 * iterations, or a total duration of 1.02s.  If a coil needs to
 * energize longer than that, it is up to the task level to
 * continue to kick it.  Each also has an 8-stage duty cycle,
 * allowing anywhere from full power to 1/8th power (one pulse
 * per 33ms).
 */


/** Per solenoid on-time.  When this value is nonzero, the solenoid
is enabled.  Each tick here corresponds to 4ms. */
U8 sol_timers[SOL_COUNT];

/** Per solenoid duty-cycle mask.  This is an 8-bit value where a '1'
bit means to keep the solenoid on, and a '0' means to (temporarily)
turn it off.  When the solenoid is enabled, this allows it to be
duty cycled.  A value of 0xFF means 100% power, 0x55 means 50% power, etc.
This value is ignored when the solenoid is disabled. */
U8 sol_duty_state[SOL_COUNT];

/** The current bit of the duty cycle masks to be examined.  After each
iteration through all solenoids, this mask is shifted.  At most one bit
is set here at a time. */
U8 sol_duty_mask;


/** Return 0 if the given solenoid/flasher should be off,
else return the bitmask that reflects that solenoid's
position in the output register. */
extern inline U8 sol_update1 (const U8 id)
{
	if (likely (sol_timers[id] != 0))
	{
		sol_timers[id]--;
		if (likely (sol_duty_state[id] & sol_duty_mask))
			return 1;
	}
	return 0;
}


/** Update the value 'bits' to reflect whether a specific solenoid
should be turned on at this instant.  'id' is the solenoid number.
bits stores the output state for an entire bank of 8 solenoids at
a time. */
#define sol_contribute(id,bits) \
	if (sol_update1 (id)) { bits |= (1 << ((id) & 0x7)); }


/** Update a set of 8 solenoids that share the same output register. */
extern inline void sol_update_set (const U8 base_id, const U16 asic_addr)
{
	/* For some reason, GCC 4.x crashes on this function... */
#ifdef GCC4
	register U8 out __areg__ = 0;
#else
	register U8 out = 0;
#endif

	/* Update each of the 8 solenoids in the bank, updating timers
	and calculating whether or not each should be on or off. */
	sol_contribute (base_id + 0, out);
	sol_contribute (base_id + 1, out);
	sol_contribute (base_id + 2, out);
	sol_contribute (base_id + 3, out);
	sol_contribute (base_id + 4, out);
	sol_contribute (base_id + 5, out);
	sol_contribute (base_id + 6, out);
	sol_contribute (base_id + 7, out);

	/* Write the final output to the hardware */
	wpc_asic_write (asic_addr, out);
}


/** Realtime update of the high power solenoids */
void sol_update_rtt_0 (void)
{
	sol_update_set (0, WPC_SOL_HIGHPOWER_OUTPUT);
}


/** Realtime update of the low power solenoids */
void sol_update_rtt_1 (void)
{
	sol_update_set (8, WPC_SOL_LOWPOWER_OUTPUT);
}


/** Realtime update of the first set of flasher outputs */
void sol_update_rtt_2 (void)
{
	sol_update_set (16, WPC_SOL_FLASH1_OUTPUT);
}


/** Realtime update of the second set of flasher outputs */
void sol_update_rtt_3 (void)
{
	sol_update_set (24, WPC_SOL_FLASH2_OUTPUT);
#ifdef MACHINE_SOL_EXTBOARD1
	sol_update_set (40, WPC_EXTBOARD1);
#endif

	/* Rotate the duty mask for the next iteration. */
	sol_duty_mask <<= 1;
	if (sol_duty_mask == 0)
		sol_duty_mask = 1;
}


/** Starts a solenoid.  The duty_mask controls how much power is
applied to the coil; the timeout says how long it should be
applied.  Infinite timeout is *not* supported here, but can
be emulated by repeated calls to this function ; this is done
on purpose to prevent exceeding timeouts due to the inherent
scheduling flaws at task level.
   For the timeout level, use the SOL_TIMEOUT() macro with
one of the TASK_xxx defines. */
void
sol_start_real (solnum_t sol, U8 duty_mask, U8 ticks)
{
	sol_duty_state[sol] = duty_mask;
	disable_interrupts ();
	sol_timers[sol] = ticks;
	enable_interrupts ();
}


/** Stop a running solenoid. */
void
sol_stop (solnum_t sol)
{
	disable_interrupts ();
	sol_timers[sol] = 0;
	enable_interrupts ();
}


/** Initialize the solenoid subsystem. */
void
sol_init (void)
{
	/* Clear all of the solenoid timers. */
	memset (sol_timers, 0, sizeof (sol_timers));

	/* Set all of the duty cycle masks to 0xFF, or 100%.  Interrupt-level
	 * code that wants to enable an output quickly can do so by just
	 * writing to the timer. */
	memset (sol_duty_state, 0xFF, sizeof (sol_duty_state));
	sol_duty_mask = 0x1;
}


