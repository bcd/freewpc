/*
 * Copyright 2009, 2010 by Brian Dominy <brian@oddchange.com>
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

__nvram__ U16 swinfo_permanent_size;
__nvram__ U16 swinfo_nvram_size;
__nvram__ U8 swinfo_system_major;
__nvram__ U8 swinfo_system_minor;
__nvram__ U8 swinfo_machine_major;
__nvram__ U8 swinfo_machine_minor;

#define swinfo_verify1(dst, src) ((dst) == (src))


/**
 * Save the current software settings into persistent & protected memory.
 */
CALLSET_ENTRY (swinfo, factory_reset)
{
	pinio_nvram_unlock ();
	swinfo_permanent_size = AREA_SIZE(permanent);
	swinfo_nvram_size = AREA_SIZE(nvram);
	swinfo_system_major = FREEWPC_MAJOR_VERSION;
	swinfo_system_minor = FREEWPC_MINOR_VERSION;
	swinfo_machine_major = MACHINE_MAJOR_VERSION;
	swinfo_machine_minor = MACHINE_MINOR_VERSION;
	pinio_nvram_lock ();
}


/**
 * Verify that nothing has changed significantly since the last reboot.
 * In particular, major software revision changes and structure sizes
 * are checked.
 */
CALLSET_BOOL_ENTRY (swinfo, init_ok)
{
	if (swinfo_verify1 (swinfo_permanent_size, AREA_SIZE(permanent))
		&& swinfo_verify1 (swinfo_nvram_size, AREA_SIZE(nvram))
		&& swinfo_verify1 (swinfo_system_major, FREEWPC_MAJOR_VERSION)
		&& swinfo_verify1 (swinfo_system_minor, FREEWPC_MINOR_VERSION)
		&& swinfo_verify1 (swinfo_machine_major, MACHINE_MAJOR_VERSION)
		&& swinfo_verify1 (swinfo_machine_minor, MACHINE_MINOR_VERSION)
		)
	{
		/* Everything matches */
		return TRUE;
	}
	else
	{
		/* Something changed ... trigger factory reset */
		dbprintf ("S/W incompatible with previous config");
		return FALSE;
	}
}

