
#include <freewpc.h>
#include <system/platform.h>

/** Runs periodically to update the physical lamp state. */
void lamp_rtt (void)
{
	U8 bits;

	/* Turn off the lamp circuits before recalculating.  But don't
	do this in native mode, because the simulator doesn't simulate
	well-enough. */
#ifdef CONFIG_NATIVE
	pinio_write_lamp_strobe (0);
#else
#ifdef __m6809__
	/* On the 6809, avoid using the CLR instruction which is known to cause
	problems in the WPC ASIC.   Also, always write ROW first to avoid
	spurious lamps. */
	__asm__ volatile ("clrb");
	__asm__ volatile ("stb\t" C_STRING (WPC_LAMP_ROW_OUTPUT));
	__asm__ volatile ("stb\t" C_STRING (WPC_LAMP_COL_STROBE));
#else
	pinio_write_lamp_data (0);
	pinio_write_lamp_strobe (0);
#endif /* __m6809__ */
#endif /* CONFIG_NATIVE */

	/* Implement lamp power saver.  When the timer is nonzero, it means
	to keep the lamp circuits off for this many IRQ iterations. */
	if (unlikely (lamp_power_timer))
	{
		--lamp_power_timer;
		return;
	}

	/* Grab the default lamp values */
	bits = lamp_matrix[lamp_strobe_column];

	/* OR in the flashing lamp values.  These are guaranteed to be
	 * zero for any lamps where the flash is turned off.
	 * Otherwise, these bits are periodically inverted by the
	 * (slower) flash rtt function above.
	 * This means that for the flash to work, the default bit
	 * must be OFF when the flash bit is ON.  (Use the tristate
	 * macros to ensure this.)
	 */
	bits |= lamp_flash_matrix_now[lamp_strobe_column];

	/* TODO : implement lamp strobing, like the newer Stern games
	do.  Implement like DMD page flipping, alternating between 2
	different lamp matrices rapidly to present 4 different
	intensities.  A background task, like the flash_rtt above,
	would toggle the intensities at a slower rate. */

	/* Override with the lamp effect lamps.
	 * Leff2 bits are low priority and used for long-running
	 * lamp effects.  Leff1 is higher priority and used
	 * for quick effects.  Therefore leff2 is applied first,
	 * and leff1 may override it.
	 */
	bits &= lamp_leff2_allocated[lamp_strobe_column];
	bits |= lamp_leff2_matrix[lamp_strobe_column];
	bits &= lamp_leff1_allocated[lamp_strobe_column];
	bits |= lamp_leff1_matrix[lamp_strobe_column];

	/* Write the result to the hardware */
	pinio_write_lamp_data (bits);
	pinio_write_lamp_strobe (lamp_strobe_mask);

	/* Advance the strobe value for the next iteration.
	Keep this together with the above so that lamp_strobe_mask
	is already in a register. */
	lamp_strobe_mask <<= 1;
	if (lamp_strobe_mask == 0)
	{
		/* All columns strobed : reset strobe */
		lamp_strobe_mask++;
		lamp_strobe_column = 0;

		/* After strobing all lamps, reload the power saver timer */
		lamp_power_timer = lamp_power_level;
	}
	else
	{
		/* Advance strobe to next position for next iteration */
		lamp_strobe_column++;
	}
}


