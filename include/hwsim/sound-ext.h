/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

#ifndef _HWSIM_SOUND_EXT_H
#define _HWSIM_SOUND_EXT_H

enum {
	SOUND_ADDR_DATA,
	SOUND_ADDR_RESET_STATUS,
};

U8 sound_ext_read (void *board, unsigned int regno);
void sound_ext_write (void *board, unsigned int regno, U8 val);

#endif /* _HWSIM_SOUND_EXT_H */

