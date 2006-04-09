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

#ifndef _SYS_SOL_H
#define _SYS_SOL_H

#include <mach/coil.h>

typedef uint8_t solnum_t;

void sol_rtt (void);
void sol_on (solnum_t sol);
void sol_off (solnum_t sol);
void sol_pulse (solnum_t sol);
void sol_serve (void);
void sol_init (void);

void flasher_pulse (solnum_t n);
void flasher_rtt (void);
void flasher_init (void);

#endif /* _SYS_SOL_H */
