
#include <freewpc.h>
#include <system/platform.h>

/** Return 0 if the given solenoid/flasher should be off,
else return the bitmask that reflects that solenoid's
position in the output register. */
extern inline U8 platform_sol_timer_check (const U8 id)
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
#define platform_sol_timer_contribute(id,bits) \
	if (platform_sol_timer_check (id)) { bits |= (1 << ((id) & (CHAR_BIT - 1))); }


/** Update a set of 8 solenoids that share the same output register.
 * base_id is the solenoid number for the first solenoid in the set.
 * asic_addr is the hardware register to be written with all 8 values
 * at once. */
extern inline void platform_sol_update_timed (const U8 set)
{
	register U8 out __areg__ = *sol_get_read_reg (set * CHAR_BIT);

	/* Update each of the 8 solenoids in the bank, updating timers
	and calculating whether or not each should be on or off. */
	platform_sol_timer_contribute (set * CHAR_BIT + 0, out);
	platform_sol_timer_contribute (set * CHAR_BIT + 1, out);
	platform_sol_timer_contribute (set * CHAR_BIT + 2, out);
	platform_sol_timer_contribute (set * CHAR_BIT + 3, out);
	platform_sol_timer_contribute (set * CHAR_BIT + 4, out);
	platform_sol_timer_contribute (set * CHAR_BIT + 5, out);
	platform_sol_timer_contribute (set * CHAR_BIT + 6, out);
	platform_sol_timer_contribute (set * CHAR_BIT + 7, out);

	/* Write the final output to the hardware */
	pinio_write_solenoid_set (set, out);
}


/** Like platform_sol_update_timed, but updates the Fliptronic outputs.
 * The base_id and asic_addr are implied here. */
extern inline void sol_update_fliptronic_powered (void)
{
	extern U8 fliptronic_powered_coil_outputs;
	register U8 out __areg__ = fliptronic_powered_coil_outputs;

	/* Update each of the 8 solenoids in the bank, updating timers
	and calculating whether or not each should be on or off. */
	platform_sol_timer_contribute (32, out);
	platform_sol_timer_contribute (33, out);
	platform_sol_timer_contribute (34, out);
	platform_sol_timer_contribute (35, out);
	platform_sol_timer_contribute (36, out);
	platform_sol_timer_contribute (37, out);
	platform_sol_timer_contribute (38, out);
	platform_sol_timer_contribute (39, out);

	/* Write the final output to the hardware */
	fliptronic_powered_coil_outputs = out;
}

extern inline void platform_sol_update_direct (const U8 sol_set)
{
	pinio_write_solenoid_set (sol_set, *sol_get_read_reg (sol_set * CHAR_BIT));
}


/** Realtime update of the first set of flasher outputs */
/* RTT(name=sol_update_rtt_0 freq=2) */
void sol_update_rtt_0 (void)
{
	platform_sol_update_direct (0);
	platform_sol_update_timed (2);
#ifdef CONFIG_PLATFORM_WPC
	if (WPC_HAS_CAP (WPC_CAP_FLIPTRONIC))
		sol_update_fliptronic_powered ();
#endif
}


/** Realtime update of the second set of flasher outputs */
/* RTT(name=sol_update_rtt_1 freq=2) */
void sol_update_rtt_1 (void)
{
	platform_sol_update_direct (1);
	platform_sol_update_timed (3);
#ifdef MACHINE_SOL_EXTBOARD1
	platform_sol_update_timed (5);
#endif

	/* Rotate the duty mask for the next iteration. */
	/* TODO - the assembly code generated here is not ideal.
	It could be done in two instructions, by shifting and then
	adding the carry.  Need a way from gcc to request this. */
	sol_duty_mask <<= 1;
	if (sol_duty_mask == 0)
		sol_duty_mask = 1;
}



