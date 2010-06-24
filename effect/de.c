
#include <freewpc.h>

#define DE_RESET 0x0
#define DE_CLEANED 0x1
#define DE_FULL_FLASH 0x2
#define DE_DIM_FLASH 0x4
#define DE_COLOR 0x8

U8 de_flags;
U8 de_loop_count;
U8 de_flash_time;
U8 de_delay;


/** Start a new display page */
void de_start_page (void)
{
	dmd_alloc_pair ();
	de_flags = DE_RESET;
	de_delay = TIME_1500MS;
}

/** Add a full-size, mono 128x32 frame to the current page */
void de_add_mono_frame (U16 id)
{
	frame_draw_plane (id);
	de_flags |= DE_CLEANED;
}

/** Add a full-size, 4-color 128x32 frame to the current page */
void de_add_frame (U16 id)
{
	frame_draw (id);
	de_flags |= (DE_CLEANED | DE_COLOR);
}

/** Set the current font */
void de_set_font (const font_t *font)
{
	font_args.font = font;
}

/** Print text */
void de_print1 (const char *s)
{
	if (!(de_flags & DE_CLEANED))
	{
		dmd_clean_page_low ();
		de_flags |= DE_CLEANED;
	}

	if (de_flags & (DE_FULL_FLASH | DE_DIM_FLASH))
	{
	}
}

void de_set_delay (task_ticks_t delay)
{
	de_delay = delay;
}

void de_set_flash (bool flashing)
{
	if (flashing == TRUE)
	{
		de_flags |= DE_FULL_FLASH;
		de_flash_time = TIME_100MS;
		if (de_flags & DE_COLOR)
		{
		}
		else
		{
			dmd_copy_low_to_high ();
		}
	}
}

void de_set_dim_flash (bool flashing)
{
	if (flashing == TRUE)
		de_flags |= DE_DIM_FLASH;
}


/** Mark the end of a page */
void de_end_page (void)
{
	if (de_flags & DE_FULL_FLASH)
	{
		if (de_flags & DE_COLOR)
		{
		}
		else
		{
			de_loop_count = de_delay / de_flash_time;
			deff_swap_low_high (de_loop_count, de_flash_time);
		}
	}
	else if (de_flags & DE_DIM_FLASH)
	{
		if (de_flags & DE_COLOR)
		{
		}
		else
		{
		}
	}
	else
	{
		if (de_flags & DE_COLOR)
			dmd_show2 ();
		else
			dmd_show_low ();
	}
	task_sleep (de_delay);
}


void de_animate (U16 start, U16 end, U8 delay)
{
	while (start <= end)
	{
		frame_draw (start++);
		dmd_show2 ();
		task_sleep (delay);
	}
}

