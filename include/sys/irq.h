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

#ifndef _SYS_IRQ_H
#define _SYS_IRQ_H

#ifdef __m6809__

/** GCC4.1 does not compile the new style 'ccreg' instructions
 * correctly, so we revert back to the old way of using
 * hand-coded assembly. */
#ifdef GCC4
#define NO_CC_REG
#endif

/** How to enable/disable the IRQ */
#ifdef NO_CC_REG
#define disable_irq()	asm ("orcc\t#" C_STRING(CC_IRQ))
#define enable_irq()		asm ("andcc\t#" C_STRING(~CC_IRQ))
#else
#define disable_irq()	cc_reg |= CC_IRQ
#define enable_irq()	cc_reg &= ~CC_IRQ
#endif

/** How to enable/disable the FIRQ */
#ifdef NO_CC_REG
#define disable_firq()	asm ("orcc\t#" C_STRING(CC_FIRQ))
#define enable_firq()	asm ("andcc\t#" C_STRING(~CC_FIRQ))
#else
#define disable_firq()	cc_reg |= CC_FIRQ
#define enable_firq()	cc_reg &= ~CC_FIRQ
#endif

#else /* __m6809__ */

#define disable_irq()
#define disable_firq()
#define enable_irq()
#define enable_firq()

#endif /* __m6809__ */

/** How to enable/disable all interrupts */
#ifdef NO_CC_REG
#define disable_interrupts()	asm ("orcc\t#" C_STRING(CC_IRQ|CC_FIRQ))
#define enable_interrupts()	asm ("andcc\t#" C_STRING(~(CC_IRQ|CC_FIRQ)))
#else
//#define disable_interrupts()	cc_reg |= (CC_IRQ + CC_FIRQ)
//#define enable_interrupts()	cc_reg &= ~(CC_IRQ + CC_FIRQ)
#define disable_interrupts()	do { disable_irq(); disable_firq(); } while (0)
#define enable_interrupts()	do { enable_irq(); enable_firq(); } while (0)
#endif

#endif /* _SYS_IRQ_H */
