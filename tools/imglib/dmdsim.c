/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

#include <termios.h>
#include "imglib.h"
#include "wpclib.h"


/** The socket for messaging with the CPU */
int cpu_sock;

/** The frame cache */
struct buffer *page_buffer[16] = { NULL, };

/** The current time on the CPU */
unsigned long cpu_time;

unsigned int cpu_port = 9000;

unsigned int system_port = 9001;


/**
 * Handle a message indicating different DMD page data.
 */
void process_dmd_page (struct wpc_message *msg)
{
	struct buffer *buf;
	int n;

	for (n = 0; n < NUM_DMD_PHASES; n++)
	{
		unsigned int page = msg->u.dmdpage.phases[n].page;
		unsigned char *data = msg->u.dmdpage.phases[n].data;

		if (page_buffer[page])
			buffer_free (page_buffer[page]);

		buf = frame_alloc ();
		memcpy (buf->data, data, 512);
		buf->len = 512;

		buf = buffer_replace (buf, buffer_splitbits (buf));
		buf->width = 128;
		buf->height = 32;

		page_buffer[page] = buf;
	}
}


/**
 * Handle a message indicating which DMD page is
 * currently visible.
 */
void process_visible_pages (struct wpc_message *msg)
{
	int phase;
	struct buffer *buf, *composite;
	int pos;

	composite = frame_alloc ();
	for (phase = 0; phase < NUM_DMD_PHASES; phase++)
	{
		unsigned int page = msg->u.dmdvisible.phases[phase];

		buf = page_buffer[page];
		for (pos=0; pos < composite->len; pos++)
			composite->data[pos] += (buf ? buf->data[pos] : 0);
	}
	bitmap_write_ascii (composite, stdout);
	buffer_free (composite);
}


/**
 * See if there is a message waiting from the CPU.
 * If so, process it.
 * This call is nonblocking, so if there is nothing to do,
 * it returns immediately.
 */
int try_receive_from_cpu (void)
{
	struct wpc_message aMsg;
	struct wpc_message *msg = &aMsg;
	int rc;

	rc = udp_socket_receive (cpu_sock, cpu_port, msg, sizeof (aMsg));
	if (rc < 0)
		return rc;

	cpu_time = msg->timestamp;
#ifdef DEBUG
		printf ("Code: %02X   Time: %09d   Len: %03d\n",
			msg->code, msg->timestamp, msg->len);
#endif

	switch (msg->code)
	{
		case CODE_DMD_PAGE:
			process_dmd_page (msg);
			break;

		case CODE_DMD_VISIBLE:
			process_visible_pages (msg);
			break;
		}
	return 0;
}


void process_key (unsigned char c)
{
}


int try_receive_from_stdin (void)
{
	fd_set fds;
	struct timeval timeout;
	unsigned char c;
	int rc;

	FD_ZERO (&fds);
	FD_SET (0, &fds);
	timeout.tv_sec = timeout.tv_usec = 0;

	if (rc = select (1, &fds, NULL, NULL, &timeout))
	{
		rc = read (0, &c, 1);
		process_key (c);
	}

	return rc;
}

void
keybuffering (int flag)
{
   struct termios tio;

   tcgetattr (0, &tio);
   if (!flag) /* 0 = no buffering = not default */
	{
      tio.c_lflag &= ~(ICANON | ECHO);
	}
   else /* 1 = buffering = default */
	{
      tio.c_lflag |= (ICANON | ECHO);
	}
   tcsetattr (0, TCSANOW, &tio);
}


void exit_handler (void)
{
	keybuffering (1);
}


int main (int argc, char *argv[])
{
	struct buffer *buf;

	/* Open a socket connection for sending/receiving messages
	with the CPU. */
	cpu_sock = udp_socket_create (system_port);
	if (cpu_sock < 0)
	{
		fprintf (stderr, "error: could not connect to CPU\n");
		exit (1);
	}

	keybuffering (0);

	/* The main loop */
	for (;;)
	{
		usleep (10 * 1000UL);
		try_receive_from_cpu ();
		try_receive_from_stdin ();
	}
}

