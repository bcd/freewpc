/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>
#include <amode.h>

/**
 * \file
 * \brief System reset message screen and user acceptance of the software.
 */

#define ACCEPT_1	0x19
#define ACCEPT_2	0x75
#define ACCEPT_3	0xB9

#ifdef CONFIG_PLATFORM_WPC
#define ACCEPT_BUTTON SW_ENTER
#endif
#ifdef CONFIG_PLATFORM_WHITESTAR
#define ACCEPT_BUTTON SW_BLACK_BUTTON
#endif

volatile static const char gcc_version[] = C_STRING(GCC_VERSION);

__nvram__ U8 freewpc_accepted[3];

extern __common__ void opto_check (void);


/**
 * Perform a full factory reset.
 * This can be triggered automatically at boot time if
 * certain sanity checks fail, or via a test mode option.
 */
void factory_reset (void)
{
	adj_reset_all ();
	rtc_factory_reset ();
	/* TODO : this should also clear audits, reset the high scores,
	 * reset the custom message/game ID, and clear the persistent
	 * area. */
#ifdef __m6809__
	memset (AREA_BASE (permanent), 0, AREA_SIZE (permanent));
#endif
	callset_invoke (factory_reset);
}


void factory_reset_if_required (void)
{
	if (0)
	{
		factory_reset ();
		/* TODO - display message to this effect */
	}
}


void render_build_date (void)
{
	locale_render_date (BUILD_MONTH, BUILD_DAY, BUILD_YEAR);
}


extern inline void wait_for_button (const U8 swno)
{
	while (!switch_poll (swno))
		task_sleep (TIME_66MS);

	while (switch_poll (swno))
		task_sleep (TIME_66MS);
}


void system_accept_freewpc (void)
{
#ifdef CONFIG_NATIVE
	return;
#endif

	if ((freewpc_accepted[0] == ACCEPT_1) &&
		 (freewpc_accepted[1] == ACCEPT_2) &&
		 (freewpc_accepted[2] == ACCEPT_3))
		return;

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 3, "FREEWPC");
	font_render_string_center (&font_mono5, 64, 9, "WARNING... BALLY WMS");
	font_render_string_center (&font_mono5, 64, 15, "DOES NOT SUPPORT");
	font_render_string_center (&font_mono5, 64, 21, "THIS SOFTWARE");
	font_render_string_center (&font_mono5, 64, 27, "PRESS ENTER");
	dmd_show_low ();
	wait_for_button (ACCEPT_BUTTON);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 3, "FREEWPC");
	font_render_string_center (&font_mono5, 64, 9, "NO WARRANTY EXISTS");
	font_render_string_center (&font_mono5, 64, 15, "ROM MAY CAUSE DAMAGE");
	font_render_string_center (&font_mono5, 64, 21, "TO REAL MACHINE");
	font_render_string_center (&font_mono5, 64, 27, "PRESS ENTER");
	dmd_show_low ();
	wait_for_button (ACCEPT_BUTTON);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 3, "FREEWPC");
	font_render_string_center (&font_mono5, 64, 9, "IF YOU ARE SURE YOU");
	font_render_string_center (&font_mono5, 64, 15, "WANT TO CONTINUE");
	font_render_string_center (&font_mono5, 64, 21, "PRESS ENTER TWICE");
	dmd_show_low ();
	wait_for_button (ACCEPT_BUTTON);
	wait_for_button (ACCEPT_BUTTON);

	dmd_alloc_low_clean ();
	dmd_show_low ();
	task_sleep_sec (1);

	wpc_nvram_get ();
	freewpc_accepted[0] = ACCEPT_1;
	freewpc_accepted[1] = ACCEPT_2;
	freewpc_accepted[2] = ACCEPT_3;
	wpc_nvram_put ();

	factory_reset ();
}


/** Display effect for system reset */
void system_reset_deff (void)
{
	dmd_alloc_low_clean ();

	font_render_string_left (&font_mono5, 1, 1, MACHINE_NAME);

#ifdef DEBUGGER
	sprintf ("D%s.%s", C_STRING(MACHINE_MAJOR_VERSION), C_STRING(MACHINE_MINOR_VERSION));
#else
	sprintf ("R%s.%s", C_STRING(MACHINE_MAJOR_VERSION), C_STRING(MACHINE_MINOR_VERSION));
#endif
	font_render_string_right (&font_mono5, 127, 1, sprintf_buffer);

	render_build_date ();
	font_render_string_left (&font_mono5, 1, 9, sprintf_buffer);

#if (MACHINE_PIC == 1)
	pic_render_serial_number ();
	font_render_string_left (&font_mono5, 1, 18, sprintf_buffer);
#endif

	font_render_string_left (&font_mono5, 1, 26, "TESTING...");
#if defined(USER_TAG)
	font_render_string_right (&font_mono5, 127, 26, C_STRING(USER_TAG));
#endif

	dmd_show_low ();

	task_sleep_sec (3);
	while (sys_init_pending_tasks != 0)
		task_sleep (TIME_66MS);

	dbprintf ("System initialized.\n");
	sys_init_complete++;
	deff_exit ();
}



void system_reset (void)
{
	system_accept_freewpc ();

#if (MACHINE_PIC == 1)
	pic_init ();
#endif

	/* Check the 12V supply to make sure optos are working */
	opto_check ();

	factory_reset_if_required ();

#ifdef FASTBOOT
	sys_init_complete++;
#else
	deff_start (DEFF_SYSTEM_RESET);
#endif
	amode_start ();
}

