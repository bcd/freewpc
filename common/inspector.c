
#include <freewpc.h>

#define INSPECTOR_PAGE_SIZE 16

U8 *inspector_addr;


void inspector_deff (void)
{
	U8 *addr, *p;
	U8 y;

	for (;;)
	{
		addr = inspector_addr;
		
		dmd_alloc_low_clean ();
		for (p = addr, y=2; p < addr + 0x10; p += 0x4, y += 7)
		{
			sprintf ("%p   %02X %02X %02X %02X", 
				p, p[0], p[1], p[2], p[3]);
			font_render_string_left (&font_mono5, 2, y, sprintf_buffer);
		}
		dmd_show_low ();

		while (addr == inspector_addr)
			task_sleep (TIME_66MS);
	}
	deff_exit ();
}

void inspector_buyin_button (void)
{
	if (deff_get_active () == DEFF_INSPECTOR)
	{
		deff_stop (DEFF_INSPECTOR);
	}
	else
	{
		deff_start (DEFF_INSPECTOR);
	}
}

void inspector_left_flipper (void)
{
	if (deff_get_active () == DEFF_INSPECTOR)
		if (inspector_addr > 0)
			inspector_addr -= INSPECTOR_PAGE_SIZE;
}

void inspector_right_flipper (void)
{
	if (deff_get_active () == DEFF_INSPECTOR)
		inspector_addr += INSPECTOR_PAGE_SIZE;
}

CALLSET_ENTRY (inspector, init)
{
	inspector_addr = 0;
}

