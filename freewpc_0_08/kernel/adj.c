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

/**
 * \file
 * \brief Adjustment module.
 *
 * This module declares non-volatile variables (in the protected area
 * of the RAM) for storing adjustment information.
 */
#include <freewpc.h>
#include <test.h>

__nvram__ std_adj_t system_config;

__nvram__ pricing_adj_t price_config;

__nvram__ hstd_adj_t hstd_config;

#ifdef MACHINE_FEATURE_ADJUSTMENTS
__nvram__ feature_adj_t feature_config;
#endif


__nvram__ U8 adj_csum;
const struct area_csum adj_csum_info = {
	.area = &system_config,
	.length = sizeof (system_config) + sizeof (price_config)
		+ sizeof (hstd_config)
#ifdef MACHINE_FEATURE_ADJUSTMENTS
		+ sizeof (feature_config)
#endif
											,
	.csum = &adj_csum,
	.reset = adj_reset_all,
};

/** Initialize the adjustment module.
 *
 * Verify that all adjustments are sane; if any values are out-of-range
 * or checksums fail, the adjustment will be reset to factory
 * defaults.
 */
void adj_init (void)
{
	extern void adj_verify_all (void);
	call_far (TEST_PAGE, adj_verify_all ());
}

