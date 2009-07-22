/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
#include <queue.h>

#define HOST_BUFFER_SIZE 16

/* Queues the data from the sound board to the CPU board */
struct {
	queue_t header;
	U8 elems[HOST_BUFFER_SIZE];
} host_write_queue;
#define host_write_args &host_write_queue.header, HOST_BUFFER_SIZE


/* Queues the data from the CPU board to the sound board */
struct {
	queue_t header;
	U8 elems[HOST_BUFFER_SIZE];
} host_read_queue;
#define host_read_args &host_read_queue.header, HOST_BUFFER_SIZE


/** Receive a character from the host, and queue it for
later processing.  This is called only when there is
known to be a new character latched there, by way of
the IRQ line. */
void host_receive (void)
{
	queue_insert (host_read_args, readb (WPCS_HOST_INPUT));
}


/** Send a pending character back to the host. */
void host_send (void)
{
	if (!queue_empty_p (&host_write_queue.header))
	{
		writeb (WPCS_HOST_OUTPUT, queue_remove (host_write_args));
	}
}


void host_write (U8 val)
{
	queue_insert (host_write_args, val);
}


U8 host_read (void)
{
	return queue_remove (host_read_args);
}


bool host_read_ready (void)
{
	return !queue_empty_p (&host_read_queue.header);
}

void host_init (void)
{
	queue_init (&host_write_queue.header);
	queue_init (&host_read_queue.header);
}

