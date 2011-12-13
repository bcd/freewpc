/*
 * Copyright 2008, 2010 by Brian Dominy <brian@oddchange.com>
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
#include <simulation.h>
#include <imglib.h>

#define MAX_PHASES 3

struct buffer *asciidmd_buffers[PINIO_NUM_DMD_PAGES] = { NULL, };

struct buffer *asciidmd_pre_splits[MAX_PHASES] = { NULL, };

struct buffer *asciidmd_phases[MAX_PHASES] = { NULL, };

U8 asciidmd_pages[MAX_PHASES] = { 0, 0 };

unsigned int asciidmd_phase = 0;

U8 *pinio_dmd_low_page;
U8 *pinio_dmd_high_page;

U8 asciidmd_visible_page;


/**
 * Allocate a buffer for a dot-matrix page.
 * This is only done once per page at initialization.
 */
static struct buffer *asciidmd_alloc (void)
{
	struct buffer *buf = buffer_alloc (128 * 32 / 8);
	buf->width = 128 / 8;
	buf->height = 32;
	return buf;
}


/**
 * Handle a write to one of the two DMD paging registers.
 */
void asciidmd_map_page (int mapping, int page)
{
	page &= 0x0F;
	if (mapping == 0)
		pinio_dmd_low_page = asciidmd_buffers[page]->_data;
	else if (mapping == 1)
		pinio_dmd_high_page = asciidmd_buffers[page]->_data;
}


/**
 * Refresh the ASCII dot-matrix.
 */
void asciidmd_refresh (void)
{
	int phase;
	struct buffer *buf, *splitbuf, *composite;
	int pos;

	/* Convert from compact to expanded form */
	buf = asciidmd_buffers[asciidmd_visible_page];

	/* Add to the phase ring */
	if (asciidmd_pre_splits[asciidmd_phase % MAX_PHASES] != buf)
	{
		asciidmd_pre_splits[asciidmd_phase % MAX_PHASES] = buf;
		splitbuf = buffer_splitbits (buf);
		if (asciidmd_phases[asciidmd_phase % MAX_PHASES])
			buffer_free (asciidmd_phases[asciidmd_phase % MAX_PHASES]);
		asciidmd_phases[asciidmd_phase % MAX_PHASES] = splitbuf;
	}
	else
		return;

	composite = frame_alloc ();
	if (composite == NULL)
		return;

	for (phase = 0; phase < MAX_PHASES; phase++)
	{
		struct buffer *buf = asciidmd_phases[phase];
		if (buf == NULL)
			goto done;

		for (pos=0; pos < composite->len; pos++)
			composite->data[pos] += buf->data[pos];
	}

	/* Show on the screen */
	ui_refresh_asciidmd (composite->data);

done:
	/* Free the composite */
	buffer_free (composite);
}


/**
 * Change the visible DMD page.
 */
void asciidmd_set_visible (int page)
{
	page &= 0x0F;
	asciidmd_phase++;
	asciidmd_pages[asciidmd_phase % MAX_PHASES] = page;
	asciidmd_visible_page = page;
	asciidmd_refresh ();
}


/**
 * Initialize the ASCII-DMD.
 */
void asciidmd_init (void)
{
	int n;
	for (n = 0; n < PINIO_NUM_DMD_PAGES; n++)
		asciidmd_buffers[n] = asciidmd_alloc ();

	for (n = 0; n < MAX_PHASES; n++)
	{
		asciidmd_phases[n] = NULL;
		asciidmd_pre_splits[n] = NULL;
	}

	asciidmd_map_page (0, 0);
	asciidmd_map_page (1, 0);
	asciidmd_visible_page = 0;
}

