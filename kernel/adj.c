/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Adjustment module.
 *
 * This module declares non-volatile variables (in the protected area
 * of the RAM) for storing adjustment information.
 */
#include <freewpc.h>
#include <test.h>


/** An area of protected memory used to test that it is kept locked. */
__nvram__ U8 nvram_test_byte;

__nvram__ std_adj_t system_config;

__nvram__ pricing_adj_t price_config;

__nvram__ hstd_adj_t hstd_config;

__nvram__ printer_adj_t printer_config;

#ifdef MACHINE_FEATURE_ADJUSTMENTS
__nvram__ feature_adj_t feature_config;
#endif

adjval_t *last_adjustment_changed;


/* The reset function is declared here because the real
reason function is in a different ROM page, and we can't
yet store far pointers. */
void adj_csum_failure (void)
{
	adj_reset_all ();
}


__nvram__ U8 adj_csum;
const struct area_csum adj_csum_info = {
	.area = (U8 *)&system_config,
	.length = sizeof (system_config) + sizeof (price_config)
		+ sizeof (hstd_config) + sizeof (printer_config)
#ifdef MACHINE_FEATURE_ADJUSTMENTS
		+ sizeof (feature_config)
#endif
											,
	.csum = &adj_csum,
	.reset = adj_csum_failure,
};


/** Called when an adjustment has been changed.  The checksum area
 * needs to be recalculated, and modules may want to know about the
 * change. */
void adj_modified (adjval_t *adjp)
{
	csum_area_update (&adj_csum_info);
	if (adjp)
	{
		last_adjustment_changed = adjp;
		callset_invoke (adjustment_changed);
	}
}


/** Initialize the adjustment module.
 *
 * Verify that all adjustments are sane; if any values are out-of-range
 * or checksums fail, the adjustment will be reset to factory
 * defaults.
 */
void adj_init (void)
{
	adj_verify_all ();
}


/** Check the protected memory at idle time to make sure that it is locked.
 * Halt the system if found unlocked. */
CALLSET_ENTRY (nvram, idle_every_100ms)
{
#ifdef HAVE_NVRAM
	U8 data = nvram_test_byte;
	++nvram_test_byte;
	barrier ();
	if (data != nvram_test_byte)
	{
		fatal (ERR_NVRAM_UNLOCKED);
	}
#endif
}

