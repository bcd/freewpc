
#include <freewpc.h>

__fastram__ volatile char testbuf[128];

unsigned char firq_off;

unsigned char exec_off;

unsigned char firq_count;

unsigned char exec_count;

unsigned char irq_count;


void fault (void)
{	
	disable_interrupts ();
	for (;;);
}


void verify (unsigned char off)
{
	if (testbuf[off] != ~off)
		fault ();
	testbuf[off] = ~off;
}


void firq_handler (void)
{
	verify (firq_off);
	verify (firq_off+1);
	verify (firq_off+2);
	verify (firq_off+3);
	firq_off += 4;
	firq_off &= 0x7f;
	wpc_dmd_set_firq_row (30);

	if (++firq_count == 0)
		dmd_low_buffer[0] = ~dmd_low_buffer[0];
}

void irq_handler (void)
{
	if (++irq_count & 0xff)
		dmd_low_buffer[128] = ~dmd_low_buffer[128];
}

void exec (void)
{
	for (;;)
	{
		for (exec_off = 0; exec_off <= 0x7f; exec_off++)
			verify (exec_off);
		if (readb (WPC_SW_CABINET_INPUT) != 0)
			fault ();
		if (++exec_count == 0)
			dmd_low_buffer[64] = ~dmd_low_buffer[64];
	}
}

void init (void)
{
	writeb (WPC_DMD_LOW_PAGE, 0);
	writeb (WPC_DMD_ACTIVE_PAGE, 0);
	writeb (WPC_ZEROCROSS_IRQ_CLEAR, 0x96);
	wpc_dmd_set_firq_row (30);
	for (exec_off = 0; exec_off <= 0x7f; exec_off++)
		testbuf[exec_off] = ~exec_off;
	exec_off = firq_off = 0;
}

