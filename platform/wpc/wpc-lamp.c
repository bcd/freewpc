
#include <freewpc.h>
#include <system/platform.h>

/** Runs periodically to update the physical lamp state. */
/* RTT(name=lamp_rtt freq=2) */
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

	/* Compute the lamps to be turned on. */
	bits = platform_lamp_compute (lamp_strobe_column);

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


