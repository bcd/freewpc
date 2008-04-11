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
#include <test.h>

/**
 * \file
 * \brief Entry point to the game program.
 */

/** The number of task ticks executed.  A tick equals 16 IRQs. */
__fastram__ U8 tick_count;

/** The number of FIRQs asserted */
__fastram__ U8 firq_count;

/** Nonzero when the system has finished initializing */
U8 sys_init_complete;

/** Nonzero when it is OK to call idle functions */
U8 idle_ok;

/** The number of background initialization tasks that are still
running.  The splash screen is kept until this reverts to zero. */
U8 sys_init_pending_tasks;

/** Indicates the last nonfatal error taken */
U8 last_nonfatal_error_code;

/** Indicate the task that was running when the last nonfatal happened */
task_gid_t last_nonfatal_error_gid;



/** Initialize the FreeWPC program. */
__noreturn__ void freewpc_init (void)
{
	extern __common__ void system_reset (void);

	/* Reset the sound board... the earlier the better */
	wpc_asic_write (WPCS_CONTROL_STATUS, 0);

	/* Initializing the RAM page */
	wpc_set_ram_page (0);

#ifdef __m6809__
	/* Install the null pointer catcher, by programming
	 * some SWI instructions at zero. */
	*(U8 *)0 = 0x3F;
	*(U8 *)1 = 0x3F;
#endif /* __m6809__ */

	/* Set up protected RAM */
	wpc_set_ram_protect (RAM_UNLOCKED);
	wpc_set_ram_protect_size (RAM_LOCK_2K);
	wpc_set_ram_protect (RAM_LOCKED);

	/* Initialize the ROM page register 
	 * page of ROM adjacent to the system area is mapped.
	 * This is the default location for machine-specific files. */
	wpc_set_rom_page (MACHINE_PAGE);

	/* Initialize other critical WPC output registers relating
	 * to hardware */
	wpc_asic_write (WPC_SOL_FLASH2_OUTPUT, 0);
	wpc_asic_write (WPC_SOL_HIGHPOWER_OUTPUT, 0);
	wpc_asic_write (WPC_SOL_FLASH1_OUTPUT, 0);
	wpc_asic_write (WPC_SOL_LOWPOWER_OUTPUT, 0);
	wpc_asic_write (WPC_LAMP_ROW_OUTPUT, 0);
	wpc_asic_write (WPC_GI_TRIAC, 0);

	/* Reset the blanking and watchdog circuitry.
	 * Eventually, the watchdog will be tickled every 1ms
	 * by the IRQ; until interrupts are enabled, we will
	 * have to do this periodically ourselves. */
	wpc_watchdog_reset ();

	/* Set init complete flag to false.  When everything is
	 * ready, we'll change this to a 1. */
	sys_init_complete = 0;
	idle_ok = 0;
	sys_init_pending_tasks = 0;

	/* Initialize all of the other kernel subsystems,
	 * starting with the hardware-centric ones and moving on
	 * to software features. */

	/* Initialize the real-time scheduler.  The periodic functions
	are scheduled at compile-time  using the 'gensched' utility. */
	VOIDCALL (tick_init);

	/* Initialize the hardware */
#ifdef DEBUGGER
	db_init ();
	wpc_watchdog_reset ();
#endif
	ac_init ();
	wpc_watchdog_reset ();
	sol_init ();
	wpc_watchdog_reset ();
	triac_init ();
	wpc_watchdog_reset ();
#if (MACHINE_DMD == 1)
	dmd_init ();
	wpc_watchdog_reset ();
#endif
	switch_init ();
	wpc_watchdog_reset ();
	flipper_init ();
	wpc_watchdog_reset ();
	lamp_init ();
	wpc_watchdog_reset ();
	device_init ();
	wpc_watchdog_reset ();
	free_timer_init ();
	wpc_watchdog_reset ();

	/* task_init is somewhat special in that it transforms the system
	 * from a single task into a multitasking one.  After this, tasks
	 * can be spawned if need be.  A task is created for the current
	 * thread of execution, too. */
	task_init ();
	wpc_watchdog_reset ();

#ifdef CONFIG_NATIVE
	linux_init ();
#endif

	/* Initialize the sound board early in a background
	 * thread, since it involves polling for data back from it,
	 * which may take unknown (or even infinite) time. */
	sys_init_pending_tasks++;
	task_create_gid (GID_SOUND_INIT, sound_init);

	/* Enable interrupts (IRQs and FIRQs).  Do this as soon as possible,
	 * but not before all of the hardware modules are done. */
	enable_interrupts ();

	/* Initialize everything else.  Some of these are given explicitly
	to force a particular order, since callsets do not guarantee the
	order of invocation.  For most things the order doesn't matter. */
	deff_init ();
	leff_init ();
	test_init ();
	adj_init ();
	callset_invoke (init);

	/* Check all adjustments and make sure that their checksums are valid.
	If problems are found, those adjustments will be made sane again. */
	csum_area_check_all ();

	/* Enable the idle processing.  Sleep briefly so that it gets
	 * a chance to run before continuing; this lets the debugger
	 * interface initialize. */
	idle_ok = 1;
	task_sleep (TIME_16MS);

	/* The system is mostly usable at this point.
	 * Now, start the display effect that runs at powerup.
	 * But in a test-only build, go directly to test mode.
	 */
#ifdef MACHINE_TEST_ONLY
	sys_init_complete++;
	callset_invoke (sw_enter);
#else
	system_reset ();

	/* Bump the power-up audit */
	audit_increment (&system_audits.power_ups);
#endif

	/* The system can run itself now, this task is done!
	 *
	 * In the simulator, the main task is not supposed to exit, so there
	 * is a conditional for that.
	 */
#ifndef CONFIG_NATIVE
	task_exit ();
#else
	while (1)
	{
		/* TODO - drop priority for idle tasks */
		task_sleep (TIME_33MS);
		db_idle ();
		callset_invoke (idle);
	}
#endif
}


/**
 * The lockup check routine examines 'task_dispatching_ok', which
 * should normally be true as normal task scheduling occurs.  
 * If this value stays false, something is very wrong.
 *
 * This check occurs every 128 IRQs.  No task should run for
 * that long without giving up control.  If the flag stays false
 * between 2 consecutive calls, we invoke a fatal error and reset.
 *
 * NOTE: if a task _really_ does take that long to execute before
 * switching out, it should set "task_dispatching_ok = TRUE"
 * periodically, to avoid a time out here.  This should rarely be
 * used.  (Even better, it should "task_yield" and let other tasks
 * run for a while.)
 */
void lockup_check_rtt (void)
{
#ifndef CONFIG_NATIVE
	if (sys_init_complete && !task_dispatching_ok)
	{
		fatal (ERR_TASK_LOCKUP);
	}
	else
	{
		task_dispatching_ok = FALSE;
	}
#endif
}


/**
 * Entry point for errors that are nonrecoverable.
 * error_code is one of the values in include/sys/errno.h.
 */
__noreturn__ 
void fatal (errcode_t error_code)
{
	/* Don't allow any more interrupts, since they might be the
	source of the error.  Since FIRQ is disabled, we can only
	do mono display at this point. */
	disable_interrupts ();

	/* Reset hardware outputs */
	wpc_asic_write (WPC_GI_TRIAC, 0);
	wpc_write_flippers (0);
	wpc_write_ticket (0);
	wpc_asic_write (WPC_SOL_HIGHPOWER_OUTPUT, 0);
	wpc_asic_write (WPC_SOL_LOWPOWER_OUTPUT, 0);
	wpc_asic_write (WPC_SOL_FLASH1_OUTPUT, 0);
	wpc_asic_write (WPC_SOL_FLASH2_OUTPUT, 0);
#ifdef MACHINE_SOL_EXTBOARD1
	wpc_asic_write (WPC_EXTBOARD1, 0);
#endif

	/* Audit the error. */
	audit_increment (&system_audits.fatal_errors);
	audit_assign (&system_audits.lockup1_addr, error_code);
	audit_assign (&system_audits.lockup1_pid_lef, task_getgid ());

#if (MACHINE_DMD == 1)
	/* Try to display the error on the DMD.  This may not work,
	you know. */
	extern void dmd_rtt0 (void);
	dmd_alloc_low_clean ();

	dbprintf ("Fatal error: %i\n", error_code);

	sprintf ("ERRNO %i", error_code);
	font_render_string_center (&font_mono5, 64, 2, sprintf_buffer);

	sprintf ("GID %i", task_getgid ());
	font_render_string (&font_mono5, 64, 8, sprintf_buffer);

	dmd_show_low ();
	dmd_rtt0 ();
#endif

	/* Dump all of the task information to the debugger port. */
	task_dump ();

#ifdef CONFIG_NATIVE
	task_sleep_sec (2);
	linux_shutdown ();
#else
	/* Go into a loop, long enough for the error message to be visible.
	Then reset the system. */
	{
		U16 count, secs;
		for (secs = 0; secs < 10; secs++)
		{
			for (count = 0; count < 25000; count++)
			{
				/* 4 nops = 8 cycles.  Loop overhead is about 6, so
				 * that's 14 cycles total for the inner loop.
				 * At 2M cycles per sec, we need ~15000 iterations per second */
				noop ();
				noop ();
				noop ();
				noop ();
			}
		}
	}

	/* Defining STOP_ON_ERROR is helpful during debugging a problem.
	Having the machine reset makes it hard to debug after the fact. */
#ifdef STOP_ON_ERROR
	while (1);
#endif

	/* Restart the system automatically (only on the 6809) */
#ifdef __m6809__
	start ();
#else
	freewpc_init ();
#endif
#endif
}


void nonfatal (errcode_t error_code)
{
	audit_increment (&system_audits.non_fatal_errors);
#ifdef DEBUGGER
	last_nonfatal_error_code = error_code;
	last_nonfatal_error_gid = task_getgid ();
	deff_start (DEFF_NONFATAL_ERROR);
#endif
}


/**
 * do_firq is the entry point from the FIRQ vector.  This interrupt
 * is generated from the WPC ASIC on two different occasions: (1)
 * when the DMD controller has been programmed to generate an 
 * interrupt after drawing a particular scan line, and (2) when the
 * WPC's peripheral timer register reaches zero.  The type of interrupt
 * can be determined by reading the peripheral timer register.
 *
 * The peripheral timer is currently unused.  Real WPC games only used
 * it on the alphanumeric machines, supposedly.  PinMAME doesn't handle
 * it correctly, so the check is kept.
 */
__interrupt__
void do_firq (void)
{
	/* The processor does not save/restore all registers on
	entry to an FIRQ, unlike IRQ.  So the handler is responsibly
	for doing this for all registers that might be used which are
	not ordinarily saved automatically.  The 6809 toolchain
	does not save X and D automatically, thus they need to be
	saved explicitly.  (It is possible that an application may
	know definitely that they are *not* used and thus this step
	can be skipped, but FreeWPC does not make any assumptions.) */
#ifdef __m6809__
	m6809_firq_save_regs ();
#endif

	/* Read the peripheral timer register.
	 * If bit 7 is set, it is a timer interrupt.  Otherwise,
	 * it is a DMD interrupt. */
	if (wpc_asic_read (WPC_PERIPHERAL_TIMER_FIRQ_CLEAR) & 0x80)
	{
		/* It is a timer interrupt.
		 * Clear the interrupt by writing back to the same register. */
		wpc_asic_write (WPC_PERIPHERAL_TIMER_FIRQ_CLEAR, 0);

		/* If we were using the timer, we would process the interrupt
		here... */
	}
	else
	{
		/* It is a DMD interrupt. */
#if (MACHINE_DMD == 1)
		dmd_rtt ();
#endif
	}
	firq_count++;

#ifdef __m6809__
	m6809_firq_restore_regs ();
#endif
}


__interrupt__
void do_nmi (void)
{
	fatal (ERR_NMI);
}


__interrupt__
void do_swi (void)
{
	fatal (ERR_SWI);
}


__interrupt__
void do_swi2 (void)
{
	fatal (ERR_SWI2);
}


__interrupt__
void do_swi3 (void)
{
	fatal (ERR_SWI3);
}

