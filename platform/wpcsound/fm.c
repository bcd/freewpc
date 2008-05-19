
#include <freewpc.h>


/**
 * Writes to a FM-chip register.
 */
__attribute__((noinline)) void fm_write (U8 addr, U8 val)
{
	fm_write_inline (addr, val, 0);
}


/**
 * Reads from a FM-chip register.
 */
__attribute__((noinline)) U8 fm_read (U8 addr)
{
	return fm_read_inline (addr, 0);
}


void fm_init (void)
{
	U8 reg;

	/* Initialize the FM chip.
	 * Use the 'in_interrupt' version, since interrupts are now
	 * disabled. */
	for (reg=0; reg <= 0xFE; reg++)
		fm_write_inline (reg, 0, 1);
	fm_write_inline (0xFF, 0, 1);

	fm_write_inline (FM_ADDR_CLOCK_CTRL, FM_TIMER_FRESETA + FM_TIMER_FRESETB, 1);
	fm_write_inline (FM_ADDR_CLOCK_A1, 0xFD, 1);
	fm_write_inline (FM_ADDR_CLOCK_A2, 0x02, 1);
	fm_timer_restart (1);
}

