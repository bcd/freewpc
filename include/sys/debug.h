/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>

#ifdef CONFIG_PLATFORM_WPC
#define WPC_DEBUG_WRITE_READY 0x1
#define WPC_DEBUG_READ_READY 0x2
#endif

extern inline U8 wpc_debug_get_status (void)
{
#ifdef CONFIG_PLATFORM_WPC
	return readb (WPC_DEBUG_CONTROL_PORT);
#endif
}

extern inline U8 wpc_debug_write_ready (void)
{
#ifdef CONFIG_PLATFORM_WPC
	return wpc_debug_get_status () & WPC_DEBUG_WRITE_READY;
#endif
}

extern inline U8 wpc_debug_read_ready (void)
{
#ifdef CONFIG_PLATFORM_WPC
	return wpc_debug_get_status () & WPC_DEBUG_READ_READY;
#endif
}

extern inline void wpc_debug_write (U8 data)
{
#ifdef CONFIG_PLATFORM_WPC
	writeb (WPC_DEBUG_DATA_PORT, data);
#endif
}

extern inline U8 wpc_debug_read (void)
{
#ifdef CONFIG_PLATFORM_WPC
	return readb (WPC_DEBUG_DATA_PORT);
#endif
}


enum wpc_debugger_request {
	WPC_DBREQ_TASK_DUMP=0x80,
};

enum wpc_debugger_response {
	WPC_DBRSP_CYCLECOUNT=0x80,
	WPC_DBRSP_STACK_POINTER,
	WPC_DBRSP_TASK_CREATE,
	WPC_DBRSP_IDLE_RUN,
};

__common__ void db_init (void);
__common__ void db_idle (void);
void db_puts (const char *s); /* moved to kernel for efficiency */

#endif /* _SYS_DEBUG_H */
