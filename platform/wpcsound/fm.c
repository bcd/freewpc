
#include <freewpc.h>


/**
 * Waits until the FM chip is ready to receive a new
 * read/write operation.
 */
extern inline void fm_wait (void)
{
	__asm__ volatile (
		"lda\t" C_STRING(WPCS_FM_DATA) "\n"
		"\tbmi\t.-3"
	);
}


/**
 * Writes to a FM-chip register.
 */
__attribute__((noinline)) void fm_write (U8 addr, U8 val)
{
	fm_wait ();
	writeb (WPCS_FM_ADDR_STATUS, addr);
	fm_wait ();
	writeb (WPCS_FM_DATA, val);
}


/**
 * Reads from a FM-chip register.
 */
__attribute__((noinline)) U8 fm_read (U8 addr)
{
	U8 val;

	fm_wait ();
	writeb (WPCS_FM_ADDR_STATUS, addr);
	fm_wait ();
	val = readb (WPCS_FM_DATA);
	return val;
}


void fm_restart_timer (void)
{
	fm_write (FM_ADDR_CLOCK_CTRL,
		FM_TIMER_FRESETA + FM_TIMER_IRQENA + FM_TIMER_LOADA);
}


void fm_init (void)
{
	fm_write (0x30, 0);
	fm_write (FM_ADDR_CLOCK_A1, 0xFD);
	fm_write (FM_ADDR_CLOCK_A2, 0x02);
	fm_restart_timer ();
}

