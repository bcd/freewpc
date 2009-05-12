/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

#ifndef _GENERIC_H
#define _GENERIC_H

/* Locales.  The values here are taken directly from the WPC jumper codes,
but they are otherwise generic. */
#define LC_USA_CANADA 0
#define LC_FRANCE 1
#define LC_GERMANY 2
#define LC_FRANCE2 3
#define LC_EXPORT_ENGLISH 8
#define LC_FRANCE3 9
#define LC_EXPORT 10
#define LC_FRANCE4 11
#define LC_UK 12
#define LC_EUROPE 13
#define LC_SPAIN 14
#define LC_USA_CANADA2 15


/** Atomically increment a variable in protected memory by N. */
#define wpc_nvram_add(var,n) \
	do { \
		volatile typeof(var) *pvar = &var; \
		wpc_nvram_get (); \
		*pvar += n; \
		wpc_nvram_put (); \
	} while (0)


/** Atomically decrement a variable in protected memory by N. */
#define wpc_nvram_subtract(var,n) \
	do { \
		volatile typeof(var) *pvar = &var; \
		wpc_nvram_get (); \
		*pvar -= n; \
		wpc_nvram_put (); \
	} while (0)


/** The call_far, wpc_push_page, and wpc_pop_page
 * macros are only safe when calling from the system
 * page, so don't define them otherwise. */
#if (PAGE == SYS_PAGE) || !defined(HAVE_PAGING)

#define call_far(page, fncall) \
do { \
	U8 __saved_page = wpc_get_rom_page (); \
	wpc_set_rom_page (page); \
	fncall; \
	wpc_set_rom_page (__saved_page); \
} while (0)


#define page_push(page) \
{ \
	U8 __saved_page = wpc_get_rom_page (); \
	wpc_set_rom_page (page);


#define page_pop() \
	wpc_set_rom_page (__saved_page); \
}

#endif /* PAGE == SYS_PAGE */


#endif /* _GENERIC_H */
