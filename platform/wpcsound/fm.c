
#include <freewpc.h>


/**
 * Writes to a FM-chip register.
 */
void fm_write (U8 addr, U8 val)
{
	fm_write_inline (addr, val, 0);
	fm_wait ();
	writeb (WPCS_FM_ADDR_STATUS, addr);
	fm_wait ();
	writeb (WPCS_FM_DATA, val);
}


/**
 * Reads from a FM-chip register.
 */
U8 fm_read (U8 addr)
{
	return fm_read_inline (addr, 0);
}


void fm_init (void)
{
	fm_write (FM_ADDR_CLOCK_A1, 0xFD);
	fm_write (FM_ADDR_CLOCK_A2, 0x02);
	fm_timer_restart (0);
}

