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
 * \brief Implements the one-coin buyin.  
 * This feature was never really used that much, AFAIK.
 */

void onecoin_buyin_deff (void)
{
	dmd_alloc_low_clean ();
	/* TODO */
	deff_exit ();
}


/** Called during end_ball when it is the last ball for a player
 * and the 1-coin buyin option is enabled */
void onecoin_buyin_offer (void)
{
	deff_start (DEFF_ONECOIN_BUYIN);
}


CALLSET_ENTRY (onecoin_buyin, init)
{
}

