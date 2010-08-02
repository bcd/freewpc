
/* CALLSET_SECTION (__machine3__, tnf) */

#include <freewpc.h>

extern U8 mpf_buttons_pressed;

U8 tnf_x;
U8 tnf_y;

void tnf_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		sprintf ("%d DOINKS", mpf_buttons_pressed);
		font_render_string_center (&font_steel, 64 + tnf_x, 16 + tnf_y, sprintf_buffer);
		dmd_show_low ();
		if (mpf_buttons_pressed > 50)
			deff_exit ();
		task_sleep (TIME_33MS);
	}
}

CALLSET_ENTRY (tnf, tnf_button_pushed)
{
	bounded_increment (mpf_buttons_pressed, 255);
	sound_send (SND_GUMBALL_LOADED + random_scaled (16));
	tnf_x = random_scaled(10);
	tnf_y = random_scaled(8);
}

CALLSET_ENTRY (tnf, tnf_start)
{
	mpf_buttons_pressed = 0;
	tnf_x = 0;
	tnf_y = 0;
}

CALLSET_ENTRY (tnf, tnf_end)
{

}
