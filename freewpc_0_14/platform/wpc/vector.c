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

/*
 * \file
 * \brief The 6809 interrupt vector table
 *
 * This module defines the contents of the vector table and places the
 * structure at a fixed location, named "vector", which is mapped at
 * link-time to address 0xFFF0.
 */

extern void start (void);
extern void do_swi3 (void);
extern void do_swi2 (void);
extern void do_firq (void);
#ifdef STATIC_SCHEDULER
extern void tick_driver (void);
#else
extern void do_irq (void);
#endif
extern void do_swi (void);
extern void do_nmi (void);


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
	void (*reset) (void);
} m6809_vector_table_t;


/** The interrupt vector table */
__attribute__((section("vector"))) m6809_vector_table_t vectors = {
	.unused = start,
	.swi3 = do_swi3,
	.swi2 = do_swi2,
	.firq = do_firq,
#ifdef STATIC_SCHEDULER
	.irq = tick_driver,
#else
	.irq = do_irq,
#endif
	.swi = do_swi,
	.nmi = do_nmi,
	.reset = start,
};

