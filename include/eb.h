/*
 * Copyright 2006, 2007, 2009 by Brian Dominy <brian@oddchange.com>
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

#ifndef _EB_H
#define _EB_H

extern U8 extra_balls;

__common__ void increment_extra_balls (void);
__common__ bool decrement_extra_balls (void);
__common__ void clear_extra_balls (void);

__common__ void light_easy_extra_ball (void);
__common__ void light_hard_extra_ball (void);
__common__ void collect_extra_ball (void);

#endif /* GAME_H */

