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

#include <freewpc.h>
#include <rtsol.h>
#include <queue.h>

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

U8 sol_reg_readable[SOL_REG_COUNT];


/**
 * The state of the solenoid request driver.
 */
enum sol_request_state {
	/* No solenoid request is pending */
	REQ_IDLE,

	/* A request is pending but has not been started.
	 * For maximum power, the start should be deferred
	 * until just after the next zerocross. */
	REQ_PENDING,

	/* The solenoid is running at 100% power.
	 * When the timer expires, it switches to the duty
	 * phase. */
	REQ_ON,

	/* The solenoid is running at less than 100%.
	 * The duty mask controls during which phases of the AC
	 * cycle it is on, and when it is off.  When the timer
	 * expires, the coil is turned off and we go back to
	 * idle state. */
	REQ_DUTY,
};

__fastram__ volatile enum sol_request_state sol_req_state;
__fastram__ volatile U8 sol_req_timer;

volatile U8 req_lock;
IOPTR req_reg_write;
U8 *req_reg_read;
U8 req_bit;
U8 req_on_time;
U8 req_duty_time;
U8 req_duty_mask;
U8 req_inverted;

#define SOL_REQ_QUEUE_LEN 8

struct {
	queue_t header;
	U8 sols[SOL_REQ_QUEUE_LEN];
} sol_req_queue;


/**
 * Dump the state of the solenoid request driver.
 */
#ifdef DEBUGGER
void sol_req_dump (void)
{
	dbprintf ("State = %d\n", sol_req_state);
	dbprintf ("Timer = %d\n", sol_req_timer);
	dbprintf ("Write = %p, Read = %p\n", req_reg_write, req_reg_read);
	dbprintf ("Bit = %02X\n", req_bit);
}
#endif


/**
 * Start a solenoid request now.
 * The state machine must be in IDLE.  This call puts it
 * into PENDING state.
 */
void sol_req_start (U8 sol)
{
	dbprintf ("Starting pulse %d now.\n", sol);

	if (sol_req_state != REQ_IDLE)
		fatal (ERR_SOL_REQUEST);

	req_reg_write = sol_get_write_reg (sol);
	if (req_reg_write == (IOPTR)0)
		return;

	req_reg_read = sol_get_read_reg (sol);
	req_bit = sol_get_bit (sol);
	req_on_time = 16;
	req_duty_time = (sol_get_time (sol) - 1) * 16;
	req_duty_mask = 0x3; /* 25% */
	req_inverted = sol_inverted (sol) ? 0xFF : 0x00;

	/* Mark the request pending, so the update procedure will see it. */
	sol_req_state = REQ_PENDING;
}




/**
 * Periodically inspect the solenoid queue and dispatch
 * the pending request.
 */
CALLSET_ENTRY (sol, idle_every_100ms)
{
	if (sol_req_state == REQ_IDLE
		&& !queue_empty_p (&sol_req_queue.header))
	{
		U8 sol = queue_remove (&sol_req_queue.header, SOL_REQ_QUEUE_LEN);
		sol_req_start (sol);
	}
}


/**
 * Make a solenoid request, and return immediately, even if it
 * is not started.
 */
void sol_request_async (U8 sol)
{
	dbprintf ("Request pulse %d\n", sol);

	/*
	 * If the state machine is IDLE, go ahead and start the request.
	 * Otherwise, it will need to be queued.
	 */
	if (sol_req_state == REQ_IDLE)
	{
		sol_req_start (sol);
	}
	else
	{
		dbprintf ("Queueing pulse %d\n", sol);
		queue_insert (&sol_req_queue.header, SOL_REQ_QUEUE_LEN, sol);
		/* TODO - what if the queue is full? */
	}
}


/**
 * Make a solenoid request, and wait for it to finish before returning.
 */
void sol_request (U8 sol)
{
	/* TODO : If coils are disabled, then just return. */

	/* Wait until any existing sync requests are finished. */
	while (req_lock)
		task_sleep (TIME_33MS);

	/* Acquire the lock for this solenoid. */
	req_lock = sol; /* TODO - what if sol == 0 ? */

	/* Issue the request */
	sol_request_async (sol);

	/* Wait for the request to finish */
	while (req_lock)
		task_sleep (TIME_33MS);
}


extern inline void sol_req_on (void)
{
	writeb (req_reg_write, (*req_reg_read |= req_bit) ^ req_inverted);
}


extern inline void sol_req_off (void)
{
	writeb (req_reg_write, (*req_reg_read &= ~req_bit) ^ req_inverted);
}


/**
 * Handle solenoid requests in realtime, every 1ms.
 *
 * The lifecycle of the request manager is as follows:
 *
 * IDLE   --->  PENDING   --->  ON   ---> DUTY
 *
 * See the declaration of these states for details.
 */
void sol_req_rtt (void)
{
	/* If nothing to do, get out quickly. */
	if (likely (sol_req_state == REQ_IDLE))
		return;

	else if (sol_req_state == REQ_PENDING)
	{
		/* In the pending state, start the solenoid
		 * at 100% power but only if just past a zerocrossing
		 * point.  If the ZC circuit is broken, this
		 * state is bypassed and task level will always
		 * program the request in ON mode. */
		if (zc_get_timer () == 2)
		{
			sol_req_timer = req_on_time;
			sol_req_on ();
			sol_req_state = REQ_ON;
		}
	}

	else
	{
		/* In either the ON or DUTY states, we must decrement the timer. */
		/* TODO - in either of these states, precise timing is not
		as critical so we may be able to do this less frequently; i.e.
		every 8ms instead of every 1ms. */
		--sol_req_timer;
		if (sol_req_timer == 0)
		{
			/* On expiry, switch to the next state. */
			if (sol_req_state == REQ_ON)
			{
				/* Switch to DUTY. */
				sol_req_timer = req_duty_time;
				sol_req_state = REQ_DUTY;
			}
			else
			{
				/* Switch to IDLE.  Ensure the coil is off.
				Release any process waiting on this solenoid to
				finish. */
				sol_req_off ();
				sol_req_state = REQ_IDLE;
				req_lock = 0;
			}
		}
		else if (sol_req_state == REQ_DUTY)
		{
			if ((sol_req_timer & sol_duty_mask) == 0)
			{
				/* Occasionally turn on the coil */
				sol_req_on ();
			}
			else
			{
				/* Most of the time, keep it off */
				sol_req_off ();
			}
		}
	}
}


/** Return 0 if the given solenoid/flasher should be off,
else return the bitmask that reflects that solenoid's
position in the output register. */
extern inline U8 sol_update1 (const U8 id)
{
#ifdef CONFIG_OLD_SOL
	if (MACHINE_SOLENOID_P (id) || MACHINE_SOL_FLASHERP (id))
#else
	if (MACHINE_SOL_FLASHERP (id))
#endif
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
extern inline void sol_update_set (const U8 set)
{
	/* For some reason, GCC 4.x crashes on this function... */
#ifdef GCC4
	register U8 out __areg__ = *sol_get_read_reg (set * 8);
#else
	register U8 out = *sol_get_read_reg (set * 8);
#endif

	/* Update each of the 8 solenoids in the bank, updating timers
	and calculating whether or not each should be on or off. */
	sol_contribute (set * 8 + 0, out);
	sol_contribute (set * 8 + 1, out);
	sol_contribute (set * 8 + 2, out);
	sol_contribute (set * 8 + 3, out);
	sol_contribute (set * 8 + 4, out);
	sol_contribute (set * 8 + 5, out);
	sol_contribute (set * 8 + 6, out);
	sol_contribute (set * 8 + 7, out);

	/* Write the final output to the hardware */
	pinio_write_solenoid_set (set, out);
}


/** Like sol_update_set, but updates the Fliptronic outputs.
 * The base_id and asic_addr are implied here. */
extern inline void sol_update_fliptronic_powered (void)
{
	extern U8 fliptronic_powered_coil_outputs;

	/* For some reason, GCC 4.x crashes on this function... */
#ifdef GCC4
	register U8 out __areg__ = fliptronic_powered_coil_outputs;
#else
	register U8 out = fliptronic_powered_coil_outputs;
#endif

	/* Update each of the 8 solenoids in the bank, updating timers
	and calculating whether or not each should be on or off. */
	sol_contribute (32, out);
	sol_contribute (33, out);
	sol_contribute (34, out);
	sol_contribute (35, out);
	sol_contribute (36, out);
	sol_contribute (37, out);
	sol_contribute (38, out);
	sol_contribute (39, out);

	/* Write the final output to the hardware */
	fliptronic_powered_coil_outputs = out;
}


/**
 * The 2 RTT functions below are only used for
 * flashers, since pulsed coils are handled through
 * the request module.
 *
 * Several improvements can be made here:
 * 1) Merge these back to a single function.
 * 2) Change the frequency from 4ms to something larger.
 * Flashers don't need that much granularity, unless
 * individually controlled dimming is required.  I'm not
 * worried about that now.
 * 3) It's unlikely that *all* flashers would be pulsed
 * at once -- could we maybe get by with fewer slots
 * than actual flashers?
 */


/** Realtime update of the first set of flasher outputs */
/* TODO - as these functions are only used for flashers now,
 * they do not need to run at the same frequency (4ms).
 * We could get by with every 16ms probably, except maybe
 * for dimming effects. */
void sol_update_rtt_0 (void)
{
	sol_update_set (2);
	if (WPC_HAS_CAP (WPC_CAP_FLIPTRONIC))
		sol_update_fliptronic_powered ();
}


/** Realtime update of the second set of flasher outputs */
void sol_update_rtt_1 (void)
{
	sol_update_set (3);
#ifdef MACHINE_SOL_EXTBOARD1
	sol_update_set (5);
#endif

	/* Rotate the duty mask for the next iteration. */
	/* TODO - the assembly code generated here is not ideal.
	It could be done in two instructions, by shifting and then
	adding the carry.  Need a way from gcc to request this. */
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
__attribute__((noinline)) void
sol_start_real (solnum_t sol, U8 duty_mask, U8 ticks)
{
	/* The duty cycle mask is only read by the IRQ
	 * function, so it can be modified easily.
	 * The timer value is read-and-decremented, so it
	 * needs to set atomically. */
	log_event (SEV_INFO, MOD_SOL, EV_SOL_START, sol);
	sol_duty_state[sol] = duty_mask;
	disable_interrupts ();
	sol_timers[sol] = ticks;
	enable_interrupts ();
}


/** Stop a running solenoid. */
void
sol_stop (solnum_t sol)
{
	log_event (SEV_INFO, MOD_SOL, EV_SOL_STOP, sol);
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

	sol_req_state = REQ_IDLE;
	memset (sol_reg_readable, 0, SOL_REG_COUNT);

	/* Initialize the solenoid queue. */
	queue_init (&sol_req_queue.header);
}

