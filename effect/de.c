
#include <freewpc.h>

#define DE_RESET 0x0
#define DE_CLEANED 0x1
#define DE_FULL_FLASH 0x2
#define DE_DIM_FLASH 0x4
#define DE_COLOR 0x8

U8 de_flags;


/** Start a new display page */
void de_start_page (void)
{
	dmd_alloc_low_high ();
	de_flags = DE_RESET;
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

void de_set_flash (bool flashing)
{
	if (flashing == TRUE)
		de_flags |= DE_FULL_FLASH;
}

void de_set_dim_flash (bool flashing)
{
	if (flashing == TRUE)
		de_flags |= DE_DIM_FLASH;
}

/** Mark the end of a page */
void de_end_page (void)
{
	if (de_flags & DE_COLOR)
		dmd_show2 ();
	else
		dmd_show_low ();
}

