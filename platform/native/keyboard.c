/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <freewpc.h>
#include <simulation.h>

/* This file contains functions used to intercept keyboard input events. */

extern const char *exec_file;
extern int exec_late_flag;
extern const switch_info_t switch_table[];

/** The file descriptor to read from for input */
int sim_input_fd = 0;

/** A mapping from keyboard command to switch */
static switchnum_t keymaps[256] = {
#ifdef MACHINE_START_SWITCH
	['1'] = MACHINE_START_SWITCH,
#endif
#ifdef MACHINE_BUYIN_SWITCH
	['2'] = MACHINE_BUYIN_SWITCH,
#endif
	/* '3': SW_LEFT_COIN is omitted on purpose... see below */
	['4'] = SW_CENTER_COIN,
	['5'] = SW_RIGHT_COIN,
	['6'] = SW_FOURTH_COIN,
	['7'] = SW_ESCAPE,
	['8'] = SW_DOWN,
	['9'] = SW_UP,
	['0'] = SW_ENTER,
	[','] = SW_LEFT_BUTTON,
	['.'] = SW_RIGHT_BUTTON,
	['-'] = SW_COIN_DOOR_CLOSED,
#ifdef MACHINE_TILT_SWITCH
	['T'] = MACHINE_TILT_SWITCH,
#endif
#ifdef MACHINE_SLAM_TILT_SWITCH
	['!'] = MACHINE_SLAM_TILT_SWITCH,
#endif
#ifdef MACHINE_LAUNCH_SWITCH
	[' '] = MACHINE_LAUNCH_SWITCH,
#endif
};

/** A dummy function intended to be used for debugging under GDB. */
void gdb_break (void)
{
	barrier ();
}


void sim_key_install (char key, unsigned int swno)
{
	keymaps[(int)key] = swno;
}


/** Read a character from the keyboard.
 * If input is closed, shutdown the program. */
static char sim_getchar (void)
{
	char inbuf;
	ssize_t res = pth_read (sim_input_fd, &inbuf, 1);
	if (res <= 0)
	{
		task_sleep_sec (2);
		sim_exit (0);
	}
	return inbuf;
}


/** Turn on/off keybuffering.  Pass a zero to put the
console in raw mode, so keystrokes are not echoed.
Pass nonzero flag to go back to the default mode. */
static void keybuffering (int flag)
{
   struct termios tio;

   tcgetattr (0, &tio);
   if (!flag) /* 0 = no buffering = not default */
      tio.c_lflag &= ~ICANON;
   else /* 1 = buffering = default */
      tio.c_lflag |= ICANON;
   tcsetattr (0, TCSANOW, &tio);
}


/** Main loop for handling the user interface. */
static void sim_interface_thread (void)
{
	char inbuf[2];
	switchnum_t sw;
	int simulator_keys = 1;
	int toggle_mode = 1;

	/* Put stdin in raw mode so that 'enter' doesn't have to
	be pressed after each keystroke. */
	keybuffering (0);

	/* Let the system initialize before accepting keystrokes */
	task_sleep_sec (3);

	if (exec_file && exec_late_flag)
		exec_script_file (exec_file);

	for (;;)
	{
#ifdef CONFIG_GTK
		gtk_poll ();
		task_yield ();
#else
		*inbuf = sim_getchar ();

		/* If switch simulation is turned off, then keystrokes
		are fed directly into the runtime debugger. */
		if (simulator_keys == 0)
		{
			/* Except tilde turns it off as usual. */
			if (*inbuf == '`')
			{
				simlog (SLC_DEBUG, "Input directed to switch matrix.");
				simulator_keys ^= 1;
			}
			else
			{
				wpc_key_press (*inbuf);
			}
			continue;
		}

		switch (*inbuf)
		{
			case '\r':
			case '\n':
				break;

			case ':':
			{
				/* Read and execute a script command */
				char cmd[128];
				char *p = cmd;

				memset (p, 0, 128);
				ui_print_command (" ");
				for (;;)
				{
					*p = sim_getchar ();
					if (*p == '\x1B')
					{
						break;
					}
					else if (*p == '\010')
					{
						*p = '\0';
						p--;
					}
					else if ((*p == '\r') || (*p == '\n'))
					{
						*p = '\0';
						exec_script (cmd);
						break;
					}
					ui_print_command (cmd);
					p++;
				}
				ui_print_command ("");
				break;
			}

			case 'C':
				gdb_break ();
				break;

			case '{':
				signal_trace_start (signo_under_trace);
				break;

			case '}':
				signal_trace_stop (signo_under_trace);
				break;

			case 'q':
				node_kick (&open_node);
				break;

#if MAX_DEVICES > 1
			case 'w':
				node_move (&device_nodes[1], &open_node);
				break;
#endif
#if MAX_DEVICES > 2
			case 'e':
				node_move (&device_nodes[2], &open_node);
				break;
#endif
#if MAX_DEVICES > 3
			case 'r':
				node_move (&device_nodes[3], &open_node);
				break;
#endif
#if MAX_DEVICES > 4
			case 't':
				node_move (&device_nodes[3], &open_node);
				break;
#endif

#ifndef MACHINE_LAUNCH_SWITCH
			case ' ':
				node_move (&open_node, &shooter_node);
				break;
#endif

			case '`':
				/* The tilde toggles between keystrokes being treated as switches,
				and as input into the runtime debugger. */
				simulator_keys ^= 1;
				simlog (SLC_DEBUG, "Input directed to built-in debugger.");
				break;

			case '\x1b':
				sim_exit (0);
				break;

			case 'T':
				task_dump ();
				break;

			case 'S':
				*inbuf = sim_getchar ();
				task_sleep_sec (*inbuf - '0');
				break;

			case '+':
				inbuf[0] = sim_getchar ();
				inbuf[1] = sim_getchar ();

				if (inbuf[0] == 'D')
					sw = inbuf[1] - '1';
				else if (inbuf[0] == 'F')
					sw = (inbuf[1] - '1')
						+ NUM_PF_SWITCHES + NUM_DEDICATED_SWITCHES;
				else
					sw = (inbuf[0] - '1') * 8 + (inbuf[1] - '1');
				sim_switch_depress (sw);
				break;

			case '3':
				sim_switch_depress (SW_LEFT_COIN);
				break;

			case '#':
				/* Treat '#' as a comment until end of line.
				This is useful for creating scripts. */
				do {
					*inbuf = sim_getchar ();
				} while (*inbuf != '\n');
				break;

			case '"':
				simlog (SLC_DEBUG, "next key will toggle, not press");
				toggle_mode = 0;
				break;

			default:
				/* For all other keystrokes, use the keymap table
				to turn the keystroke into a switch trigger. */
				sw = keymaps[(int)*inbuf];
				if (sw)
				{
					if ((switch_table[sw].flags & SW_EDGE) || !toggle_mode)
					{
						simlog (SLC_DEBUG, "switch %d toggled",  sw);
						sim_switch_toggle (sw);
						toggle_mode = 1;
					}
#if (MACHINE_FLIPTRONIC == 1)
					else if (sw >= 72)
					{
						flipper_button_depress (sw);
					}
#endif
					else
					{
						sim_switch_depress (sw);
					}
				}
				else
					simlog (SLC_DEBUG, "invalid key '%c' pressed (0x%02X)",
						*inbuf, *inbuf);
			}
#endif
	}
}


void keyboard_open (const char *filename)
{
	sim_input_fd = open (filename, O_RDONLY);
}


void keyboard_init (void)
{
	task_create_gid_while (GID_LINUX_INTERFACE, sim_interface_thread,
		TASK_DURATION_INF);
}


