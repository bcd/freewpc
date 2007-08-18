
#include <freewpc.h>


static U8 console_data[64];
static U8 console_write_off;
static U8 console_read_off;


void dmd_console_deff (void)
{
	U8 last_read_off;

	for (;;)
	{
		last read_off = console_read_off;

		dmd_alloc_low_clean ();
			
		dmd_show_low ();
	
		do {
			task_sleep (TIME_100MS);
		} while (console_read_off == last_read_off);
			
	}
}


void dmd_console_toggle (void)
{
	if (deff_get_active () == DEFF_CONSOLE)
	{
		deff_stop (DEFF_CONSOLE);
	}
	else
	{
		deff_start (DEFF_CONSOLE);
	}
}


void dmd_console_putc (char c)
{
	console_data[console_write_off] = c;
	console_write_off = (console_write_off + 1) % 64;
}


void dmd_console_init (void)
{
	console_write_off = console_read_off = 0;
}

