/*
 * Copyright 2009-2011 by Brian Dominy <brian@oddchange.com>
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

__nvram__ struct software_info
{
	U16 permanent_size;
	U16 nvram_size;
	U8 sys_major;
	U8 sys_minor;
	U8 mach_major;
	U8 mach_minor;
} software_info;


#define swinfo_verify1(dst, src) ((dst) == (src))


/**
 * Save the current software settings into persistent & protected memory.
 */
void swinfo_reset (void)
{
	dbprintf ("resetting SW versions\n");
	software_info.permanent_size = AREA_SIZE(permanent);
	software_info.nvram_size = AREA_SIZE(nvram);
	software_info.sys_major = FREEWPC_MAJOR_VERSION;
	software_info.sys_minor = FREEWPC_MINOR_VERSION;
	software_info.mach_major = MACHINE_MAJOR_VERSION;
	software_info.mach_minor = MACHINE_MINOR_VERSION;
}


/**
 * Verify that software versions and the total size of the saved RAM
 * has not changed since the last power up.
 *
 * This check is called _after_ individual areas of the persistent memory
 * are verified.  It intends to catch global problems not isolated to a
 * particular module.  When this fails, a factory reset is forced.  This
 * is more forceful than simply reinitializing each area.
 */
CALLSET_BOOL_ENTRY (swinfo, init_ok)
{
	if (swinfo_verify1 (software_info.permanent_size, AREA_SIZE(permanent))
		&& swinfo_verify1 (software_info.nvram_size, AREA_SIZE(nvram))
		&& swinfo_verify1 (software_info.sys_major, FREEWPC_MAJOR_VERSION)
		&& swinfo_verify1 (software_info.sys_minor, FREEWPC_MINOR_VERSION)
		&& swinfo_verify1 (software_info.mach_major, MACHINE_MAJOR_VERSION)
		&& swinfo_verify1 (software_info.mach_minor, MACHINE_MINOR_VERSION)
		)
	{
		/* Everything matches */
		return TRUE;
	}
	else
	{
		/* Something changed ... trigger factory reset */
		dbprintf ("S/W incompatible with previous config\n");
		return FALSE;
	}
}

const struct area_csum software_csum_info = {
	.type = FT_VERSION,
	.version = 1,
	.area = (U8 *)&software_info,
	.length = sizeof (software_info),
	.reset = swinfo_reset
};


CALLSET_ENTRY (swinfo, file_register)
{
	file_register (&software_csum_info);
}

