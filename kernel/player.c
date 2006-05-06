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

/* Theory of operation:
 *
 * Some state needs to be "per-player" and not global.
 * We will keep all data 'global' so that access can be fast.
 * We will reserve space for each player so that this data can be
 * swapped in/out quickly whenever the current player changes.
 */

void player_start_game (void)
{
	/* Clear all player local variables */
	__blockclear16 (LOCAL_BASE, LOCAL_SIZE);

	/* Clear the local bitflags */
	memset (get_bit_base (), 0, 8);
}

void player_save (void)
{
	__blockcopy16 (LOCAL_SAVE_BASE(player_up), LOCAL_BASE, LOCAL_SIZE);
}

void player_restore (void)
{
	__blockcopy16 (LOCAL_BASE, LOCAL_SAVE_BASE(player_up), LOCAL_SIZE);
}

