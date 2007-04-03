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

/** The number of IRQ loops executed.  A loop consists of 8 IRQs. */
__fastram__ U8 irq_loop_count;

/** The number of task ticks executed.  A tick equals 16 IRQs. */
__fastram__ U8 tick_count;

/** The number of FIRQs asserted */
__fastram__ U8 firq_count;

/** An area of NVRAM used to test that it is kept locked. */
__nvram__ volatile U8 nvram_test_byte;

__fastram__ void (*irq_function) (void);

U8 sys_init_complete;

U8 sys_init_pending_tasks;

U8 last_nonfatal_error_code;

task_gid_t last_nonfatal_error_gid;

#ifdef CONFIG_PLATFORM_LINUX
char sprintf_buffer[PRINTF_BUFFER_SIZE];
#endif


__interrupt__ void do_irq0 (void);
__interrupt__ void do_irq1 (void);
__interrupt__ void do_irq2 (void);
__interrupt__ void do_irq3 (void);
__interrupt__ void do_irq4 (void);
__interrupt__ void do_irq5 (void);
__interrupt__ void do_irq6 (void);
__interrupt__ void do_irq7 (void);

void irq_init (void)
{
	irq_loop_count = 0;
	irq_function = do_irq0;
}


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

	timer_init ();
	deff_init ();
	leff_init ();
	test_init ();
	score_init ();
	coin_init ();
	adj_init ();
	high_score_init ();
	ball_search_init ();
	random_init ();
	game_init ();
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

#ifdef MACHINE_TEST_ONLY
	sys_init_complete++;
	test_enter_button ();
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
 * The lockup check routine examines 'task_dispatch_count', which
 * should normally be incrementing continually as normal task
 * scheduling occurs.  If this value stops moving, something
 * is very wrong.
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

	/* Audit the error.  TODO : log the error code also */
	audit_increment (&system_audits.fatal_errors);

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
	do_reset ();
#endif
}


void nonfatal_error_deff (void)
{
#ifdef DEBUGGER
	dmd_alloc_low_clean ();
	sprintf ("NONFATAL %ld", system_audits.non_fatal_errors);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	sprintf ("ERRNO %i GID %i", last_nonfatal_error_code, last_nonfatal_error_gid);
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
	dmd_show_low ();
	sound_send (SND_TEST_ALERT);
	task_sleep (TIME_200MS);
	sound_send (SND_TEST_ALERT);
	task_sleep (TIME_200MS);
	sound_send (SND_TEST_ALERT);
	task_sleep_sec (4);
#endif
	deff_exit ();
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


CALLSET_ENTRY (nvram, idle)
{
#ifndef GCC4
#ifndef CONFIG_PLATFORM_LINUX
	U8 data = nvram_test_byte;
	++nvram_test_byte;
	if (data != nvram_test_byte)
	{
		fatal (ERR_NVRAM_UNLOCKED);
	}
#endif
#endif
}


/** Realtime function that is called every 1ms */
static inline void do_irq_1ms (void)
{
	/* TODO - newer WPC roms clear WPC_RAM_BANK here */

	/* Clear the source of the interrupt */
	wpc_write_irq_clear (0x96);

	/* When building a profiling program, count the
	 * number of IRQs by writing to the pseudoregister
	 * WPC_PINMAME_CYCLE_COUNT every time we take an IRQ.
	 * Pinmame has been modified to understand this and
	 * keep track of how long we're spending in the IRQ
	 * based on these writes.  This should never be defined
	 * for real hardware. */
#ifdef IRQPROFILE
	wpc_asic_write (WPC_PINMAME_CYCLE_COUNT, 0);
#endif

#ifdef MACHINE_1MS_RTTS
	MACHINE_1MS_RTTS
#endif
}

static inline void do_irq_1ms_end (void)
{
	/* Again, for profiling, we mark the end of an IRQ
	 * by writing these markers. */
#ifdef IRQPROFILE
	wpc_debug_write (0xDD);
	wpc_debug_write (wpc_asic_read (WPC_PINMAME_CYCLE_COUNT));
#endif
}

static inline void do_irq_2ms_a (void)
{
	lamp_rtt ();
	/* Update flippers */
	flipper_rtt ();
}

static inline void do_irq_2ms_b (void)
{
	switch_rtt ();
#ifdef CONFIG_ZEROCROSS
	ac_rtt ();
#endif
}


/** Realtime function that is called every 8ms */
static inline void do_irq_8ms (void)
{
	/* Execute rtts every 8ms */
	sol_rtt ();
	triac_rtt ();
	flasher_rtt ();
#ifdef MACHINE_8MS_RTTS
	MACHINE_8MS_RTTS
#endif

	/* Increment total number of IRQ loops */
	irq_loop_count++;

	/* Every other 8ms (i.e. every 16ms), bump the tick count */
	if ((irq_loop_count & 1) == 0)
	{
		tick_count++;

		/* Every 4 loops, run the 32ms rtts */
		if ((irq_loop_count & 3) == 0)
		{
			wpc_led_toggle ();
			sound_rtt ();
			lamp_flash_rtt ();
#ifdef MACHINE_32MS_RTTS
			MACHINE_32MS_RTTS
#endif

			/* Every 16 loops, execute the 128ms rtts */
			if ((irq_loop_count & 15) == 0)
			{
#ifndef CONFIG_PLATFORM_LINUX
				lockup_check_rtt ();
#endif
				/* Execute machine-specific rtts */
#ifdef MACHINE_128MS_RTTS
				MACHINE_128MS_RTTS
#endif
			}
		}
	}
}

__interrupt__ void do_irq0 (void)
{
	do_irq_1ms ();
	do_irq_2ms_a ();
	irq_function = do_irq1;
	do_irq_1ms_end ();
}

__interrupt__ void do_irq1 (void)
{
	do_irq_1ms ();
	do_irq_2ms_b ();
	irq_function = do_irq2;
	do_irq_1ms_end ();
}

__interrupt__ void do_irq2 (void)
{
	do_irq_1ms ();
	do_irq_2ms_a ();
	irq_function = do_irq3;
	do_irq_1ms_end ();
}

__interrupt__ void do_irq3 (void)
{
	do_irq_1ms ();
	do_irq_2ms_b ();
	irq_function = do_irq4;
	do_irq_1ms_end ();
}

__interrupt__ void do_irq4 (void)
{
	do_irq_1ms ();
	do_irq_2ms_a ();
	irq_function = do_irq5;
	do_irq_1ms_end ();
}

__interrupt__ void do_irq5 (void)
{
	do_irq_1ms ();
	do_irq_2ms_b ();
	irq_function = do_irq6;
	do_irq_1ms_end ();
}

__interrupt__ void do_irq6 (void)
{
	do_irq_1ms ();
	do_irq_2ms_a ();
	irq_function = do_irq7;
	do_irq_1ms_end ();
}

__interrupt__ void do_irq7 (void)
{
	do_irq_1ms ();
	do_irq_2ms_b ();
	do_irq_8ms ();
	irq_function = do_irq0;
	do_irq_1ms_end ();
}

void do_irq (void)
{
	(*irq_function) ();
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


