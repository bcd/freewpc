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

#ifndef _XBMPROG_H
#define _XBMPROG_H

#define XBMPROG_METHOD_RAW 0
#define XBMPROG_METHOD_RLE 1
#define XBMPROG_METHOD_RLE_DELTA 2
#define XBMPROG_METHOD_END 3

#ifndef XBMPROG_RLE_SKIP
#define XBMPROG_RLE_SKIP 0xEEu
#endif
#ifndef XBMPROG_RLE_REPEAT
#define XBMPROG_RLE_REPEAT 0xEDu
#endif

#endif /* _XBMPROG_H */
