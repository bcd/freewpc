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
#include <test.h>

/**
 * \file
 * \brief Entry point to the game program.
 */

/** The current system time as the number of IRQs/960 microsecond intervals */
__fastram__ U16 sys_time;

/** Nonzero when the system has finished initializing */
U8 sys_init_complete;

/** Nonzero when it is OK to call periodic functions */
U8 periodic_ok;

/** The number of background initialization tasks that are still
running.  The splash screen is kept until this reverts to zero. */
U8 sys_init_pending_tasks;

/* RTT(name=advance_time_rtt freq=16 inline=1) */

/**
 * Handle a warm reboot.
 *
 * Called when the system wants to start over completely.
 * Previously, we just called freewpc_init(), but this is not entirely
 * correct.  Interrupts need to be disabled during re-init, and we can
 * set flags here to distinguish between cold/warm boot.
 */
__noreturn__ void warm_reboot (void)
{
#ifdef __m6809__
	start ();
#else
	disable_interrupts ();
	freewpc_init ();
#endif
}


/** Initialize the FreeWPC program. */
__noreturn__ void freewpc_init (void)
{
	extern __common__ void system_reset (void);

	/* Initialize the platform specifics first */
	VOIDCALL (platform_init);

	/* Reset the blanking and watchdog circuitry.
	 * Eventually, the watchdog will be tickled every 1ms
	 * by the IRQ; until interrupts are enabled, we will
	 * have to do this periodically ourselves. */
	pinio_watchdog_reset ();

	/* Set init complete flag to false.  When everything is
	 * ready, we'll change this to a 1. */
	sys_init_complete = 0;
	periodic_ok = 0;
	sys_init_pending_tasks = 0;

	/* Initialize all of the other kernel subsystems,
	 * starting with the hardware-centric ones and moving on
	 * to software features. */

	/* Initialize the real-time scheduler.  The periodic functions
	are scheduled at compile-time  using the 'gensched' utility. */
#ifdef CONFIG_GEN_RTT
	VOIDCALL (rtt_init);
#else
	VOIDCALL (tick_init);
#endif

	/* Initialize the hardware.
	 * After each init call, tickle the watchdog (IRQ isn't enabled yet)
	 * to prevent it from expiring and resetting the CPU.
	 * We don't use a callset here because there are some ordering
	 * dependencies -- some modules must be initialized before others --
	 * and gencallset doesn't allow us to express those conditions.
	 */
#ifdef DEBUGGER
	db_init ();
	bpt_init ();
	pinio_watchdog_reset ();
#endif
#ifdef CONFIG_AC
	ac_init ();
	pinio_watchdog_reset ();
#endif
	sol_init ();
	pinio_watchdog_reset ();
#ifdef CONFIG_GI
	gi_init ();
	pinio_watchdog_reset ();
#endif
#ifdef CONFIG_DMD_OR_ALPHA
	display_init ();
	pinio_watchdog_reset ();
#endif
	switch_init ();
	pinio_watchdog_reset ();
	flipper_init ();
	pinio_watchdog_reset ();
	lamp_init ();
	pinio_watchdog_reset ();
	device_init ();
	pinio_watchdog_reset ();
	free_timer_init ();
	pinio_watchdog_reset ();
	sound_init ();
	pinio_watchdog_reset ();
#if (MACHINE_PIC == 1)
	pic_init ();
	pinio_watchdog_reset ();
#endif

	/* task_init is somewhat special in that it transforms the system
	 * from a single task into a multitasking one.  After this, tasks
	 * can be spawned if need be.  A task is created for the current
	 * thread of execution, too. */
	task_init ();
	pinio_watchdog_reset ();

	/* log_init needs to run early, before any callset_invokes are
	   made. */
	log_init ();

#ifdef CONFIG_NATIVE
	{
		void realtime_loop (void);

		/* This is done here, because the task subsystem isn't ready
		inside main () */
		task_create_gid_while (GID_LINUX_REALTIME, realtime_loop,
			TASK_DURATION_INF);
	}
#endif
#ifdef CONFIG_SIM
	/* Notify the simulator when the core OS is up and running. */
	sim_init ();
#endif

	/* Initialize the sound board early in a background
	 * thread, since it involves polling for data back from it,
	 * which may take unknown (or even infinite) time. */
	sys_init_pending_tasks++;
	task_create_gid (GID_SOUND_INIT, sound_board_init);

	/* Enable interrupts (IRQs and FIRQs).  Do this as soon as possible,
	 * but not before all of the hardware modules are done. */
	enable_interrupts ();

	/* Check all adjustments and make sure that their checksums are valid.
	If problems are found, those adjustments will be made sane again. */
	file_init ();

	/* Initialize everything else.  Some of these are given explicitly
	to force a particular order, since callsets do not guarantee the
	order of invocation.  For most things the order doesn't matter. */
	deff_init ();
	leff_init ();
#ifdef CONFIG_TEST
	test_init ();
#endif
	adj_init ();
	callset_invoke (init);

	/* Enable periodic processing. */
	periodic_ok = 1;
	task_sleep (TIME_16MS);

	/* The system is initialized from a hardware perspective.
	 * Now, perform additional final initializations. */
	system_reset ();

	/* The system can run itself now, this task is done! */
	task_exit ();
}


U8 get_elapsed_time (U16 then)
{
	return get_sys_time () - then;
}



