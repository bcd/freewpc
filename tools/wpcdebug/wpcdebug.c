/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <termios.h>
#ifndef STANDALONE
#include "driver.h"
#include "core.h"
#endif

/* 
This is the standalone WPC debugger program.
WPC calls their version 'orkin', for obvious reasons...

The ORKIN registers act very much like another serial port.
PinMAME has been patched to forward reads/writes to the ORKIN
registers to this file for processing.  A socket interface
is used to allow the debugger and the emulator to run together.
*/

#ifdef WPC_ORKIN_DEBUG
#define orkin_debug(fmt, rest...)		printf (fmt , ## rest )
#else
#define orkin_debug(fmt, rest...)
#endif


#ifdef STANDALONE
typedef unsigned char UINT8;
#endif

/* Orkin (debugger) emulation
 * 
 * Control Port (0x3D61)
 * Read
 * Bit 1 : 1=Ready, 0=Busy
 * Bit 0 : 1=Presence
 *
 * Data Port (0x3D60)
 * Writes to this port sends a byte of data to the console display.
 * Reads from this port receives the next character from the console keyboard.
 *
 * After a clear to ORKIN_DATA_PORT, the presence latch will read 0 on 
 * the next single read.
 *
 * The game CPU will check presence three times before declaring that 
 * the debugger is truly present.
 *
 * When control bit 1 is zero, the CPU will loop until it becomes clear
 * before sending any more data.  Then the data register is read for
 * some reason...
 *
 * Writes can occur whenever Presence=1 (Write Ready?)
 * Bit 1=1 when a Read is pending
 */

/*
 * The WPCMAME side of the debug interface creates a TCP socket at a
 * well-known address.  Reads/writes are taken from small FIFOs
 * that are driven by client apps connecting to the socket and
 * doing their own reads/writes.  When no client exists, reads
 * return 0xff and writes are ignored.  Only one client at a time
 * is supported currently.
 */

#define SOCK_READABLE	0x1
#define SOCK_WRITEABLE	0x2


void process_input (int fd, uint8_t val);


/* Configuration */
static int server_port = 3333;

/* Socket descriptions for the server and client */
static int server_sd = 0;
static int client_sd = 0;

static int write_ready = 1;


static int one_shot_mode = 0;


static int
orkin_select (int sd)
{
	fd_set rds, wds, eds;
	struct timeval to;
	int rc;

	FD_ZERO (&rds);
	FD_ZERO (&wds);
	FD_ZERO (&eds);
	FD_SET(sd, &rds);
	FD_SET(sd, &wds);

	to.tv_sec = to.tv_usec = 0;
	
	rc = select (sd+1, &rds, &wds, &eds, &to);
	if (rc > 0)
	{
		rc = 0;
		if (FD_ISSET (sd, &rds))
			rc |= SOCK_READABLE;
		if (FD_ISSET (sd, &wds))
			rc |= SOCK_WRITEABLE;
	}
	return (rc);
}


static int
orkin_readable (int sd)
{
	return !!(orkin_select (sd) & SOCK_READABLE);
}


#if 0
static int
orkin_writeable (int sd)
{
	return !!(orkin_select (sd) & SOCK_WRITEABLE);
}
#endif


static int
orkin_get_client_sd (void)
{
	if (client_sd)
		return client_sd;

	if (!orkin_readable (server_sd))
		return 0;

	client_sd = accept (server_sd, NULL, 0);
	if (client_sd <= 0)
		client_sd = 0;

	return client_sd;
}


static void
orkin_close_client (void)
{
	close (client_sd);
	client_sd = 0;
}


UINT8
orkin_control_read (void)
{
	int val = 0;
	/* Check if data read ready and write ready, and return result */
	int sd = orkin_get_client_sd ();
	if (sd) 
		val = (orkin_readable (sd) ? 0x2 : 0x0) + write_ready;

	write_ready = 1;

	orkin_debug ("Orkin Control Read = %02X\n", val);
	return (val);
}

UINT8
orkin_data_read (void)
{
	UINT8 val = 0xFF;
	/* Return next byte of data written by client */
	int sd = orkin_get_client_sd ();
	if (sd) 
		if (orkin_readable (sd))
			if (read (sd, &val, 1) == 0)
				orkin_close_client ();
	orkin_debug ("Orkin Data Read = %02X\n", val);
	return (val);
}

void
orkin_control_write (UINT8 val)
{
	/* Control is never written, so just return */
	orkin_debug ("Orkin Control Write(%02X) *** ERROR ***\n", val);
	return;
}

void
orkin_data_write (UINT8 val)
{
	/* Send data byte to the client socket if connected */
	orkin_debug ("Orkin Data Write(%02X)\n", val);
	int sd = orkin_get_client_sd ();
	if (!sd) 
		return;
	write (sd, &val, 1);

	write_ready = 0;

	/* Clear input queue after sending a character */
	while (orkin_readable (sd))
		read (sd, &val, 1);
}

	
int
orkin_init (void)
{
	struct sockaddr_in addr;
	int flag;

	orkin_debug ("Initializing Orkin server.\n");

	/* Create a TCP server socket */
	if ((server_sd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
		goto socket_error;

	/* Set reuse */
	flag = 1;
	if (setsockopt (server_sd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof (flag))
			< 0)
		goto reuse_error;

	/* Bind to the well-known port number */
	addr.sin_family = AF_INET;
	addr.sin_port = server_port;
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind (server_sd, (struct sockaddr *)&addr, sizeof (addr)) < 0)
		goto bind_error;

	if (listen (server_sd, 1) < 0)
		goto listen_error;

	return (0);

listen_error:;
	orkin_debug ("Cleaning up listen error\n");

bind_error:;
	orkin_debug ("Cleaning up bind error\n");

reuse_error:;
	orkin_debug ("Cleaning up reuse error\n");

socket_error:
	orkin_debug ("Errno is %d\n", errno);
	orkin_debug ("Closing socket\n");
	close (server_sd);
	server_sd = 0;
	return (-1);
}


#ifdef STANDALONE

int
orkin_client (int argc, char *argv[])
{
	struct sockaddr_in addr;
	int rc;	
	int fd;
	struct termios t;
	int init;

start:
	fd = socket (PF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		orkin_debug ("*** Could not open client socket.\n");
		exit (1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = server_port;
	addr.sin_addr.s_addr = INADDR_ANY;
	rc = -1;
	printf (">>> Connecting...\n");
	while (rc < 0)
	{
		rc = connect (fd, (struct sockaddr *)&addr, sizeof (addr));
		if (rc < 0)
		{
			sleep (1);
		}
	}

	tcgetattr (0, &t);
	t.c_lflag &= ~ICANON;
	tcsetattr (0, TCSANOW, &t);

	for (init = 0; init < 2; init++)
	{
		int val;

		printf (">>> Sending init byte #%d\n", init);
		val = 0;
		write (fd, &val, 1);

		if (init == 1)
			break;

		printf (">>> Waiting for ACK from game...");
		while (!orkin_readable (fd))
			usleep (10000);
		printf (" ACK received\n");

		read (fd, &val, 1);
		printf (">>> ACK byte was %02X\n", val);
	}

	printf ("\nREADY.\n");
	for (;;)
	{
		UINT8 val;

		if (orkin_readable (0))
		{
			read (0, &val, 1);
			write (fd, &val, 1);
			continue;
		}

		if (orkin_readable (fd))
		{
			if (read (fd, &val, 1) == 0)
			{
				close (fd);
				if (one_shot_mode)
					exit (0);
				else
					goto start;
			}
			process_input (fd, val);
			continue;
		}

		usleep (25000);
	}

	return 0;
}


int
main (int argc, char *argv[])
{
	if (argc > 1)
	{
		char option = argv[1][1];
		switch (option)
		{
			case '1':
				one_shot_mode = 1;
				break;

			case 's':
			{
				orkin_init ();
				for (;;)
				{
					printf ("CTL %02X DATA %02X\n",
						orkin_control_read (), orkin_data_read ());
					orkin_data_write ('.');
					sleep (1);
				}
			}
		}
	}
	return orkin_client (argc, argv);
}
#endif

