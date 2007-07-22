/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Entry point to the game program.
 */

U8 errcode;

/** The number of FIRQs asserted */
__fastram__ U8 firq_count;

/** An area of NVRAM used to test that it is kept locked. */
__nvram__ U8 nvram_test_byte;

U8 sys_init_complete;

U8 idle_ok;

U8 sys_init_pending_tasks;

U8 last_nonfatal_error_code;

task_gid_t last_nonfatal_error_gid;

#ifdef CONFIG_PLATFORM_LINUX
char sprintf_buffer[PRINTF_BUFFER_SIZE];
#endif


#ifdef __m6809__
void exit (void)
{
}

void abort (void)
{
	fatal (ERR_LIBC_ABORT);
}

#endif

/** do_reset is the beginning of the C code.
 * When building for the 6809, control transfers here from the
 * assembly bootup code in start.s.
 * Under Linux emulation, this function is called from main().
 */
__naked__ __noreturn__ 
void do_reset (void)
{
	extern void system_reset (void);

#ifdef __m6809__
	/* Reset the sound board... the earlier the better */
	wpc_asic_write (WPCS_CONTROL_STATUS, 0);

	/* Initializing the RAM page */
	wpc_set_ram_page (0);

	/* Install the null pointer catcher, by programming
	 * an actual instruction at address 0x0 (branch to self)
	 * TODO : disable interrupts, too? */
	*(U8 *)0 = 0x20;
	*(U8 *)1 = 0xFE;
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

	/* Set init complete flag to false.  When everything is
	 * ready, we'll change this to a 1. */
	sys_init_complete = 0;
	idle_ok = 0;
	sys_init_pending_tasks = 0;

	/* Initialize all of the other kernel subsystems,
	 * starting with the hardware-centric ones and moving on
	 * to software features. */
	wpc_led_toggle ();
	irq_init ();
#ifdef DEBUGGER
	db_init ();
#endif
	ac_init ();
	sol_init ();
	flasher_init ();
	triac_init ();
	dmd_init ();
	switch_init ();
	flipper_init ();
	lamp_init ();
	device_init ();

	wpc_led_toggle ();

	/* task_init is somewhat special in that it transforms the system
	 * from a single task into a multitasking one.  After this, tasks
	 * can be spawned if need be.  A task is created for the current
	 * thread of execution, too. */
	task_init ();

	/* Initialize the sound board first.  This is started as a background
	 * thread, since it involves polling for data back from the sound board,
	 * which may take unknown (or even infinite) time. */
	sys_init_pending_tasks++;
	task_create_gid (GID_DCS_INIT, sound_init);

	deff_init ();
	leff_init ();
	test_init ();
	adj_init ();
	callset_invoke (init);

	csum_area_check_all ();

	/* Enable interrupts (IRQs and FIRQs) at the source (WPC) and
	 * in the 6809 */
#ifdef CONFIG_PLATFORM_LINUX
	linux_init ();
#endif
	wpc_write_irq_clear (0x06);
	enable_interrupts ();

	/* The system is mostly usable at this point.
	 * Now, start the display effect that runs at powerup.
	 */
	idle_ok = 1;
	//db_idle ();

#ifdef MACHINE_TEST_ONLY
	sys_init_complete++;
	callset_invoke (sw_enter);
#else
	task_create_gid (GID_SYSTEM_RESET, system_reset);

	/* Bump the power-up audit */
	audit_increment (&system_audits.power_ups);
#endif

	/* The system can run itself now, this task is done!
	 *
	 * In the simulator, the main task is not supposed to exit, so there
	 * is a conditional for that.
	 */
#ifndef CONFIG_PLATFORM_LINUX
	task_exit ();
#else
	while (1)
	{
		/* TODO - drop priority for idle tasks */
		task_sleep (TIME_66MS);
		callset_invoke (idle);
	}
#endif
}


/**
 * The lockup check routine examines 'task_dispatch_ok', which
 * should normally be true as normal task scheduling occurs.  
 * If this value stays false, something is very wrong.
 *
 * This check occurs every 128 IRQs.  No task should run for
 * that long without giving up control.  If the count doesn't
 * change on every check, we invoke a fatal error and reset.
 *
 * NOTE: if a task _really_ does take that long to execute before
 * switching out, it should set "task_dispatching_ok = TRUE"
 * periodically, to avoid a time out here.  This should rarely be
 * used.  (Even better, it should "task_yield" and let other tasks
 * run for a while.)
 */
void lockup_check_rtt (void)
{
	if (sys_init_complete && !task_dispatching_ok)
	{
		fatal (ERR_FCFS_LOCKUP);
	}
	else
	{
		task_dispatching_ok = FALSE;
	}
}


/**
 * Entry point for errors that are nonrecoverable.
 * error_code is one of the values in include/sys/errno.h.
 */
__noreturn__ 
void fatal (errcode_t error_code)
{
	/* Don't allow any more interrupts, since they might be the
	source of the error. */
	disable_irq ();

	/* Reset hardware outputs */
	wpc_write_flippers (0);
	wpc_write_ticket (0);
	wpc_asic_write (WPC_SOL_HIGHPOWER_OUTPUT, 0);
	wpc_asic_write (WPC_SOL_LOWPOWER_OUTPUT, 0);
	wpc_asic_write (WPC_SOL_FLASH1_OUTPUT, 0);
	wpc_asic_write (WPC_SOL_FLASH2_OUTPUT, 0);
#ifdef MACHINE_SOL_EXTBOARD1
	wpc_asic_write (WPC_EXTBOARD1, 0);
#endif
	wpc_asic_write (WPC_GI_TRIAC, 0);

	/* Audit the error. */
	audit_increment (&system_audits.fatal_errors);
	audit_assign (&system_audits.lockup1_addr, error_code);
	audit_assign (&system_audits.lockup1_pid_lef, task_getgid ());

	/* Try to display the error on the DMD.  This may not work,
	you know. */
	dmd_alloc_low_clean ();

	dbprintf ("Fatal error: %i\n", error_code);

	sprintf ("ERRNO %i", error_code);
	font_render_string_center (&font_mono5, 64, 2, sprintf_buffer);

	sprintf ("GID %i", task_getgid ());
	font_render_string (&font_mono5, 64, 8, sprintf_buffer);

	dmd_show_low ();

	/* Dump all of the task information to the debugger port. */
	task_dump ();

#ifdef CONFIG_PLATFORM_LINUX
	exit (1);
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
				asm ("nop"); 
				asm ("nop");
				asm ("nop");
				asm ("nop");
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
	do_reset ();
#endif
#endif
}


#ifndef CONFIG_PLATFORM_LINUX
S16 main (void)
{
	return 0;
}
#endif

void nonfatal (errcode_t error_code)
{
	audit_increment (&system_audits.non_fatal_errors);
#ifdef DEBUGGER
	last_nonfatal_error_code = error_code;
	last_nonfatal_error_gid = task_getgid ();
	deff_start (DEFF_NONFATAL_ERROR);
#endif
}


CALLSET_ENTRY (nvram, idle)
{
#ifdef HAVE_NVRAM
	U8 data = nvram_test_byte;
	++nvram_test_byte;
	asm ("; nop" ::: "memory");
	if (data != nvram_test_byte)
	{
		fatal (ERR_NVRAM_UNLOCKED);
	}
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
#ifdef __m6809__
	asm __volatile__ ("pshs\td,x");
#endif

	if (wpc_asic_read (WPC_PERIPHERAL_TIMER_FIRQ_CLEAR) & 0x80)
	{
		/* Timer interrupt */
		wpc_asic_write (WPC_PERIPHERAL_TIMER_FIRQ_CLEAR, 0);
	}
	else
	{
		/* DMD interrupt */
		dmd_rtt ();
		firq_count++;
	}

#ifdef __m6809__
	asm __volatile__ ("puls\td,x");
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

