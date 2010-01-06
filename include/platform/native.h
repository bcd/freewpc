/*
 * Copyright 2007, 2008, 2009, 2010, 2011 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Definitions/macros related to native builds.
 */

#ifndef _PLATFORM_NATIVE_H
#define _PLATFORM_NATIVE_H

#define far_call_pointer(function, page, arg) (*function) (arg)
#define slow_memcpy memcpy
#define slow_memset memset

extern inline U8 far_read8 (const void *address, U8 page)
{
	return *(U8 *)address;
}

extern inline U16 far_read16 (const void *address, U8 page)
{
	return *(U16 *)address;
}

extern inline void *far_read_pointer (const void *address, U8 page)
{
	return *(void **)address;
}

typedef void (*void_function) (void);

extern inline void far_indirect_call_handler (void_function address, U8 page)
{
	address ();
}

typedef U8 (*value_function) (void);

extern inline U8 far_indirect_call_value_handler (value_function address, U8 page)
{
	return address ();
}

/**
 * Load the low-order 8-bits of an address into a byte register for
 * a function argument.
 */
#define __addrval(p) (((int)p) & 0xFF)

#endif /* _PLATFORM_NATIVE_H */

