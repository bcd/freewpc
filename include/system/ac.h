/*
 * Copyright 2006, 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef _AC_H
#define _AC_H

#define AC_DOMESTIC_CYCLE 17
#define AC_EXPORT_CYCLE 20

/**
 * The different states of the zerocross circuit.
 */
typedef enum {
	/** At initialization, it is unknown if the zerocross circuit is
	 * functional or not, or whether we are running on 50Hz or 60Hz
	 * AC.  In this state, do not allow any solenoids/GI to be
	 * controlled.
	 */
	ZC_INITIALIZING,

	/** Set when the zerocross circuit is known to be working OK.
	 * It can be used to perform lamp dimming and precise solenoid
	 * timing.
	 */
	ZC_WORKING,

	/** Set when the zerocross circuit is known to be broken.
	 * It will not be used anymore.
	 */
	ZC_BROKEN,
} zc_status_t;


extern __fastram__ U8 zc_timer_stored;
extern __fastram__ U8 zc_timer;

extern inline U8 zc_get_timer (void)
{
	return zc_timer;
}


extern inline zc_status_t zc_get_status (void)
{
	extern zc_status_t zc_status;
	return zc_status;
}

void ac_rtt (void);
void ac_init (void);

#endif /* _AC_H */

