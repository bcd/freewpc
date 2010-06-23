/*
 * Copyright 2009-2010 by Brian Dominy <brian@oddchange.com>
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
 * \brief Message subsystem
 *
 * This module is used to write diagnostic messages during test report.
 * It is needed to overcome two problems:
 *
 * 1. It can break up a long string, separated by newlines, into multiple
 * lines and format it onto the display correctly.
 *
 * 2. Because it passes strings around, it deals with some bank switching
 * issues that come up on 6809 hardware.
 */

#include <freewpc.h>


void (*message_line_handler) (U8 index, const char *line);

U8 message_line_count;


/**
 * Handles one line of a multiline message, during phase 1.
 * In this phase, we just count how many total lines there are.
 */
static void line_counter (U8 index, const char *line)
{
	message_line_count++;
}


/**
 * Handles one line of a multiline message, during phase 2.
 * In this phase, we actually writes the line to the display.
 * The line count determined in phase 1 controls where we put it.
 */
static void line_printer (U8 index, const char *line)
{
	U8 row;

	if (message_line_count == 1)
	{
		row = 16;
	}
	else if (message_line_count == 2)
	{
		if (index == 0)
			row = 10;
		else
			row = 20;
	}
	else
		return;

	font_render_string_center (&font_mono5, 64, row, line);
}


/**
 * Iterate over the lines of a message.  Each time a newline is
 * found, the contents of that line are passed to a callback
 * function for processing.
 *
 * The last line must also end with a newline, else it will not
 * be processed at all.
 */
static void message_iterate (char *msg)
{
	char *line = msg;
	U8 index = 0;

	while (*msg != '\0')
	{
		if (*msg == '\n')
		{
			*msg = '\0';
			message_line_handler (index, line);
			*msg = '\n';
			line = msg+1;
			index++;
		}
		msg++;
	}
}


/**
 * Write a multiline message to the display.
 * MSG is the string, which may contain embedded newlines.
 * PAGE is the bank in which the caller resides (and therefore, where
 *   the string itself resides).
 *
 * This is the main API exported by this module.
 */
void message_write (const char *msg, U8 page)
{
	dmd_alloc_low_clean ();
	page_push (page);

	if (msg != sprintf_buffer)
		sprintf ("%s", msg);

	message_line_handler = line_counter;
	message_line_count = 0;
	message_iterate (sprintf_buffer);

	message_line_handler = line_printer;
	message_iterate (sprintf_buffer);

	page_pop ();
}

