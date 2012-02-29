/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SYS_TRIAC_H
#define _SYS_TRIAC_H

/** The number of triacs provided by the hardware */
#define NUM_GI_TRIACS	5

/** The number of brightness levels for a GI circuit.
The state of the triac can be rapidly switched this many times
per AC cycle */
#define NUM_BRIGHTNESS_LEVELS 8


typedef U8 triacbits_t;

#ifdef CONFIG_TRIAC
void triac_rtt (void);
void gi_dim (U8 bits, U8 brightness);
void gi_leff_dim (U8 bits, U8 brightness);
#endif

void gi_enable (triacbits_t bits);
void gi_disable (triacbits_t bits);
void gi_leff_allocate (U8 triac);
void gi_leff_free (U8 triac);
void gi_leff_enable (U8 triac);
void gi_leff_disable (U8 triac);

void gi_init (void);

#endif /* _SYS_TRIAC_H */
