/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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
#include <asm-6809.h>

/**
 * \file
 * \brief Entry point to the game program.
 */

U8 errcode;

__fastram__ U8 irq_count;
__fastram__ U8 tick_count;

__nvram__ volatile U8 nvram_test_byte;

U8 sys_init_complete;
U8 sys_init_pending_tasks;



/*
 * fatal is the entry point for errors that are nonrecoverable.
 * error_code is one of the values in include/sys/errno.h.
 */
__noreturn__ 
void fatal (errcode_t error_code)
{
	disable_irq ();

	audit_increment (&system_audits.fatal_errors);
	
	dmd_alloc_low_clean ();

	dbprintf ("Fatal error: %i", error_code);

	sprintf ("ERRNO %i", error_code);
	font_render_string_center (&font_mono5, 64, 2, sprintf_buffer);

	sprintf ("GID %i", task_getgid ());
	font_render_string (&font_mono5, 64, 8, sprintf_buffer);

	dmd_show_low ();
	task_dump ();
	/* TODO : reset hardware here!! */
	for (;;);
}


void nonfatal (errcode_t error_code)
{
	audit_increment (&system_audits.non_fatal_errors);
}


void irq_init (void)
{
	irq_count = 0;
}


/** do_reset is the entry point to the program.  It all starts here. */
__attribute__((naked)) __noreturn__ 
void do_reset (void)
{
	register uint8_t *ramptr asm ("x");

	extern void test_init (void);
	extern void system_reset (void);

	/** Disable hardware interrupts in the 6809 */
	disable_interrupts ();

	/** Initialize the direct page pointer.  This hardware register
	 * determines where 'direct' addressing instructions are targeted.
	 * By setting to zero, direct addresses are mapped to 0000h-00FFh.
	 * We can use shorter instructions when referencing variables here.
	 */
	set_direct_page_pointer (0);

	/** Initialize the stack pointer.  We can now make
	 * function calls!  Note that this stack is only used
	 * for execution that is not task-based.  Once tasks
	 * can be run, each task will use its own stack pointer
	 * separate from this one.
	 *
	 * The initial stack pointer is shifted down from the
	 * available stack size, because do_reset() may
	 * use local variables, and will assume that it already
	 * has space allocated for them; the naked pragma prevents
	 * them from being allocated explicitly.
	 */
	set_stack_pointer (STACK_BASE - 8);

	/** Initializing the RAM page */
	wpc_set_ram_page (0);

	/** Perform basic diagnostics to ensure that everything is
	 * more or less working.
	 * 1. Verify ROM is good first, since that ensures that this
	 * code is not corrupted somehow.
	 * 2. Verify RAM next, using a read-write test.
	 * 3. Verify WPC ASIC functions.
	 * At any point, if something goes wrong, we go into a hard
	 * loop and pulse the diagnostic LED with a flash code to
	 * report the error.  We can't reply on the DMD working
	 * properly to help us here.
	 * TODO
	 */

	/** Initialize RAM to all zeroes */
	ramptr = (uint8_t *)USER_RAM_SIZE;
	do
	{
		*ramptr-- = 0;
	} while (ramptr != 0);

	/** Install the null pointer catcher, by programming
	 * an actual instruction at address 0x0 */
	*(U8 *)0 = 0x20;
	*(U8 *)1 = 0xFE;

	/** Set up protected RAM */
	wpc_set_ram_protect (RAM_UNLOCKED);
	wpc_set_ram_protect_size (RAM_LOCK_2K);
	wpc_set_ram_protect (RAM_LOCK_2K);

	/** Initialize the ROM page register 
	 * page of ROM adjacent to the system area is mapped.
	 * This is the default location for machine-specific files. */
	wpc_set_rom_page (MACHINE_PAGE);

	/** Initialize other critical WPC output registers relating
	 * to hardware */
	*(volatile U8 *)WPC_SOL_FLASH2_OUTPUT = 0;
	*(volatile U8 *)WPC_SOL_HIGHPOWER_OUTPUT = 0;
	*(volatile U8 *)WPC_SOL_FLASH1_OUTPUT = 0;
	*(volatile U8 *)WPC_SOL_LOWPOWER_OUTPUT = 0;
	*(volatile U8 *)WPC_LAMP_ROW_OUTPUT = 0;
	*(volatile U8 *)WPC_GI_TRIAC = 0;

	/** Set init complete flag to false.  When everything is
	 * ready, we'll change this to a 1. */
	sys_init_complete = 0;
	sys_init_pending_tasks = 0;

	/** Initialize all of the other kernel subsystems,
	 * starting with the hardware-centric ones and moving on
	 * to software features. */
	wpc_led_toggle ();
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
#if (MACHINE_DCS == 0)
	sound_init ();
#endif
	lamp_init ();
	device_init ();
	trough_init ();

	wpc_led_toggle ();
	irq_init ();

	/** task_init is somewhat special in that it transforms the system
	 * from a single task into a multitasking one.  After this, tasks
	 * can be spawned if need be.  A task is created for the current
	 * thread of execution, too. */
	task_init ();
	timer_init ();
	deff_init ();
	leff_init ();
	call_far (TEST_PAGE, test_init ());
	score_init ();
	coin_init ();
	adj_init ();
	audit_init ();
	high_score_init ();
	ball_search_init ();
	call_hook (init);

	/** Enable interrupts (IRQs and FIRQs) at the source (WPC) and
	 * in the 6809 */
	wpc_write_irq_clear (0x06);
	enable_interrupts ();

	/** The system is mostly usable at this point.
	 * Now, start the display effect that runs at powerup.
	 */

	task_create_gid (GID_SYSTEM_RESET, system_reset);

	/* Bump the power-up audit */
	audit_increment (&system_audits.power_ups);

	/* Initialize the sound board further */
#if (MACHINE_DCS == 1)
	sys_init_pending_tasks++;
	task_create_gid (GID_DCS_INIT, sound_init);
#endif

	/* Also run a probe on all of the ball devices, to see
	 * if they are working properly and empty out any
	 * balls in them that don't belong there yet. */
	sys_init_pending_tasks++;
	task_create_gid (GID_DEVICE_PROBE, device_probe);

	/* The system can run itself now, this task is done! */
	task_exit ();
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


void nvram_idle_task (void)
{
	U8 data = nvram_test_byte;
	++nvram_test_byte;
	if (data != nvram_test_byte)
	{
		fatal (ERR_NVRAM_UNLOCKED);
	}
}


/**
 * do_irq is the entry point from the IRQ vector.  Due to the
 * way the hardware works, the CPU will stop whatever it is doing
 * and jump to this location every 976 microseconds (1024 times
 * per second).  This function is used for time-critical operations
 * which won't necessarily get scheduled accurately from the
 * nonpreemptive tasks.
 *
 * You MUST keep processing in this function to the absolute
 * minimum, as it must be fast!
 */
__attribute__((interrupt))
void do_irq (void)
{
	/** Clear the source of the interrupt */
	wpc_write_irq_clear (0x96);

	irq_count++;
	if ((irq_count & 15) == 0)
		tick_count++;

	/* Execute rtts every 1ms */
	sol_rtt ();
	flipper_rtt ();
	if (irq_count & 0x1)
		lamp_rtt ();
	else
		switch_rtt ();
#ifdef MACHINE_1MS_RTTS
	MACHINE_1MS_RTTS
#endif

	if ((irq_count & 7) == 0)
	{
		/* Execute rtts every 8ms */
		ac_rtt ();
		triac_rtt ();
		flasher_rtt ();
#ifdef MACHINE_8MS_RTTS
		MACHINE_8MS_RTTS
#endif

		if ((irq_count & 31) == 0)
		{
			/* Execute rtts every 32ms */
			wpc_led_toggle ();
			sound_rtt ();
			lamp_flash_rtt ();
#ifdef MACHINE_32MS_RTTS
			MACHINE_32MS_RTTS
#endif

			if ((irq_count & 127) == 0)
			{
				/* Execute rtts every 128ms */
				lockup_check_rtt ();
#ifdef MACHINE_128MS_RTTS
				MACHINE_128MS_RTTS
#endif
			}
		}
	}
}


/**
 * do_firq is the entry point from the FIRQ vector.  This interrupt
 * is generated from the WPC ASIC on two different occasions: (1)
 * when the DMD controller has been programmed to generate an 
 * interrupt after drawing a particular scan line, and (2) when the
 * WPC's peripheral timer register reaches zero.  The type of interrupt
 * can be determined by reading the peripheral timer register.
 */
__attribute__((interrupt))
void do_firq (void)
{
	asm __volatile__ ("pshs\ta,b");

	if (*(U8 *)WPC_PERIPHERAL_TIMER_FIRQ_CLEAR & 0x80)
	{
		/* Timer interrupt */
		*(uint8_t *)WPC_PERIPHERAL_TIMER_FIRQ_CLEAR  = 0;
	}
	else
	{
		/* DMD interrupt */
		dmd_rtt ();
	}

	asm __volatile__ ("puls\ta,b");
}


__attribute__((interrupt))
void do_nmi (void)
{
	fatal (ERR_NMI);
}


__attribute__((interrupt))
void do_swi (void)
{
	fatal (ERR_SWI);
}


__attribute__((interrupt))
void do_swi2 (void)
{
	fatal (ERR_SWI2);
}


__attribute__((interrupt))
void do_swi3 (void)
{
	fatal (ERR_SWI3);
}


