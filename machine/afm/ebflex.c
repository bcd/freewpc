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
#include <flex.h>

__nvram__ struct flex_data eb_flex_data;
extern const struct flex_config eb_flex_config;

void eb_flex_reset (void)
{
	flex_reset (&eb_flex_config);
}

const struct area_csum eb_flex_csum_info = {
	.type = FT_FLEX1, /* need to add this */
	.version = 1,
	.area = (U8 *)&eb_flex_data,
	.length = sizeof (struct flex_data),
	.reset = eb_flex_reset,
};

const struct flex_config eb_flex_config = {
	.min_level = 6,
	.max_level = 14,
	.adj_level = NULL,
	.min_percent = 0, /* only used by adjustment range check */
	.max_percent = 20, /* only used by adjustment range check */
	.adj_percent = &feature_config.eb_percent,
	.frequency = 50,
	.data = &eb_flex_data,
	.csum = &eb_flex_csum_info,
};

CALLSET_ENTRY (ebflex, file_register)
{
	file_register (&eb_flex_csum_info);
}

CALLSET_ENTRY (ebflex, init_complete)
{
	dbprintf ("EB level=%d\n", eb_flex_data.level);
}

CALLSET_ENTRY (ebflex, idle_every_second)
{
#if 1
	if (deff_get_active () == DEFF_AMODE)
	{
		flex_end_player (&eb_flex_config, random_scaled (25));
		flex_end_game (&eb_flex_config);
	}
#endif
}

