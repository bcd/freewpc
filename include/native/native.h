/*
 * Copyright 2012 by Brian Dominy <brian@oddchange.com>
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

#ifndef _NATIVE_NATIVE_H
#define _NATIVE_NATIVE_H

/** AREA_DECL is used to expose a linker area name within the C
 * variable namespace.  It appears an external name.  The asm syntax
 * is needed so that the normal appending of an underscore does not
 * occur. */
#define AREA_DECL(name) extern U8 *__start_ ## name; extern U8 *__stop_ ## name;

/** Return the base address of a linker area.  This has type (U8 *). */
#define AREA_BASE(name) (&__start_ ## name)

/** Return a pointer to the end of the linker area. */
#define AREA_END(name) (&__stop_ ## name)

/** Return the runtime size of a linker area.  This has type U16.
 * This is not the maximum allowable space for the area, but rather
 * reflects how many actual variables have been mapped there. */
#define AREA_SIZE(name) ((U16)( (int)AREA_END(name) - (int)AREA_BASE(name) ))

/* Define externs for all of these areas.  AREA_BASE and AREA_SIZE can
 * only be called on these. */
AREA_DECL(direct)
AREA_DECL(ram)
AREA_DECL(local)
AREA_DECL(heap)
AREA_DECL(stack)
AREA_DECL(permanent)
AREA_DECL(nvram)


#endif /* _NATIVE_NATIVE_H */


