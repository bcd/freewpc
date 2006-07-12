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

#include <freewpc.h>

extern const struct area_csum coin_csum_info;
extern const struct area_csum replay_csum_info;


const struct area_csum *csum_info_table[] = {
	&coin_csum_info,
	&replay_csum_info,
};

const U8 csum_paging_info_table[] = {
	SYS_PAGE,
	COMMON_PAGE,
};


void
csum_area_update (struct area_csum *csi)
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


void
csum_area_check (struct area_csum *csi)
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


