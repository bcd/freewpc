/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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
#include <queue.h>

/**
 * \file
 * \brief Handles updating the solenoids.
 *
 * The state of the solenoids is maintained in memory; the
 * actual I/O is written at IRQ time to refresh the hardware, every 4ms.
 *
 * There are two different mechanisms here, one for solenoids and one for
 * flashers.  For the flashers, there is duplicate inline code for each
 * flasher that controls it.  This allows multiple flashers to run in
 * parallel.  For the solenoids (the first 16 on WPC), there is a single
 * update function, with the solenoid number as a parameter, so only one
 * of these can be pulsed at a time.  (If you need to control an output
 * that might need to run concurrently with other things, like a long-lived
 * divertor, then these are not the right functions to use; you want to
 * use a driver in the 'drivers' directory.)
 *
 * A pulse request specifies both a time (in 4ms increments) and a duty cycle
 * (as low as 1/8, up to full 100% on).  The timer allows a maximum
 * duration of about 1 second.  The default time and duty cycle for the
 * shared solenoid driver comes from a table that is built from the
 * parameters in the [drives] section of the machine description.  When you
 * call sol_request(), these are the settings that are used.
 *
 * When using the shared driver, if another request is made while the driver
 * is already pulsing another solenoid, that request can be queued up
 * so the caller does not have to wait for it.
 */


/** Per flasher timers.  When this value is nonzero, the flasher
is enabled.  Each tick here corresponds to 4ms. */
__fastram__ U8 sol_timers[PINIO_NUM_SOLS - SOL_MIN_FLASHER];

/** Per flasher duty-cycle mask.  This is an 8-bit value where a '1'
bit means to turn it on, and a '0' means to turn it off, during the next 4ms.
When the timer is enabled, this allows the flasher to be dimmed.
Use the SOL_DUTY values here. */
U8 sol_duty_state[PINIO_NUM_SOLS - SOL_MIN_FLASHER];

/** The current bit of the duty cycle masks to be examined.  After servicing
all devices, this mask is shifted.  At most one bit is ever set here at a time. */
__fastram__ U8 sol_duty_mask;

/** The default values for the solenoid registers.  These are set by device drivers
outside of this module, providing the initial on/off states for everything. */
U8 sol_reg_readable[SOL_REG_COUNT];

/**
 * A locking mechanism used to implement synchronous pulse requests.
 * At most one synchronous request can be in process at a time; this
 * variable controls the access.
 *
 * When it is zero, the lock is available; no sync request in progress.
 * When a request is active, it is set to the solenoid number plus one
 * (so for sol 0, lock=1).
 * When a request finishes, it is set to 0x80 by the realtime driver.
 * This signals the task-level code to continue, which then releases
 * the lock by writing it back to zero.
 */
volatile U8 req_lock;

/* The read-only parameters to the one-at-a-time pulse driver,
which says which solenoid to pulse.  These are setup outside of
the realtime task to make it run faster. */
IOPTR req_reg_write;
U8 *req_reg_read;
U8 req_bit;
U8 req_inverted;

/** The timer for the shared pulse driver */
U8 sol_pulse_timer;

/** The duty cycle for the shared pulse driver */
U8 sol_pulse_duty;

/** The solenoid number for the current pulse */
U8 sol_pulsing;

#define SOL_REQ_QUEUE_LEN 8

/** A queue of solenoid pulse requests that are pending */
struct {
	queue_t header;
	U8 sols[SOL_REQ_QUEUE_LEN];
} sol_req_queue;


/**
 * Pulse a solenoid with a specific duty/time.
 */
void
sol_req_start_specific (U8 sol, U8 mask, U8 time)
{
	dbprintf ("Starting pulse %d now.\n", sol);

	/* If the timer is nonzero, another request is already running.
	This shouldn't happen.  The solenoid code takes care not to start
	a request when something is already active.  The test mode code
	calls this directly and bypasses those checks, but it enforces a delay
	between pulses so it shouldn't occur. */
	if (sol_pulse_timer != 0)
	{
		nonfatal (ERR_SOL_REQUEST);
		return;
	}

	req_reg_write = sol_get_write_reg (sol);
	if (req_reg_write == (IOPTR)0)
		return;

	req_reg_read = sol_get_read_reg (sol);
	req_bit = sol_get_bit (sol);
	sol_pulse_duty = mask;
	req_inverted = sol_inverted (sol) ? 0xFF : 0x00;

	/* This must be last, as it triggers the IRQ code */
	sol_pulse_timer = time / 4;
}



/**
 * Start a solenoid request now.
 * The state machine must be in IDLE.  This call puts it
 * into PENDING state.
 */
void sol_req_start (U8 sol)
{
	sol_pulsing = sol;

	/* Normally, just start sol_req_start_specific with default parameters.
	But provide a hook that can override them.  Any machine that wants finer
	control should declare one event handler named 'sol_pulse', which can
	inspect the solenoid number in 'sol_pulsing' and decide if special handling
	is needed.  It can call sol_req_start_specific() with different values.
	If it does, it should return FALSE and then the default call here is
	skipped.
		Reasons for providing this: 1) some devices may need more than just
	a quick pulse; it may require a sequence of pulses of different sizes.
	(It is safe to do this in the caller's context, without spawning a new
	task, if it is a ball device, since that is always done synchronously.)
	2) Device retries can kick harder on subsequent tries if the first try
	doesn't work.  You can inspect dev->kick_errors to see how many failed
	attempts have already occurred. */
	if (callset_invoke_boolean (sol_pulse))
	{
		sol_req_start_specific (sol, sol_get_duty (sol), sol_get_time (sol));
	}
}


/**
 * Periodically inspect the solenoid queue and dispatch
 * the next pending request.
 */
CALLSET_ENTRY (sol, idle_every_100ms)
{
	if (sol_pulse_timer == 0 &&
		!queue_empty_p (&sol_req_queue.header))
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
	/*
	 * If no request is active, start it now.  Otherwise, it will need to be queued.
	 */
	if (sol_pulse_timer == 0)
	{
		sol_req_start (sol);
	}
	else
	{
		dbprintf ("Queueing pulse %d\n", sol);
		/* Note that we do not check for ring buffer overflow.  This should
		never happen in practice; if it does, at worst some pulse will be
		skipped, which must already be handled elsewhere as when a pulse
		is too weak... */
		queue_insert (&sol_req_queue.header, SOL_REQ_QUEUE_LEN, sol);
	}
}


/**
 * Allocate a slot in the pulse driver for SOL.
 * This is an internal function only, and is called only when a
 * synchronous pulse request is made.  It waits if a
 * previous request is being serviced, then acquires a
 * lock so that future sync requests will be blocked.
 *
 * The caller MUST invoke sol_free() at some point later when the
 * pulse is done.  Thiis is done automatically if you use sol_request();
 * or must be done by your custom pulse shaper if you write your own.
 */
static void sol_alloc (U8 sol)
{
	/* Wait until any existing sync requests are finished. */
	while (req_lock || sol_pulse_timer)
		task_sleep (TIME_66MS);

	/* Acquire the lock for this solenoid. */
	req_lock = (sol + 1) | 0x80;

	/* Remember which solenoid we are pulsing now */
	sol_pulsing = sol;
}


/**
 * Change the duty cycle of a pulse in progress.
 */
void sol_modify_duty (U8 duty)
{
	sol_pulse_duty = duty;
}


/**
 * Change the timeout of a pulse in progress.
 */
void sol_modify_timeout (U8 timeout)
{
	sol_pulse_timer = timeout / 4;
}


/**
 * Free the lock on a particular solenoid.  This waits for the
 * pulse to finish, then releases the driver for others.
 */
void sol_free (U8 sol)
{
	/* Wait for the pulse to finish */
	while (req_lock != 0x80)
		task_sleep (TIME_66MS);

	/* Release the lock for another task */
	req_lock = 0;
}


/**
 * Make a solenoid request, and wait for it to finish before returning.
 *
 * This is one of the 2 topmost APIs that starts a solenoid pulse
 * (see also sol_request_async).
 *
 * By default, sol_request behaves identically to sol_request_async, with
 * the addition of the alloc and free calls around it.  However, machines
 * can override this behavior by catching the 'sol_request' event, and
 * doing something different.  The handler should inspect the value of
 * sol_pulsing to see which solenoid is being pulsed, it should call
 * sol_free() just before returning, and finally it should return FALSE.
 */
void sol_request (U8 sol)
{
	sol_alloc (sol);
	if (callset_invoke_boolean (sol_request))
	{
		sol_req_start (sol);
		sol_free (sol);
	}
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
 * The realtime pulsed solenoid update.
 *
 * It works identically to the code for the flashers, except there can only be
 * one at a time.
 */
void sol_req_rtt (void)
{
	if (sol_pulse_timer != 0)
	{
		sol_pulse_timer--;
		if (sol_pulse_timer && (sol_pulse_duty & sol_duty_mask))
			sol_req_on ();
		else
		{
			sol_req_off ();
			if (sol_pulse_timer == 0)
			{
				sol_pulse_duty = 0;
				if (req_lock)
					req_lock = 0x80;
			}
		}
	}
}


/** Return 0 if the given solenoid/flasher should be off,
else return the bitmask that reflects that solenoid's
position in the output register. */
extern inline U8 sol_update1 (const U8 id)
{
	if (MACHINE_SOL_FLASHERP (id))
		if (likely (sol_timers[id - SOL_MIN_FLASHER] != 0))
		{
			sol_timers[id - SOL_MIN_FLASHER]--;

			if (likely (sol_duty_state[id - SOL_MIN_FLASHER] & sol_duty_mask))
				return 1;
		}
	return 0;
}


/** Update the value 'bits' to reflect whether a specific solenoid
should be turned on at this instant.  'id' is the solenoid number.
bits stores the output state for an entire bank of 8 solenoids at
a time. */
#define sol_contribute(id,bits) \
	if (sol_update1 (id)) { bits |= (1 << ((id) & (CHAR_BIT - 1))); }


/** Update a set of 8 solenoids that share the same output register.
 * base_id is the solenoid number for the first solenoid in the set.
 * asic_addr is the hardware register to be written with all 8 values
 * at once. */
extern inline void sol_update_set (const U8 set)
{
	register U8 out __areg__ = *sol_get_read_reg (set * 8);

	/* Update each of the 8 solenoids in the bank, updating timers
	and calculating whether or not each should be on or off. */
	sol_contribute (set * CHAR_BIT + 0, out);
	sol_contribute (set * CHAR_BIT + 1, out);
	sol_contribute (set * CHAR_BIT + 2, out);
	sol_contribute (set * CHAR_BIT + 3, out);
	sol_contribute (set * CHAR_BIT + 4, out);
	sol_contribute (set * CHAR_BIT + 5, out);
	sol_contribute (set * CHAR_BIT + 6, out);
	sol_contribute (set * CHAR_BIT + 7, out);

	/* Write the final output to the hardware */
	pinio_write_solenoid_set (set, out);
}


/** Like sol_update_set, but updates the Fliptronic outputs.
 * The base_id and asic_addr are implied here. */
extern inline void sol_update_fliptronic_powered (void)
{
	extern U8 fliptronic_powered_coil_outputs;
	register U8 out __areg__ = fliptronic_powered_coil_outputs;

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


/** Realtime update of the first set of flasher outputs */
void sol_update_rtt_0 (void)
{
	pinio_write_solenoid_set (0, *sol_get_read_reg (0));
	sol_update_set (2);
#ifdef CONFIG_PLATFORM_WPC
	if (WPC_HAS_CAP (WPC_CAP_FLIPTRONIC))
		sol_update_fliptronic_powered ();
#endif
}


/** Realtime update of the second set of flasher outputs */
void sol_update_rtt_1 (void)
{
	pinio_write_solenoid_set (1, *sol_get_read_reg (8));
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
    User code should not invoke this directly; use one of the
macros in sys/sol.h instead. */
__attribute__((noinline)) void
sol_start_real (solnum_t sol, U8 duty_mask, U8 ticks)
{
	/* The duty cycle mask is only read by the IRQ
	 * function, so it can be modified easily.
	 * The timer value is read-and-decremented, so it
	 * needs to set atomically. */
	log_event (SEV_INFO, MOD_SOL, EV_SOL_START, sol);
	sol_duty_state[sol - SOL_MIN_FLASHER] = duty_mask;
	disable_interrupts ();
	sol_timers[sol - SOL_MIN_FLASHER] = ticks;
	enable_interrupts ();
}


/** Stop a running solenoid. */
void
sol_stop (solnum_t sol)
{
	log_event (SEV_INFO, MOD_SOL, EV_SOL_STOP, sol);
	disable_interrupts ();
	sol_timers[sol - SOL_MIN_FLASHER] = 0;
	sol_duty_state[sol - SOL_MIN_FLASHER] = 0;
	enable_interrupts ();
}


/** Initialize the solenoid subsystem. */
void
sol_init (void)
{
	U8 sol;

	/* Clear all of the solenoid timers. */
	memset (sol_timers, 0, sizeof (sol_timers));

	/* Initialize the duty state of all solenoids to their nominal
	 * values. */
	for (sol = SOL_MIN_FLASHER; sol < PINIO_NUM_SOLS; sol++)
	{
		sol_duty_state[sol - SOL_MIN_FLASHER] = sol_get_duty (sol);
	}

	/* Initialize the rotating duty strobe mask */
	sol_duty_mask = 0x1;

	/* Initialize the one-at-a-time pulse driver */
	sol_pulse_timer = sol_pulse_duty = 0;
	req_lock = 0;

	memset (sol_reg_readable, 0, SOL_REG_COUNT);

	/* Initialize the solenoid queue. */
	queue_init (&sol_req_queue.header);
}

