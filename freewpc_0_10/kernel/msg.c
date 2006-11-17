
#include <freewpc.h>

char msg_buffer[PRINTF_BUFFER_SIZE];


void
msg_task (void)
{
	dmd_pagenum_t orig_lo, orig_hi;
	dmd_pagenum_t msg_lo = 0xFF, msg_hi = 0xFF;
	extern dmd_pagenum_t dmd_low_page, dmd_high_page;
	U8 refresh_count;

	for (refresh_count = 0; refresh_count < 16; refresh_count++)
	{
		if ((dmd_low_page != msg_lo) || (dmd_high_page != msg_hi))
		{
			orig_lo = dmd_low_page;
			orig_hi = dmd_high_page;
			dmd_alloc_low_clean ();
			msg_lo = dmd_low_page;
			msg_hi = dmd_high_page;
		}

		__blockcopy16 (sprintf_buffer, msg_buffer, PRINTF_BUFFER_SIZE);
		font_render_string (&font_mono5, 0, 0, sprintf_buffer);
		dmd_show_low ();

		task_sleep (TIME_100MS);
	}
	task_exit ();
}


void
msgbox (void)
{
	__blockcopy16 (msg_buffer, sprintf_buffer, PRINTF_BUFFER_SIZE);
	task_recreate_gid (GID_DEBUG_MSG, msg_task);
}

