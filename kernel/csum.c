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

/**
 * \file
 * \brief Generic checksum calculation/verification routines.
 *
 * Each module that uses protected memory should declare a structure of
 * type "struct area_csum" that says how that memory should be
 * managed.  Two APIs are exposed per region: one to update the
 * checksum (after the data is changed through proper means), and one
 * to verify the area.  If the checksum does not match, the
 * structure provides a callback function that says how to reset the
 * data to sane values.
 */

#include <freewpc.h>


/**
 * Updates a checksummed region after an update.
 * This should be invoked immediately after any changes to protected
 * memory.
 */
void
csum_area_update (const struct area_csum *csi)
{
	U8 csum;
	U8 *ptr;

	/* Compute the current checksum of the area */
	csum = 0;
	for (ptr = csi->area; ptr < csi->area + csi->length; ptr++)
		csum += *ptr;

	/* Store this as the new checksum */
	pinio_nvram_unlock ();
	*(csi->csum) = csum;
	pinio_nvram_lock ();
}


/**
 * Checks a checksummed region for correctness.
 * If the checksum fails, the data is reset to defaults.
 */
void
csum_area_check (const struct area_csum *csi)
{
	U8 csum;
	U8 *ptr;

	/* Compute the current checksum of the area */
	csum = 0;
	for (ptr = csi->area; ptr < csi->area + csi->length; ptr++)
		csum += *ptr;

	/* Compare against the stored checksum */
	if (csum != *(csi->csum))
	{
		/* If different, reset area to default values.
		 * Enable write access prior to calling this. */
		pinio_nvram_unlock ();
		csi->reset ();
		pinio_nvram_lock ();

		/* Automatically invoke update to save the correct
		 * checksum for the default values. */
		csum_area_update (csi);
	}
}

