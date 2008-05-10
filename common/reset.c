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

volatile static const char gcc_version[] = C_STRING(GCC_VERSION);

__nvram__ U8 freewpc_accepted[3];


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
	wait_for_button (SW_ENTER);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 3, "FREEWPC");
	font_render_string_center (&font_mono5, 64, 9, "NO WARRANTY EXISTS");
	font_render_string_center (&font_mono5, 64, 15, "ROM MAY CAUSE DAMAGE");
	font_render_string_center (&font_mono5, 64, 21, "TO REAL MACHINE");
	font_render_string_center (&font_mono5, 64, 27, "PRESS ENTER");
	dmd_show_low ();
	wait_for_button (SW_ENTER);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 3, "FREEWPC");
	font_render_string_center (&font_mono5, 64, 9, "IF YOU ARE SURE YOU");
	font_render_string_center (&font_mono5, 64, 15, "WANT TO CONTINUE");
	font_render_string_center (&font_mono5, 64, 21, "PRESS ENTER TWICE");
	dmd_show_low ();
	wait_for_button (SW_ENTER);
	wait_for_button (SW_ENTER);

	dmd_alloc_low_clean ();
	dmd_show_low ();
	task_sleep_sec (1);

	wpc_nvram_get ();
	freewpc_accepted[0] = ACCEPT_1;
	freewpc_accepted[1] = ACCEPT_2;
	freewpc_accepted[2] = ACCEPT_3;
	wpc_nvram_put ();

	adj_reset_all ();
	rtc_factory_reset ();
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

#ifdef FASTBOOT
	sys_init_complete++;
#else
	deff_start (DEFF_SYSTEM_RESET);
#endif
	amode_start ();
}

