/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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


/**
 * Declare the file info table.  This object resides at a fixed location in
 * persistent memory.  It caches the addresses and sizes of the other persistent
 * objects from previous runs of the software.  If structure sizes or addresses
 * change, we can detect that and try to recover.  At the end of initialization,
 * this structure would be updated to reflect the current addresses/sizes in use.
 *
 * Each entry also holds the checksum for the structure, which was previously
 * declared as a separate object in each module.
 */
__dirtab__ struct file_info file_info[MAX_FILE_INFO];


/**
 * Return a pointer to the file info for a particular file type.
 */
struct file_info *file_find (enum file_type type)
{
	U8 i;
	struct file_info *fi;
	for (i=0, fi = file_info; i < MAX_FILE_INFO; i++, fi++)
	{
		/* If the entry appears corrupted, ignore its contents hereafter. */
		if (fi->type != FT_NONE &&
				(fi->type >= 0x40 ||
#ifdef __m6809__
				fi->data >= (void *)file_info ||
#endif
				fi->len > 0x200))
		{
			pinio_nvram_unlock ();
			fi->type = FT_NONE;
			pinio_nvram_lock ();
		}

		if (fi->type == type)
			return fi;
	}
	return NULL;
}


/* Create a new entry in the filesystem table. */
struct file_info *file_create (enum file_type type)
{
	/* Find a free slot.  Ensure that the file does not already exist.
	   Initialize it after creation. */
	struct file_info *fi = file_find (FT_NONE);
	if (!fi)
		dbprintf ("warning: could not file_create!\n");
	pinio_nvram_unlock ();
	fi->type = type;
	fi->attr = 0;
	fi->version = 0;
	pinio_nvram_lock ();
	return fi;
}


/**
 * Initialize the filesystem module at power up.
 */
void file_init (void)
{
	/* Give each module a chance to declare its nvram structures.  (This
	replaced the 'csum_area_check_all' function in earlier versions of the
	software. */
	callset_invoke (file_register);
}


/**
 * Reset the contents of all registered files.  This invalidates the
 * entire file table and then performs re-registration.
 */
void file_reset (void)
{
	U8 i;
	struct file_info *fi;
	pinio_nvram_unlock ();
	for (i=0, fi = file_info; i < MAX_FILE_INFO; i++, fi++)
		fi->type = FT_NONE;
	pinio_nvram_lock ();
	file_init ();
}


/**
 * Register a file used by the current build.  Each module that uses persistent
 * data should call this API during the 'file_register' event call.  This happens
 * before 'init'.  The purpose is to verify that previously saved values are sane
 * before we try to use them.
 */
void file_register (const struct area_csum *csi)
{
	struct file_info *fi;
	bool need_reset = FALSE;

	/* If type is FT_NONE, which is reserved, it probably means that csi was not
	updated to include a value for the type field, so it defaults to zero.  This
	indicates improper coding. */
	if (csi->type == FT_NONE)
	{
		dbprintf ("warning: csi %p has zero type\n", csi);
		return;
	}

	/* Find the file info in persistent storage.  If it doesn't exist, create one. */
	fi = file_find (csi->type);
	if (fi)
	{
		dbprintf ("file type %d: prev addr=%p len=%ld\n", csi->type, fi->data, fi->len);
	}
	else
	{
		fi = file_create (csi->type);
		dbprintf ("file type %d: new entry\n", csi->type);
	}

	/* Now check for changes to the structure from the previous version of the code.
	We are not checking values here, just the structure itself. */

	if (fi->version != csi->version)
	{
		/* If the version has changed, force reset.  Previous data will not be considered
		no matter what.  Developers bump the version when they change the structure in
		incompatible ways that absolutely require that previous data is discarded. */
		dbprintf ("new version %d\n", csi->version);
		need_reset = TRUE;
	}
	else if (fi->data != csi->area)
	{
		/* If the structure moved from the last power up, then ... */
		dbprintf ("new address %p\n", csi->area);
		need_reset = TRUE;
	}
	else if (csi->length != fi->len)
	{
		/* If only the structure size changed, then ... */
		dbprintf ("new length %p\n", csi->length);
		need_reset = TRUE;
	}
	else
	{
		/* Everything matches, the old data can be used as-is */
		dbprintf ("ok\n");
	}

	/* Save current structure info into the file table. */
	pinio_nvram_unlock ();
	fi->version = csi->version;
	fi->data = csi->area;
	fi->len = csi->length;
	pinio_nvram_lock ();

	if (need_reset)
		csum_area_reset (csi);
	else
		csum_area_check (csi);
}

