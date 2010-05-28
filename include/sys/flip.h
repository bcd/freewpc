/*
 * Copyright 2006, 2007, 2010 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SYS_FLIP_H
#define _SYS_FLIP_H

void flipper_enable (void);
void flipper_disable (void);
void flipper_init (void);
void fliptronic_rtt (void);
void fliptronic_ball_search (void);
void flipper_hold_on (void);
void flipper_hold_off (void);
void flipper_override_pulse (U8 switches);

#endif /* _SYS_FLIP_H */
