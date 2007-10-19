/*
 * Copyright 2007 by Brian Dominy <brian@oddchange.com>
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
	obj->flash_period = period;
}


void animation_add_symbol (U8 x0, U8 y0,
	void (*update) (struct animation_object *))
{
}


/** Step through one frame of an animation.  Allocates new DMD pages
for the frame, initializes the pages as needed, then calls each of the
elements to draw itself and finally displays the page(s). */
void animation_step (void)
{
	U8 n;

	dbprintf ("animation_step\n");

	if (an->flags & AN_DOUBLE)
	{
		dmd_alloc_low_high ();
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

		if ((obj->flash_period == 0)
			|| (an->iteration % obj->flash_period) < (obj->flash_period / 2))
		{
			obj->draw (obj);
		}
	}

	if (an->flags & AN_DOUBLE)
		dmd_show2 ();
	else
		dmd_show_low ();

	an->iteration++;
	task_sleep (an->interframe_delay);
}


void animation_run (void)
{
	for (;;)
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

