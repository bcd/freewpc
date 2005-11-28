/*********************************************************************
 * FreeWPC - the pinball software alternative
 * Copyright (C) 2005 by Brian Dominy.  All Rights Reserved.
 *********************************************************************/

#include <freewpc.h>
#include <asm-6809.h>

uint8_t errcode;

__fastram__ uint8_t irq_count;
__fastram__ uint8_t tick_count;

uint8_t sys_init_complete;


__noreturn__ void do_fatal (errcode_t error_code)
{
	U8 *stack = (U8 *)get_stack_pointer () + 16;

	dmd_alloc_low_clean ();

	dbprintf ("Fatal error: %i", error_code);

	sprintf ("ERRNO %i", error_code);
	font_render_string_center (&font_5x5, 64, 2, sprintf_buffer);

	sprintf ("%02x", stack[0]);
	font_render_string (&font_5x5, 0*16, 10, sprintf_buffer);
	sprintf ("%02x", stack[1]);
	font_render_string (&font_5x5, 1*16, 10, sprintf_buffer);

	dmd_show_low ();
	task_dump ();
	for (;;);
}


void do_nonfatal (errcode_t error_code)
{
}


void irq_init (void)
{
	irq_count = 0;
}


/* do_reset is the entry point to the program.  It all starts here. */
#pragma naked
__noreturn__ void do_reset (void)
{
	register uint8_t *ramptr asm ("x");

	extern void lamp_demo ();
	extern void test_start (void);
	extern void test_init (void);
	extern void system_reset (void);

	/* Initialize the direct page pointer.  This hardware register
	 * determines where 'direct' addressing instructions are targeted.
	 * By setting to zero, direct addresses are mapped to 0000h-00FFh.
	 * We can use shorter instructions when referencing variables here.
	 */
	set_direct_page_pointer (0);

	/* Initialize the stack pointer.  We can now make
	 * function calls!  Note that this stack is only used
	 * for execution that is not task-based.  Once tasks
	 * can be run, each task will use its own stack pointer
	 * separate from this one. */
	set_stack_pointer (STACK_BASE);

	/* Initialize RAM to all zeroes */
	ramptr = (uint8_t *)USER_RAM_SIZE;
	do
	{
		*ramptr-- = 0;
	} while (ramptr != 0);

	/* Set up protected RAM */
	wpc_set_ram_protect (RAM_UNLOCKED);
	wpc_set_ram_protect_size (RAM_LOCK_512);
	wpc_set_ram_protect (RAM_LOCK_512);

	/* Initialize the ROM page register so that the
	 * page of ROM adjacent to the system area is mapped. */
	wpc_set_rom_page (61);

	sys_init_complete = 0;

	/* Initialize all of the other kernel subsystems,
	 * starting with the hardware-centric ones and moving on
	 * to software features. */
	wpc_led_toggle ();
#ifdef DEBUGGER
	db_init ();
#endif
	sol_init ();
	flasher_init ();
	triac_init ();
	dmd_init ();
	switch_init ();
	flipper_init ();
	sound_init ();
	lamp_init ();

	device_init ();
	trough_init ();

	wpc_led_toggle ();
	irq_init ();

	/* task_init is somewhat special in that it transforms the system
	 * from a single task into a multitasking one.  After this, tasks
	 * can be spawned if need be.  A task is created for the current
	 * thread of execution, too. */
	task_init ();
	deff_init ();
	leff_init ();
	test_init ();
	score_init ();
	adj_init ();
	call_hook (init);

	*(uint8_t *)WPC_ZEROCROSS_IRQ_CLEAR = 0x06;

	wpc_led_toggle ();

	/* The system is mostly usable at this point.
	 * Now, start the display effect that runs at powerup.
	 */
	task_create_gid (GID_SYSTEM_RESET, system_reset);

	/* Also run a probe on all of the ball devices, to see
	 * if they are working properly and empty out any
	 * balls in them that don't belong there yet. */
	task_create_gid (GID_DEVICE_PROBE, device_probe);

#if 0
	/* The system can run itself now, this task is done! */
	while (task_find_gid (GID_SYSTEM_RESET))
		task_sleep (TIME_100MS * 5);
#endif

	sys_init_complete++;
	task_exit ();
}


void lockup_check_rtt (void)
{
	/* FIXME : static does not do what you think in gcc 3.1.1 */
	static U8 last_idle_count = 0;

	if (last_idle_count != 0)
	{
		if (last_idle_count == task_idle_count)
		{
			do_reset ();
		}
	}
	last_idle_count = task_idle_count;
}


/*
 * do_irq is the entry point from the IRQ vector.  Due to the
 * way the hardware works, the CPU will stop whatever it is doing
 * and jump to this location every 976 microseconds (1024 times
 * per second).  This function is used for time-critical operations
 * which won't necessarily get scheduled accurately from the
 * nonpreemptive tasks.
 */
#pragma interrupt
void do_irq (void)
{
	*(uint8_t *)WPC_ZEROCROSS_IRQ_CLEAR = 0x96;

	/* Execute rtts every 1ms */
#ifdef DEBUGGER
	db_rtt ();
#endif
	irq_count++;
	sol_rtt ();
	flipper_rtt ();
	if (irq_count & 0x1)
		lamp_rtt ();
	else
		switch_rtt ();

	if ((irq_count & 7) == 0)
	{
		/* Execute rtts every 8ms */
		tick_count++;
		triac_rtt ();
		flasher_rtt ();

		if ((tick_count & 3) == 0) /* 4 x 8ms */
		{
			/* Execute rtts every 32ms */
			wpc_led_toggle ();
			sound_rtt ();
			lamp_flash_rtt ();
			/// lockup_check_rtt (); /* disabled for now */

			extern void tz_clock_rtt (void);
			tz_clock_rtt ();
		}
	}
}


/*
 * do_firq is the entry point from the FIRQ vector.  This interrupt
 * is generated from the WPC ASIC on two different occasions: (1)
 * when the DMD controller has been programmed to generate an 
 * interrupt after drawing a particular scan line, and (2) when the
 * WPC's peripheral timer register reaches zero.  The type of interrupt
 * can be determined by reading the peripheral timer register.
 */
#pragma interrupt
void do_firq (void)
{
	if (*(int8_t *)WPC_PERIPHERAL_TIMER_FIRQ_CLEAR < 0)
	{
		/* Timer interrupt */
	}
	else
	{
		/* DMD interrupt */
		dmd_rtt ();
	}

	*(uint8_t *)WPC_PERIPHERAL_TIMER_FIRQ_CLEAR  = 0;
}


#pragma interrupt
void do_nmi (void)
{
	fatal (ERR_NMI);
}


#pragma interrupt
void do_swi (void)
{
	fatal (ERR_SWI);
}


#pragma interrupt
void do_swi2 (void)
{
	fatal (ERR_SWI2);
}


#pragma interrupt
void do_swi3 (void)
{
	fatal (ERR_SWI3);
}


