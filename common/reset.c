/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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
#include <diag.h>
#include <replay.h>

/**
 * \file
 * \brief System reset message screen and user acceptance of the software.
 */

#define ACCEPT_1	0x19
#define ACCEPT_2	0x75
#define ACCEPT_3	0xB9


volatile static const char gcc_version[] = C_STRING(GCC_VERSION);

__nvram__ U8 freewpc_accepted[3];

extern __common__ void opto_check (void);


/**
 * Wait for a button to be pressed and released.
 */
extern inline void wait_for_button (const U8 swno)
{
	/* TODO - why is this not done in native mode? */
#ifndef CONFIG_NO_ACCEPT
#ifndef CONFIG_NATIVE
	while (!switch_poll (swno))
		task_sleep (TIME_66MS);

	while (switch_poll (swno))
		task_sleep (TIME_66MS);
#endif
#else
	task_sleep (TIME_500MS);
#endif
}


/**
 * Perform a full factory reset.
 * This can be triggered automatically at boot time if
 * certain sanity checks fail, or via a test mode option.
 */
void factory_reset (void)
{
	file_reset ();
#ifdef __m6809__
	memset (AREA_BASE (permanent), 0, AREA_SIZE (permanent));
#else
	/* TODO - how to clean the permanent area in native mode? */
#endif
	timestamp_update (&system_timestamps.factory_reset);
	callset_invoke (factory_reset);
}


/**
 * See if a factory reset is needed.  The 'init_ok' event is
 * thrown and any catchers can return FALSE if a factory reset
 * should occur.  The accept button must be pressed afterwards
 * to confirm.
 */
void factory_reset_if_required (void)
{
	if (!callset_invoke_boolean (init_ok))
	{
		deff_stop (DEFF_SYSTEM_RESET);
#ifdef CONFIG_DMD_OR_ALPHA
		dmd_alloc_low_clean ();
		font_render_string_center (&font_mono5, 64, 10, "FACTORY SETTINGS");
		font_render_string_center (&font_mono5, 64, 20, "RESTORED");
		dmd_show_low ();
#endif
		factory_reset ();
		task_sleep_sec (4);
		warm_reboot ();
	}
}


void system_accept_freewpc (void)
{
	if ((freewpc_accepted[0] == ACCEPT_1) &&
		 (freewpc_accepted[1] == ACCEPT_2) &&
		 (freewpc_accepted[2] == ACCEPT_3))
		return;

#ifdef CONFIG_DMD
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
#endif

	pinio_nvram_unlock ();
	freewpc_accepted[0] = ACCEPT_1;
	freewpc_accepted[1] = ACCEPT_2;
	freewpc_accepted[2] = ACCEPT_3;
	pinio_nvram_lock ();

	factory_reset ();
	warm_reboot ();
}


/** Display effect for system reset */
void system_reset_deff (void)
{
	/* On DMD machines display the full copyright screen.  Alphanumerics
	aren't large enough to do this. */
#if (MACHINE_DMD == 1)
	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 3, "FREEWPC (C) COPYRIGHT " C_STRING(BUILD_YEAR));
	font_render_string_center (&font_var5, 64, 11, "THIS SOFTWARE IS NOT");
	font_render_string_center (&font_var5, 64, 17, "SUPPORTED BY BALLY/WILLIAMS");
	font_render_string_center (&font_var5, 64, 25, "WWW.ODDCHANGE.COM/FREEWPC");
	dmd_show_low ();
#endif
#if (MACHINE_ALPHANUMERIC == 1)
	seg_alloc_clean ();
	seg_write_row_center (0, "FREEWPC <C>" C_STRING(BUILD_YEAR));
	seg_write_row_center (1, "WWW.ODDCHANGE.COM");
	seg_show ();
#endif
	task_sleep_sec (3);
#ifdef CONFIG_DMD_OR_ALPHA
	dmd_alloc_low_clean ();
	font_render_string_left (&font_mono5, 1, 1, MACHINE_NAME);

#ifdef DEBUGGER
	sprintf ("D%s.%s", C_STRING(MACHINE_MAJOR_VERSION), C_STRING(MACHINE_MINOR_VERSION));
#else
	sprintf ("R%s.%s", C_STRING(MACHINE_MAJOR_VERSION), C_STRING(MACHINE_MINOR_VERSION));
#endif
	font_render_string_right (&font_mono5, 127, 1, sprintf_buffer);

	SECTION_VOIDCALL (__common__, render_build_date);
	font_render_string_left (&font_mono5, 1, 9, sprintf_buffer);

#if (MACHINE_PIC == 1)
	pic_render_serial_number ();
	font_render_string_left (&font_mono5, 1, 18, sprintf_buffer);
#endif

	font_render_string_left (&font_mono5, 1, 26, "TESTING...");
	dmd_show_low ();
#endif

	/* Keep the reset display for at least 3 seconds (so
	 * it is readable), keep it longer if any of the
	 * asynchronous initializations are still running. */
	task_sleep_sec (3);
	while (sys_init_pending_tasks != 0)
		task_sleep (TIME_66MS);

	/* If there were any diagnostic errors discovered, announce
	this fact. */
	diag_announce_if_errors ();

	dbprintf ("System initialized.\n");
	deff_exit ();
}


/**
 * Called when the system hardware is fully initialized.
 * This function performs final initialization.
 */
void system_reset (void)
{
	system_accept_freewpc ();

	/* Start the reset display effect, which shows
	 * the machine name, version, etc. */
	deff_start (DEFF_SYSTEM_RESET);

	/* Perform final checks on the switch matrix to make sure
	 * it is working properly. */
	/* TODO - poll certain switches that must be operational
	 * before allowing the system to complete init. */
	opto_check ();

	/* Check various persistent variables for sane values.
	 * If there are any incompatibilities, perform a factory
	 * reset to be safe. */
	factory_reset_if_required ();

	/* Mark hardware initialization complete.  This will
	 * allow switch scanning to start, so sleep briefly
	 * to allow that to happen once. */
	sys_init_complete++;
	task_sleep (TIME_66MS);

	/* Invoke other final initializations. */
	callset_invoke (init_complete);

	/* Bump the power-up audit */
	audit_increment (&system_audits.power_ups);
	log_event (SEV_INFO, MOD_SYSTEM, EV_SYSTEM_INIT, 0);

	/* Start the attract mode effects */
	amode_start ();

	/* In test-only mode, pretend ENTER was pressed
	 * and go straight to test mode. */
#ifdef MACHINE_TEST_ONLY
	while (sys_init_pending_tasks != 0)
		task_sleep (TIME_66MS);
	callset_invoke (sw_enter);
#endif
}

