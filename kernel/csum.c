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

/**
 * \file
 * \brief Generic checksum calculation/verification routines.
 * Each module that uses protected memory should declare a structure of
 * type "struct area_csum" that says how that memory should be
 * managed.
 */

#include <freewpc.h>


extern const struct area_csum coin_csum_info;
extern const struct area_csum replay_csum_info;
extern const struct area_csum champion_csum_info;
extern const struct area_csum highscore_csum_info;
extern const struct area_csum rtc_csum_info;
extern const struct area_csum adj_csum_info;
extern const struct area_csum audit_csum_info;


/** A table of all csum info structures.  All of these
will be scanned during initialization to ensure that the
protected memory is valid. */
const struct area_csum *csum_info_table[] = {
	&coin_csum_info,
	&replay_csum_info,
	&champion_csum_info,
	&highscore_csum_info,
	&rtc_csum_info,
	&adj_csum_info,
	&audit_csum_info,
};


/** A table of the csum info page locations */
const U8 csum_paging_info_table[] = {
	COMMON_PAGE,
	COMMON_PAGE,
	COMMON_PAGE,
	COMMON_PAGE,
	COMMON_PAGE,
	SYS_PAGE,
	SYS_PAGE,
};


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
	wpc_nvram_get ();
	*(csi->csum) = csum;
	wpc_nvram_put ();
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
		wpc_nvram_get ();
		csi->reset ();
		wpc_nvram_put ();

		/* Automatically invoke update to save the correct
		 * checksum for the default values. */
		csum_area_update (csi);
	}
}


/**
 * Checks all checksummed regions for correctness.
 * This is called during system initialization.
 */
void
csum_area_check_all (void)
{
	U8 n;
	for (n=0 ;
		n < sizeof (csum_info_table) / sizeof (struct area_csum *);
		n++)
	{
		wpc_push_page (csum_paging_info_table[n]);
		csum_area_check (csum_info_table[n]);
		wpc_pop_page ();
	}
}

