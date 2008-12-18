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

/**
 * \file
 * \brief A generic linked list implementation.
 */

#include <freewpc.h>

void dll_init_element1 (ELEM *elem)
{
}


/** Initialize a doubly-linked list. */
void dll_init1 (ELEM **head)
{
	*head = NULL;
}


/** Add an element to the front of a doubly-linked list. */
void dll_add_front1 (ELEM **head, ELEM *elem)
{
	if (*head == NULL)
	{
		*head = elem;
		elem->next = elem->prev = elem;
	}
	else
	{
		ELEM *second = *head;
		*head = elem;
		elem->next = second;
		elem->prev = second->prev;
		second->prev->next = elem;
		second->prev = elem;
	}
}


/** Add an element to the back of a doubly-linked list. */
void dll_add_back1 (ELEM **head, ELEM *elem)
{
}


/** Remove an element from a doubly-linked list. */
void dll_remove1 (ELEM **head, ELEM *elem)
{
	if (elem->next == elem)
	{
		/* If the list only has 1 element, it becomes empty. */
		*head = NULL;
	}
	else
	{
		/* Else, remove the element from the list and
		chain together its prev and next nodes */
		elem->next->prev = elem->prev;
		elem->prev->next = elem->next;

		/* If this was the head of the list,
		change it to point to the next element */
		if (elem == *head)
			*head = elem->next;
	}
}

