/*
 * Copyright 2007-2011 by Brian Dominy <brian@oddchange.com>
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
#undef sprintf
#include <native/log.h>

/**
 * \file
 * \brief Implements protected memory for native mode.
 *
 * Protected memory variables can be detected because they reside in a special
 * section of the output file (in much the same way that the 6809 compile does
 * it).  The entire block of RAM can be read from/written to a file to
 * provide persistence.
 */

/** The name of the backing file */
char protected_memory_file[256] = "nvram/default.nv";


/** Load the contents of the protected memory from file to RAM. */
void protected_memory_load (void)
{
	int size = AREA_SIZE(nvram);
	FILE *fp;

	/* Use a different file for each machine */
	sprintf (protected_memory_file, "nvram/%s.nv", MACHINE_SHORTNAME);

	print_log ("Loading protected memory from '%s'\n", protected_memory_file);
	fp = fopen (protected_memory_file, "r");
	if (fp)
	{
		fread (AREA_BASE(nvram), 1, size, fp);
		fclose (fp);
	}
	else
	{
		print_log ("Error loading memory, using defaults\n");
		memset (AREA_BASE(nvram), 0, size);
	}
}


/** Save the contents of the protected memory from RAM to a file. */
void protected_memory_save (void)
{
	int size = AREA_SIZE(nvram);
	FILE *fp;

	print_log ("Saving 0x%X bytes of protected memory to %s\n", size, protected_memory_file);
	fp = fopen (protected_memory_file, "w");
	if (fp)
	{
		if (fwrite (AREA_BASE(nvram), 1, size, fp) < size)
		{
			print_log ("Warning: could not save all of memory\n");
			task_sleep_sec (1);
		}
		fclose (fp);
	}
	else
	{
		print_log ("Warning: could not write to memory file\n");
		task_sleep_sec (1);
	}
}


