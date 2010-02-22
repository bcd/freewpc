/*
 * Copyright 2007-2010 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>
#include <animation.h>

struct animation *an;


/** Begin a new DMD animation. */
struct animation *animation_begin (U8 flags)
{
	dbprintf ("animation_begin\n");

	an = malloc (sizeof (struct animation));
	an->n_objects = 0;
	an->interframe_delay = TIME_100MS;
	an->position = 0;
	an->flags = flags;
	an->iteration = 0;
	return an;
}


void animation_set_speed (U8 interframe_delay)
{
	an->interframe_delay = interframe_delay;
}


/** Add a new element to an animation. */
struct animation_object *animation_add (
	U8 x, U8 y,
	void (*draw) (struct animation_object *))
{
	struct animation_object *obj;

	dbprintf ("animation_add\n");

	obj = an->object[an->n_objects++] =
		malloc (sizeof (struct animation_object));

	obj->x = x;
	obj->y = y;
	obj->draw = draw;
	obj->flags = 0;
	obj->data.ptr = 0;
	obj->flash_period = 0;
	return obj;
}


void animation_object_flash (struct animation_object *obj, U8 period)
{
#ifdef PARANOID
	/* Ensure that this is a power of 2 */
	if (period & (period - 1))
	{
	}
#endif
	obj->flash_period = period;
}


/** Step through one frame of an animation.  Allocates new DMD pages
for the frame, initializes the pages as needed, then calls each of the
elements to draw itself and finally displays the page(s). */
void animation_step (void)
{
	U8 n;

	if (an->flags & AN_DOUBLE)
	{
		dmd_alloc_pair ();
		if (an->flags & AN_CLEAN)
			dmd_clean_page_low ();
	}
	else
	{
		if (an->flags & AN_CLEAN)
			dmd_alloc_low_clean ();
		else
			dmd_alloc_low ();
	}

	for (n = 0; n < an->n_objects; n++)
	{
		struct animation_object *obj = an->object[n];

		/* If the flash period is nonzero, that means we only draw the object
		 * 50% of the time.  For example, a flash period of 8 would draw the
		 * object on iterations 0, 1, 2, and 3; but not on 4, 5, 6, or 7.
		 * IDEA : Since the iteration count is not used
		 * elsewhere, it should be zeroed when it reaches the max below. */
		if ((obj->flash_period == 0)
			|| (an->iteration & (obj->flash_period - 1)) < (obj->flash_period / 2))
		{
			obj->draw (obj);
		}
	}

	if (an->flags & AN_DOUBLE)
		dmd_show2 ();
	else
		dmd_show_low ();

	/* TODO : for video modes there should be a callback here for
	 * updating 'global state', like collision detection. */

	an->iteration++;
	task_sleep (an->interframe_delay);
}


void animation_run (void)
{
	while (!(an->flags & AN_STOP))
		animation_step ();
}


/** End a DMD animation.  Frees all of the dynamically allocated memory
needed to store the animation state. */
void animation_end (void)
{
	U8 n;
	for (n = 0 ; n < an->n_objects; n++)
		free (an->object[n]);
	free (an);
	an = NULL;
}

