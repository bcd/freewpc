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

#include <freewpc.h>
#include <rtsol.h>


/**
 * \file
 * \brief Handles updating the solenoids.
 * The state of the solenoids is maintained in memory; the
 * actual I/O is written at IRQ time to refresh the hardware.
 *
 * Each solenoid set (of 8) is serviced during each successive 1ms.
 * Real-time update cycles through 4 sets of outputs, so
 * the effective service time per solenoid is once every 4ms.
 * (For some machines there are more than 4 sets to be serviced;
 * in that case it is necessary to service more than 1 set sometimes.)
 *
 * Each solenoid a timer that allows it to remain active for up to 255
 * iterations, or a total duration of 1.02s.  If a coil needs to
 * energize longer than that, it is up to the task level to
 * continue to kick it.  Each also has an 8-stage duty cycle,
 * allowing anywhere from full power to 1/8th power (one pulse
 * per 33ms).
 *
 * The algorithm has been designed to minimize hardware damage due
 * to software failures.  A random write to any of the solenoid timers
 * would cause that solenoid to turn on, but at most for 1.02s, since
 * the timer countdown automatically restores it to zero.
 * TODO : it is best to zero the mask registers as well, so that
 * two random writes would be required to mistakenly turn on a solenoid.
 */


/** Per solenoid on-time.  When this value is nonzero, the solenoid
is enabled.  Each tick here corresponds to 4ms. */
__fastram__ U8 sol_timers[SOL_COUNT];

/** Per solenoid duty-cycle mask.  This is an 8-bit value where a '1'
bit means to turn the solenoid on, and a '0' means to (temporarily)
turn it off.  When the solenoid is enabled, this allows it to be
duty cycled.  A value of 0xFF means 100% power, 0x55 means 50% power, etc.
Each phase of the duty cycle is 4ms long. */
U8 sol_duty_state[SOL_COUNT];

/** The current bit of the duty cycle masks to be examined.  After each
iteration through all solenoids, this mask is shifted.  At most one bit
is set here at a time. */
__fastram__ U8 sol_duty_mask;


/** Return 0 if the given solenoid/flasher should be off,
else return the bitmask that reflects that solenoid's
position in the output register. */
extern inline U8 sol_update1 (const U8 id)
{
	if (MACHINE_SOLENOID_P (id) || MACHINE_SOL_FLASHERP (id))
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


/** Update a set of 8 solenoids that share the same output register.
 * base_id is the solenoid number for the first solenoid in the set.
 * asic_addr is the hardware register to be written with all 8 values
 * at once. */
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


/** Like sol_update_set, but updates the Fliptronic outputs.
 * The base_id and asic_addr are implied here. */
extern inline void sol_update_fliptronic_powered (void)
{
	extern U8 fliptronic_powered_coil_outputs;

	/* For some reason, GCC 4.x crashes on this function... */
#ifdef GCC4
	register U8 out __areg__ = 0;
#else
	register U8 out = 0;
#endif

	/* Update each of the 8 solenoids in the bank, updating timers
	and calculating whether or not each should be on or off. */
	sol_contribute (36, out);
	sol_contribute (37, out);
	sol_contribute (38, out);
	sol_contribute (39, out);

	/* Write the final output to the hardware */
	fliptronic_powered_coil_outputs = out;
}


/** Realtime update of the high power solenoids */
void sol_update_rtt_0 (void)
{
	sol_update_set (SOL_BASE_HIGH, WPC_SOL_HIGHPOWER_OUTPUT);
}


/** Realtime update of the low power solenoids */
void sol_update_rtt_1 (void)
{
	sol_update_set (SOL_BASE_LOW, WPC_SOL_LOWPOWER_OUTPUT);
#if (MACHINE_WPC95 == 1)
	sol_update_fliptronic_powered ();
#endif
}


/** Realtime update of the first set of flasher outputs */
void sol_update_rtt_2 (void)
{
	sol_update_set (SOL_BASE_GENERAL, WPC_SOL_FLASH1_OUTPUT);
}


/** Realtime update of the second set of flasher outputs */
void sol_update_rtt_3 (void)
{
	sol_update_set (SOL_BASE_AUXILIARY, WPC_SOL_FLASH2_OUTPUT);
#ifdef MACHINE_SOL_EXTBOARD1
	sol_update_set (SOL_BASE_EXTENDED, WPC_EXTBOARD1);
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
one of the TASK_xxx defines.  This ensures that it is converted
to the proper units based on the scheduling of the solenoid
update at IRQ time. */
void
sol_start_real (solnum_t sol, U8 duty_mask, U8 ticks)
{
	/* The duty cycle mask is only read by the IRQ
	 * function, so it can be modified easily.
	 * The timer value is read-and-decremented, so it
	 * needs to set atomically. */
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
	sol_duty_state[sol] = 0;
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
	 * writing to the timer.  TODO : this is not particularly safe */
	memset (sol_duty_state, 0xFF, sizeof (sol_duty_state));
	sol_duty_mask = 0x1;
}


