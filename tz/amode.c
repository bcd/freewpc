
#include <freewpc.h>

extern void starfield_start (void);

void amode_page_delay (U8 secs)
{
	U8 amode_flippers;
	U8 amode_flippers_start;

	/* Convert secs to 66ms ticks */
	secs <<= 4;

	amode_flippers_start = switch_poll_logical (SW_LL_FLIP_SW);
	while (secs != 0)
	{
		task_sleep (TIME_66MS);
		amode_flippers = switch_poll_logical (SW_LL_FLIP_SW);

		if ((amode_flippers != amode_flippers_start) &&
			 (amode_flippers != 0))
		{
			return;
		}
		amode_flippers_start = amode_flippers;
		secs--;
	}
}

void amode_flipper_sound (void)
{
}

void amode_scroll (void)
{
}

void amode_left_flipper (void)
{
	amode_flipper_sound ();
	amode_scroll ();
}


void amode_right_flipper (void)
{
	amode_flipper_sound ();
	amode_scroll ();
}


void amode_leff (void) __taskentry__
{
	U8 i;

	triac_enable (TRIAC_GI_MASK);
	for (;;)
	{
		lampset_set_apply_delay (0);
		lampset_apply_on (LAMPSET_ALL);

		lampset_set_apply_delay (TIME_33MS);
		lampset_apply_toggle (LAMPSET_ALL);
	
		lampset_set_apply_delay (TIME_16MS);
		lampset_apply_toggle (LAMPSET_ALL);

		lampset_set_apply_delay (0);
		for (i=0; i < 64; i += 2)
			lamp_on (i);
		for (i=1; i < 64; i += 2)
			lamp_off (i);

		for (i=0; i < 16; i++)
		{
			lampset_apply_toggle (LAMPSET_ALL);
			task_sleep (TIME_100MS * 2);
		}
	}
}


void amode_deff (void) __taskentry__
{
	extern char freewpc_bits[];

	for (;;)
	{
		int i;
		
		/** Display game over screen **/
		dmd_alloc_low_clean ();
		font_render_string_center (&font_5x5, 64, 16, "GAME OVER");
		dmd_show_low ();
		amode_page_delay (5);

		/** Display last set of player scores **/
		dmd_alloc_low_clean ();
		scores_draw ();
		dmd_show_low ();
		amode_page_delay (7);

		/** Display FreeWPC logo **/
		dmd_alloc_low ();
		dmd_copy_page (dmd_low_buffer, (dmd_buffer_t *)freewpc_bits);
		dmd_show_low ();
		dmd_copy_low_to_high ();
		dmd_invert_page (dmd_low_buffer);
		deff_swap_low_high (25, TIME_100MS + TIME_50MS);
		amode_page_delay (3);

		/** Display TZ 2005 message **/
		dmd_alloc_low_high ();
		dmd_clean_page_low ();
		font_render_string_center (&font_5x5, 64, 10, "TWILIGHT ZONE");
		starfield_start ();
		dmd_copy_low_to_high ();
		font_render_string_center (&font_5x5, 64, 20, "2005");
		deff_swap_low_high (23, TIME_100MS * 2);

		for (i = 32; i != 0; --i)
		{
			dmd_shift_up (dmd_low_buffer);
			task_sleep (TIME_33MS);
		}

		/** Display high scores **/

		/** Display credits message **/
		credits_draw ();
		credits_draw ();

		/** Display 'custom message'? **/

		/** Display rules hint **/
		dmd_alloc_low_clean ();
		dmd_draw_border (dmd_low_bytes);
		font_render_string_center (&font_5x5, 64, 5, "HOLD FLIPPERS");
		font_render_string_center (&font_5x5, 64, 13, "TO VIEW THE");
		font_render_string_center (&font_5x5, 64, 21, "GAME RULES");
		dmd_show_low ();
		amode_page_delay (5);
	}
}


void amode_start (void)
{
	deff_start (DEFF_AMODE);
	task_create_gid (GID_LAMP_DEMO, amode_leff);
}

void amode_stop (void)
{
	deff_stop (DEFF_AMODE);
	task_kill_gid (GID_LAMP_DEMO);
	lamp_all_off ();
}

