/*
 * Copyright 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
#include <simulation.h>

extern char *__start_nvram, *__stop_nvram;
extern char *__start_local, *__stop_local;

char protected_memory_file[256] = "freewpc.mem";


void protected_memory_load (void)
{
	int size = &__stop_nvram - &__start_nvram;
	FILE *fp;

	simlog (SLC_DEBUG, "Loading protected memory from '%s'", protected_memory_file);
	fp = fopen (protected_memory_file, "r");
	if (fp)
	{
		fread (&__start_nvram, 1, size, fp);
		fclose (fp);
	}
	else
	{
		simlog (SLC_DEBUG, "Error loading memory, using defaults\n");
		memset (&__start_nvram, 0, size);
	}
}


void protected_memory_save (void)
{
	int size = &__stop_nvram - &__start_nvram;
	FILE *fp;

	simlog (SLC_DEBUG, "Saving protected memory to %s", protected_memory_file);
	fp = fopen (protected_memory_file, "w");
	if (fp)
	{
		if (fwrite (&__start_nvram, 1, size, fp) < size)
		{
			simlog (SLC_DEBUG, "Warning: could not save all of memory\n");
			task_sleep_sec (1);
		}
		fclose (fp);
	}
	else
	{
		simlog (SLC_DEBUG, "Warning: could not write to memory file\n");
		task_sleep_sec (1);
	}
}


void local_save (void)
{
}


void local_restore (void)
{
}

