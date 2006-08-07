
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
		kickout_unlock (KLOCK_DEBUGGER);
	}
	else
	{
		deff_start (DEFF_INSPECTOR);
		kickout_lock (KLOCK_DEBUGGER);
	}
}

void inspector_fast_page_down (void)
{
	task_sleep (TIME_500MS);
	while (switch_poll_logical (SW_LL_FLIP_SW))
	{
		inspector_addr -= INSPECTOR_PAGE_SIZE * 4;
		task_sleep (TIME_33MS);
	}
	task_exit ();
}

void inspector_left_flipper (void)
{
	if (deff_get_active () == DEFF_INSPECTOR)
	{
		if (inspector_addr > 0)
			inspector_addr -= INSPECTOR_PAGE_SIZE;
		task_recreate_gid (GID_INSPECTOR_FAST_PAGE, inspector_fast_page_down);
	}
}

void inspector_fast_page_up (void)
{
	task_sleep (TIME_500MS);
	while (switch_poll_logical (SW_LR_FLIP_SW))
	{
		inspector_addr += INSPECTOR_PAGE_SIZE * 4;
		task_sleep (TIME_33MS);
	}
	task_exit ();
}

void inspector_right_flipper (void)
{
	if (deff_get_active () == DEFF_INSPECTOR)
	{
		inspector_addr += INSPECTOR_PAGE_SIZE;
		task_recreate_gid (GID_INSPECTOR_FAST_PAGE, inspector_fast_page_up);
	}
}

CALLSET_ENTRY (inspector, init)
{
	inspector_addr = 0;
}

