
#include <freewpc.h>

U8 egg_code_values[3];
U8 egg_index;

extern void starfield_start (void);

bool amode_page_delay (U8 secs)
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
			return TRUE;
		}
		amode_flippers_start = amode_flippers;
		secs--;
	}
	return FALSE;
}

void amode_flipper_sound_debounce_timer (void)
{
	task_sleep_sec (10);
	task_sleep_sec (10);
	task_exit ();
}

void amode_flipper_sound (void)
{
	if (!task_find_gid (GID_AMODE_FLIPPER_SOUND_DEBOUNCE))
	{
		task_create_gid (GID_AMODE_FLIPPER_SOUND_DEBOUNCE,
			amode_flipper_sound_debounce_timer);
		sound_send (SND_THUD);
	}
}

void amode_scroll (void)
{
}

void egg_timer_task (void)
{
	task_sleep_sec (7);
	task_exit ();
}

void egg_left_flipper (void)
{
	if (!task_find_gid (GID_EGG_TIMER))
	{
		task_create_gid1 (GID_EGG_TIMER, egg_timer_task);
		egg_index = 0;
		egg_code_values[0] = 0;
		egg_code_values[1] = 0;
		egg_code_values[2] = 0;
	}
	egg_code_values[egg_index]++;
}

void egg_brian_image_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_draw_bitmap (brian_bits, 0, 0, 32, 32);
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

void egg_right_flipper (void)
{
	if (!task_find_gid (GID_EGG_TIMER)) 
		return;
	egg_index++;
	if (egg_index == 3)
	{
		dbprintf ("\nEgg code %d %d %d entered\n", egg_code_values[0], egg_code_values[1], egg_code_values[2]);
		if ((egg_code_values[0] == 2) &&
			 (egg_code_values[1] == 3) &&
			 (egg_code_values[2] == 4))
		{
			deff_start (DEFF_BRIAN_IMAGE);
		}
	}
}

void amode_left_flipper (void)
{
	amode_flipper_sound ();
	amode_scroll ();
	egg_left_flipper ();
}


void amode_right_flipper (void)
{
	amode_flipper_sound ();
	amode_scroll ();
	egg_right_flipper ();
}

void amode_lamp_toggle_task (void) __taskentry__
{
	lampset_apply_leff_toggle (LAMPSET_AMODE_ALL);
	task_exit ();
}


void amode_leff (void) __taskentry__
{
	U8 i;

	triac_enable (TRIAC_GI_MASK);
	lampset_set_apply_delay (0);
	for (;;)
	{
		lampset_apply_leff_off (LAMPSET_AMODE_ALL);

		lampset_set_apply_delay (TIME_16MS);
		for (i=0; i < 6; i++)
		{
			lampset_apply_leff_toggle (LAMPSET_AMODE_ALL);
			task_sleep (TIME_100MS * 3);
		}

		lampset_set_apply_delay (0);
		lampset_apply_leff_alternating (LAMPSET_AMODE_ALL, 0);
		for (i=0; i < 25; i++)
		{
			lampset_apply_leff_toggle (LAMPSET_AMODE_ALL);
			task_sleep (TIME_100MS * 2);
		}
	}
}


void amode_deff (void) __taskentry__
{
	/** Display game over screen **/
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 16, "GAME OVER");
	dmd_show_low ();
	amode_page_delay (5);

	for (;;)
	{
		/** Display last set of player scores **/
		dmd_alloc_low_clean ();
		scores_draw ();
		dmd_show_low ();
		if (amode_page_delay (7) && system_config.tournament_mode)
			continue;

		/** Display FreeWPC logo **/
		dmd_alloc_low_high ();
		dmd_draw_image2 (freewpc_logo_bits);
		dmd_show2 ();
		if (amode_page_delay (7) && system_config.tournament_mode)
			continue;

		/** Display game title message **/
		dmd_alloc_low_high ();
		dmd_clean_page_low ();
		font_render_string_center (&font_5x5, 64, 10, "BACK TO THE ZONE");
		starfield_start ();
		dmd_copy_low_to_high ();
		font_render_string_center (&font_5x5, 64, 20, "TZ 2006");
		deff_swap_low_high (23, TIME_100MS * 2);

		/** Display high scores **/

		/** Display PLAY PINBALL message **/
		dmd_sched_transition (&trans_scroll_left);
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed10, 64, 16, 
			"PLAY PINBALL");
		dmd_show_low ();
		if (amode_page_delay (3) && system_config.tournament_mode)
			continue;

		/** Display credits message **/
		credits_draw ();
		credits_draw ();

		/** Display 'custom message'? **/

#if 00000
		/** Display rules hint **/
		dmd_alloc_low_clean ();
		dmd_draw_border (dmd_low_bytes);
		font_render_string_center (&font_5x5, 64, 5, "HOLD FLIPPERS");
		font_render_string_center (&font_5x5, 64, 13, "TO VIEW THE");
		font_render_string_center (&font_5x5, 64, 21, "GAME RULES");
		dmd_show_low ();
		amode_page_delay (5);
#endif

		/* Kill music if it is running */
		music_set (MUS_OFF);
	}
}

