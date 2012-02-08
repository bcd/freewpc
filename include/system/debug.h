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

#ifndef _SYS_DEBUG_H
#define _SYS_DEBUG_H

#ifdef CONFIG_BPT
extern U8 db_paused;
#endif

__common__ void db_init (void);
__common__ void db_dump_all (void);
__common__ void db_periodic (void);

extern void (*puts_handler) (const char *s);
void puts_debug (const char *s);
void puts_parallel (const char *s);
void puts_sim (const char *s);


/**
 * CONFIG_BPT is used to turn on the embedded debugger.
 * It depends on 6809 assembly functions and thus cannot be used
 * in native mode.  Plus, you already have gdb there anyway.
 * So don't allow it.
 */
#ifdef CONFIG_NATIVE
#undef CONFIG_BPT
#endif


/**
 * When CONFIG_BPT is set, the breakpoint APIs are available.
 * Otherwise they are all no-ops.
 * Also when turned on, FREE_ONLY is implied, because the
 * escape button is hijacked for debugger use, preventing it
 * from being used to add credits.
 */
#ifdef CONFIG_BPT
#define bpt()  asm ("jsr\t*bpt_handler")
void bpt_init (void);
void bpt_clear (void);
void bpt_stop (void);
void bpt_set (void *addr, U8 page);
extern U8 bpt_addr[];
#undef FREE_ONLY
#define FREE_ONLY
#else
#define bpt()
#define bpt_init()
#define bpt_clear()
#define bpt_set(addr, page)
#endif

#endif /* _SYS_DEBUG_H */
