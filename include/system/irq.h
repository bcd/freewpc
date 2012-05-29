/*
 * Copyright 2006-2012 by Brian Dominy <brian@oddchange.com>
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

/** How to enable/disable the IRQ */
#define disable_irq() m6809_orcc (CC_IRQ)
#define enable_irq() m6809_andcc (~CC_IRQ)

/** How to enable/disable the FIRQ */
#define disable_firq() m6809_orcc (CC_FIRQ)
#define enable_firq() m6809_andcc (~CC_FIRQ)

#ifdef CONFIG_PLATFORM_WPC
#define rtt_disable() do { disable_irq(); disable_firq(); } while (0)
#define rtt_enable()  do { enable_irq(); enable_firq(); } while (0)
#else
#ifdef CONFIG_PERIODIC_IRQ
#define rtt_disable() disable_irq()
#define rtt_enable() enable_irq()
#endif
#ifdef CONFIG_PERIODIC_FIRQ
#define rtt_disable() disable_firq()
#define rtt_enable() enable_firq()
#endif
#endif

#endif /* __m6809__ */

#ifdef CONFIG_NATIVE

extern bool linux_irq_enable;
extern bool linux_firq_enable;

#define disable_irq()	linux_irq_enable = FALSE;
#define disable_firq()	linux_firq_enable = FALSE;

#define enable_irq()		linux_irq_enable = TRUE;
#define enable_firq()	linux_firq_enable = TRUE;

#define rtt_disable() disable_irq()
#define rtt_enable() enable_irq()

#endif /* CONFIG_NATIVE */

/* For compatibility with the older names used */
#define disable_interrupts() rtt_disable()
#define enable_interrupts() rtt_enable()

#endif /* _SYS_IRQ_H */
