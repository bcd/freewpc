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

#ifndef _SYS_DEBUG_H
#define _SYS_DEBUG_H

/* Include ASIC address definitions */
#include <wpc.h>

extern inline U8 wpc_debug_get_status (void)
{
	return *(volatile U8 *)WPC_DEBUG_CONTROL_PORT;
}

extern inline U8 wpc_debug_write_ready (void)
{
	return wpc_debug_get_status () & 0x1;
}

extern inline U8 wpc_debug_read_ready (void)
{
	return wpc_debug_get_status () & 0x2;
}

extern inline void wpc_debug_write (U8 data)
{
	*(volatile U8 *)WPC_DEBUG_DATA_PORT = data;
}

extern inline U8 wpc_debug_read (void)
{
	return *(volatile U8 *)WPC_DEBUG_DATA_PORT;
}


#ifdef DEBUGGER
#define db_putc(b) wpc_debug_write(b)
void db_puts (const char *s);
void db_puti (U8 v);
void db_put2x (U8 v);
void db_put4x (U16 v);
#else
#define db_puts(s)
#define db_puti(i)
#define db_put2x(v)
#define db_put4x(v)
#define db_putc(b)
#endif

#define db_putp(p)	db_put4x ((U16)p)


enum wpc_debugger_request {
	WPC_DBREQ_TASK_DUMP=0x80,
};

enum wpc_debugger_response {
	WPC_DBRSP_CYCLECOUNT=0x80,
	WPC_DBRSP_STACK_POINTER,
	WPC_DBRSP_TASK_CREATE,
	WPC_DBRSP_IDLE_RUN,
};

void db_init (void);
void db_dump_switches (void);
void db_idle_task (void);

#endif /* _SYS_DEBUG_H */
