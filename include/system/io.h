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

#ifndef __SYSTEM_IO_H
#define __SYSTEM_IO_H

/* Default I/O accessor functions writeb() and readb().
 *
 * On memory-mapped I/O platforms, define CONFIG_MMIO and use the memory-
 * mapped address as your I/O address (IOPTR).  Then readb() and writeb()
 * will work for both the target platform, and for the simulator.
 *
 * On other platforms, you should test CONFIG_SIM in your platform header
 * and only call readb/writeb in simulation.  Then it is up to you to
 * decide how to do the I/O on a real target.  If you don't mind function
 * call overhead, you can also just set CONFIG_CALLIO and then define
 * your own implementation of readb/writeb to do the right thing.
 */

#ifdef CONFIG_MMIO
extern inline void writeb (IOPTR addr, U8 val)
{
	*(volatile U8 *)addr = val;
	barrier ();
}
#elif defined(CONFIG_CALLIO) || defined(CONFIG_NATIVE)
void writeb (IOPTR addr, U8 val);
#else
#error
#endif

extern inline void writew (IOPTR addr, U16 val)
{
#ifdef CONFIG_MMIO
	*(volatile U16 *)addr = val;
	barrier ();
#else
	writeb (addr, val >> 8);
	writeb (addr+1, val & 0xFF);
#endif
}


#ifdef CONFIG_MMIO
extern inline U8 readb (IOPTR addr)
{
	return *(volatile U8 *)addr;
}
#elif defined(CONFIG_CALLIO) || defined(CONFIG_NATIVE)
U8 readb (IOPTR addr);
#else
#error
#endif

#endif /* __SYSTEM_IO_H */
