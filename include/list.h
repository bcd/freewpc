/*
 * Copyright 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef _LIST_H
#define _LIST_H

/* Generic doubly linked list macros. */


/** A doubly linked list header.  Any object that should be maintained
 * in a linked list should embed one of these structures in it. */
typedef struct dll_header
{
	struct dll_header *next;
	struct dll_header *prev;
} ELEM;

/* List functions are defined as macros so that typecasting
is not required in the main code. */

#define dll_init_element(e) dll_init_element1 ((ELEM *)e)
#define dll_init(h) dll_init1 ((ELEM **)h)
#define dll_add_front(h,e) dll_add_front1 ((ELEM **)h, (ELEM *)e)
#define dll_add_back(h,e) dll_add_back1((ELEM **)h, (ELEM *)e)
#define dll_remove(h,e) dll_remove1 ((ELEM **)h, (ELEM *)e)


void dll_init_element1 (ELEM *elem);
void dll_init1 (ELEM **head);
void dll_add_front1 (ELEM **head, ELEM *elem);
void dll_add_back1 (ELEM **head, ELEM *elem);
void dll_remove1 (ELEM **head, ELEM *elem);

#endif /* LIST_H */


