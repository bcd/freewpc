
#include <freewpc.h>

void amode_deff (void) __taskentry__
{
	extern void starfield_start (void);
	extern void starfield_stop (void);

	for (;;)
	{
		int i;
		
		/** Display game over screen **/
		dmd_alloc_low_clean ();
		font_render_string_center (&font_5x5, 64, 16, "GAME OVER");
		dmd_show_low ();
		task_sleep_sec (5);

		/** Display FreeWPC message **/
		dmd_alloc_low_clean ();
		font_render_string_center (&font_5x5, 64, 13, "FREEWPC");
		dmd_copy_low_to_high ();
		dmd_invert_page (dmd_low_buffer);
		deff_swap_low_high (24, TIME_100MS + TIME_50MS);

		/** Display TZ 2005 message **/
		dmd_alloc_low_high ();
		dmd_clean_page_low ();
		font_render_string_center (&font_5x5, 64, 10, "TWILIGHT ZONE");
		starfield_start ();
		dmd_copy_low_to_high ();
		font_render_string_center (&font_5x5, 64, 20, "2005");
		deff_swap_low_high (17, TIME_100MS * 2);
		starfield_stop ();

		for (i = 32; i != 0; --i)
		{
			dmd_shift_up (dmd_low_buffer);
			task_sleep (TIME_33MS);
		}

		/** Display last set of player scores **/
		dmd_alloc_low_clean ();
		scores_draw ();
		dmd_show_low ();
		task_sleep_sec (7);

		/** Display high scores **/

		/** Display credits message **/
		credits_draw ();
		credits_draw ();

		/** Display 'custom message'? **/
	}
}


void amode_start (void)
{
	deff_start (DEFF_AMODE);
}

void amode_stop (void)
{
	deff_stop (DEFF_AMODE);
}

