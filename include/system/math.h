/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef _FREEWPC_MATH_H
#define _FREEWPC_MATH_H

/** Returns a random true/false value */
#define random_bool()	(random() & 0x80)

#ifdef CONFIG_NATIVE
#undef random
#define random freewpc_random
#endif

U8 random (void);
U8 random_scaled (U8);
void random_hw_event (void);
void random_init (void);

#ifdef __m6809__
#include <m6809/math.h>
#else
#include <native/math.h>
#endif

#define bounded_increment(var, max) do { if (var < max) var++; } while (0)

#define bounded_decrement(var, min) do { if (var > min) var--; } while (0)

#define value_rotate_down(var,min,max) do { if (var == min) { var = max; } else { var--; } } while (0)

#define value_rotate_up(var,min,max) do { if (var == max) { var = min; } else { var++; } } while (0)
		

#endif /* _FREEWPC_MATH_H */
