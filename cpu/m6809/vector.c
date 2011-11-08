/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief The 6809 interrupt vector table
 *
 * This module defines the contents of the vector table and places the
 * structure at a fixed location, named "vector", which is mapped at
 * link-time to address 0xFFF0.
 */

extern __attribute__((noreturn)) void start (void);

#ifdef CONFIG_PLATFORM_WPC
extern void do_swi3 (void);
extern void do_swi2 (void);
extern void do_firq (void);
extern void tick_driver (void);
extern void do_swi (void);
extern void do_nmi (void);
#endif

/** The 6809 vector table structure */
typedef struct
{
	void (*unused) (void);
	void (*swi3) (void);
	void (*swi2) (void);
	void (*firq) (void);
	void (*irq) (void);
	void (*swi) (void);
	void (*nmi) (void);
	__attribute__((noreturn)) void (*reset) (void);
} m6809_vector_table_t;


/** The interrupt vector table.  Each platform may define the
 * callbacks differently, except for the reset vector which
 * always maps to the start function. */
__attribute__((section("vector"))) m6809_vector_table_t vectors = {
#ifdef CONFIG_PLATFORM_WPC
	/* The unused vector should never occur.  It is treated like hard reset. */
	.unused = start,

	/* The SWI, SWI2, and SWI3 interrupts occur when instructions generate
	 * them.  FreeWPC doesn't use them, but an invalid branch into data could
	 * possibly generate one.  So catch them, log the error, and then reset
	 * the CPU again. */
	.swi3 = do_swi3,
	.swi2 = do_swi2,

	/* The FIRQ vector is connected to the ASIC timer and to the DMD controller. */
	.firq = do_firq,

	/* The IRQ vector is connected to the periodic (~1ms) interrupt.
	 * This interrupt is the basis for system timing and realtime processing.
	 * The function 'tick_driver' is automatically generated from a schedule
	 * of functions to call.  See build/sched-irq.c for the results and
	 * kernel/system.sched for the tasks common to all machines. */
	.irq = tick_driver,
	.swi = do_swi,

	/* NMI also shouldn't happen, but log it if it does. */
	.nmi = do_nmi,
#endif
	.reset = start,
};

