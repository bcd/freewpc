/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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

#ifndef _FLIPCODE_H
#define _FLIPCODE_H

/* To recognize a flipper code in attract mode, declare a CALLSET_ENTRY
   for the event 'flipper_code_entered'.  Then call either flipcode3()
	or flipcode4() to get the sequence that was entered, and compare it
	against the value that you are handling.  Values returned are
	in BCD, so for example, the sequence 1-2-3-4 would be tested as
	'flipcode4() == 0x1234'. */

extern U16 flipcode_value;

#define flipcode3() (flipcode_value & 0xFFF)
#define flipcode4() flipcode_value

#endif /* _FLIPCODE_H */
