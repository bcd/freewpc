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

#ifndef _SYS_TRIAC_H
#define _SYS_TRIAC_H

#define NUM_GI_TRIACS	5

#define TRIAC_GI_STRING(n)			(1 << (n))
#define TRIAC_COINDOOR_INTERLOCK	0x40
#define TRIAC_FLIPPER_ENABLE		0x80

#define TRIAC_GI_MASK \
	(TRIAC_GI_STRING(0) | TRIAC_GI_STRING(1) | TRIAC_GI_STRING(2) | \
	TRIAC_GI_STRING(3) | TRIAC_GI_STRING(4))

#define TRIAC_CONTROL_MASK		(TRIAC_COINDOOR_INTERLOCK | TRIAC_FLIPPER_ENABLE)


#define flipper_triac_enable()	triac_enable (TRIAC_FLIPPER_ENABLE)
#define flipper_triac_disable()	triac_disable (TRIAC_FLIPPER_ENABLE)

#define coindoor_triac_enable()	triac_enable (TRIAC_COINDOOR_INTERLOCK)
#define coindoor_triac_disable()	triac_disable (TRIAC_COINDOOR_INTERLOCK)

#define triac_read()					triac_io_cache

#define triac_write(v) \
do { \
	triac_io_cache = v; \
	wpc_asic_write (WPC_GI_TRIAC, v); \
} while (0)

#define ac_zerocross_set()			(*(U8 *)WPC_ZEROCROSS & 0x80)

#define TRIAC_GI0					0x01
#define TRIAC_GI1					0x02
#define TRIAC_GI2					0x04
#define TRIAC_GI3					0x08
#define TRIAC_GI4					0x10
#define TRIAC_COIN_INTERLOCK	0x20
#define TRIAC_FLIPPER_ENABLE	0x80

typedef U8 triacbits_t;

void gi_recalc_zerocross (void);
void triac_rtt (void);
void triac_enable (triacbits_t bits);
void triac_disable (triacbits_t bits);
void triac_leff_allocate (U8 triac);
void triac_leff_free (U8 triac);
void triac_leff_enable (U8 triac);
void triac_leff_disable (U8 triac);
U8 triac_get_brightness (void);
void triac_set_brightness (U8 bits);

void triac_init (void);

#endif /* _SYS_TRIAC_H */
