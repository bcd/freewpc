/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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
 * \file
 * \brief AC/zerocross circuit handling
 *
 * This module is not yet complete and is not used.
 */

__fastram__ U8 ac_zc_count;

__fastram__ U8 ac_zc_period;


void ac_rtt (void)
{
#ifdef CONFIG_ZEROCROSS
	if ( wpc_read_ac_zerocross () )
	{
		/* At zero cross point */
		ac_zc_count = 0;
	}
	else
	{
		/* Not at zero cross point */
		ac_zc_count++;
	}
#endif /* CONFIG_ZEROCROSS */
}


CALLSET_ENTRY (ac, idle)
{
}


void ac_init (void)
{
	ac_zc_count = 0;
	ac_zc_period = 8;
}

