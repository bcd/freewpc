/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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

/* \file ui_remote.c
 * \brief The remote UI sends messages to a remote host instead of updating
 * the local display.  The visual component is thus separated from the FreeWPC
 * program.
 *
 * State changes are built up in memory until a certain amount of elapsed time.
 * Then, messages are generated which encapsulates all of the state change since
 * the last update.
 *
 * This is similar to the console UI but the output is more machine parseable.
 *
 * How the messages are formatted is common, but what happens after that is
 * configurable.  They could be dumped to the console for debugging, or sent
 * remotely (which is the real intent).
 */

/* Pinball state message types */
#define PSM_STRING 0   /* A string was just formatted to the game display */
#define PSM_DEBUG 1    /* The program printed a message to the debug port */
#define PSM_SIM 2      /* The simulator printed a debug message */
#define PSM_SOL 3      /* The states of solenoids have changed */
#define PSM_LAMP 4     /* The states of lamps have changed */
#define PSM_SWITCH 5   /* The states of switches have changed */
#define PSM_GI 6       /* The states of GI have changed */
#define PSM_SOUND 7    /* A sound command was written */
#define PSM_BALL 8     /* The location of a ball has changed */
#define PSM_TICK 9     /* The update tick has expired */

/* The frequency at which remote messages are sent */
#define PS_FREQ 50     /* Send messages every 50 milliseconds */


/* The remote message format is designed to keep messages as short as possible.
   On each remote update, we only send the changes that have occurred since the
	previous message.

	For binary input/outputs, the message contains a list of the I/Os that changed.
	The receiver is responsible for tracking the current state, and updating based
	on the changelists that it receives.  The message data is simply a list of the
	I/Os, identified by number, of that type which changed.  At most one message
	of these types is generated per update (there may be none).

   For non-binaries, like strings, the message just contains the string data.
	There can be many of these sent per update.
 */
struct remote_msg
{
	U8 type;
	U8 seq;
	U8 len;
	U8 data[128];
};

#define MAX_MSGS 256

struct remote_msg remote_msg_queue[MAX_MSGS];

unsigned int remote_msg_tail = 0;
unsigned int remote_msg_head = 0;

struct remote_msg *remote_sol_msg = NULL;
struct remote_msg *remote_lamp_msg = NULL;
struct remote_msg *remote_sw_msg = NULL;
struct remote_msg *remote_gi_msg = NULL;

unsigned long total_bytes = 0;
unsigned int total_updates = 0;

const char *remote_msg_typenames[] = {
	"String", "Debug", "Sim", "Sol", "Lamp", "Switch", "GI", "Sound", "Ball", "Tick",
};

struct remote_msg *remote_msg_alloc (U8 type)
{
	struct remote_msg *msg = remote_msg_queue + remote_msg_head;
	remote_msg_head = (remote_msg_head + 1) % MAX_MSGS;
	while (remote_msg_head == remote_msg_tail)
		task_sleep (1);
	msg->type = type;
	msg->seq = realtime_read() % 256; /* LSB of millisecond timer */
	msg->len = 0;
	return msg;
}


/* Add the element VAL to a message, indicating that it has changed.
   If VAL is already in the list, that means it has changed back to its
	previous value, and it should be removed. */
void remote_msg_modify (struct remote_msg *msg, U8 val)
{
	U8 *ptr = msg->data;
	U8 *endptr = ptr + msg->len;

	/* Scan the list to see if it's already there */
	while (ptr < endptr)
	{
		if (*ptr == val)
		{
			/* Yes, just remove it and exit */
			*ptr = endptr[-1];
			msg->len--;
			return;
		}
		ptr++;
	}

	/* No, it's not there, so add it */
	*endptr = val;
	msg->len++;
}


void remote_msg_send (struct remote_msg *msg)
{
	U8 *ptr, *endptr;

	if (msg->type == PSM_TICK)
		return;

	ptr = msg->data;
	endptr = ptr + msg->len;

	printf ("[%02X]  %s:", msg->seq, remote_msg_typenames[msg->type]);
	switch (msg->type)
	{
		case PSM_STRING:
		case PSM_DEBUG:
		case PSM_SIM:
		case PSM_BALL:
			printf (" %s", msg->data);
			break;

		default:
			while (ptr < endptr)
				printf (" %02X", *ptr++);
	}
	putchar ('\n');
}


void remote_msg_update (void)
{
	struct remote_msg *msg;

	msg = remote_msg_alloc (PSM_TICK);
	msg->len = 1;
	msg->data[0] = 0;
	total_updates++;
	unsigned int total = 0;

	while (remote_msg_tail != remote_msg_head)
	{
		msg = remote_msg_queue + remote_msg_tail;
		remote_msg_tail = (remote_msg_tail + 1) % MAX_MSGS;
		if (msg->len > 0)
			remote_msg_send (msg);
		total += 3 + msg->len;
	}
	remote_sol_msg = NULL;
	remote_lamp_msg = NULL;
	remote_sw_msg = NULL;
	remote_gi_msg = NULL;

#ifdef CONFIG_REMOTE_STATS
	if (total && total_updates > 80)
	{
		total_bytes += total;
		printf ("Sent %d bytes. (%d bytes/sec)\n", total,
			total_bytes / (total_updates / (1000 / PS_FREQ)));
	}
#endif
}


void ui_print_command (const char *cmdline)
{
}

void ui_console_render_string (const char *buffer)
{
	struct remote_msg *msg = remote_msg_alloc (PSM_STRING);
	msg->len = strlen (buffer);
	strcpy (msg->data, buffer);
}

void ui_write_debug (enum sim_log_class c, const char *buffer)
{
	if (c == SLC_DEBUG_PORT)
		printf ("[PROG] %s", buffer);
	else
		printf ("[SIM]  %s", buffer);
	printf ("\n");
}

void ui_write_solenoid (int solno, int on_flag)
{
	if (!remote_sol_msg)
		remote_sol_msg = remote_msg_alloc (PSM_SOL);
	remote_msg_modify (remote_sol_msg, solno);
}

void ui_write_lamp (int lampno, int on_flag)
{
	if (!remote_lamp_msg)
		remote_lamp_msg = remote_msg_alloc (PSM_LAMP);
	remote_msg_modify (remote_lamp_msg, lampno);
}

void ui_write_triac (int triacno, int on_flag)
{
	if (!remote_gi_msg)
		remote_gi_msg = remote_msg_alloc (PSM_GI);
	remote_msg_modify (remote_gi_msg, triacno);
}

void ui_write_switch (int switchno, int on_flag)
{
	if (!remote_sw_msg)
		remote_sw_msg = remote_msg_alloc (PSM_SWITCH);
	remote_msg_modify (remote_sw_msg, switchno);
}

void ui_write_sound_command (unsigned int x)
{
}

void ui_write_sound_reset (void)
{
}

void ui_write_task (int taskno, int gid)
{
}

#if (MACHINE_DMD == 1)
void ui_refresh_asciidmd (unsigned char *data)
{
}
#else
void ui_refresh_display (unsigned int x, unsigned int y, char c)
{
}
#endif

void ui_update_ball_tracker (unsigned int ballno, const char *location)
{
	struct remote_msg *msg = remote_msg_alloc (PSM_BALL);
	sprintf (msg->data, "%d=%s", ballno, location);
	msg->len = strlen (msg->data);
}

void ui_init (void)
{
	sim_time_register (PS_FREQ, TRUE, remote_msg_update, NULL);
}

void ui_exit (void)
{
}

