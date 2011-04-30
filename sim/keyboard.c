/*
 * Copyright 2010-2011 by Brian Dominy <brian@oddchange.com>
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

#define KEY_SW 0x1
#define KEY_NODE 0x2
#define KEY_TOGGLE 0x4
#define KEY_ACTION 0x8
#define KEY_SHOOTER 0x10
#define KEY_NODE_FROM 0x10
#define KEY_SHOT 0x20

/* Describes how a keystroke is converted into a playfield event */
struct key_binding
{
	/* One or more KEY_xxx defines (see above).
	A few flags with specific meanings:
	KEY_SHOT says that this action is a feasible shot from a flipper.
	It can be used to simulate game play by automatically triggering
	actions rather than requiring lots of keystrokes. */
	unsigned int flags;

	/* When KEY_NODE is set, says that the key causes a ball
	movement to/from this node.  By default, the movement is from
	the playfield TO the node.  If KEY_NODE_FROM is also set,
	the movement is to the playfield FROM this node. */
	struct ball_node *node;

	/* When KEY_SW is set, says that the key causes a simple switch
	activation event on this switch.  KEY_NODE is preferable as it
	simulates much better, but this was the older method used. */
	switchnum_t sw;

	/* When KEY_ACTION is set, causes a user-defined handler to be
	invoked instead of the KEY_NODE or KEY_SW builtin actions. */
	void (*action) (struct key_binding *);
};

static struct key_binding keymaps[128] = {};

/** A dummy function intended to be used for debugging under GDB.
 * When you want to break at a future time, but not necessarily at a
 * particular place, set a breakpoint here.  Then press the 'C' key
 * to halt the system.
 */
void gdb_break (void)
{
	barrier ();
}

/** Attach a keystroke to a switch. */
void sim_key_install (char key, unsigned int swno)
{
	struct key_binding *kb = &keymaps[(int)key];
	kb->sw = swno;
	kb->flags |= KEY_SW;
	if (switch_table[kb->sw].flags & SW_PLAYFIELD)
	{
		kb->node = switch_nodes + swno;
		kb->flags |= KEY_NODE;
	}
}

void sim_key_install_node (char key, struct ball_node *node)
{
	struct key_binding *kb = &keymaps[(int)key];
	kb->node = node;
	kb->flags |= KEY_NODE;
}


void sim_key_install_shooter (char key)
{
	struct key_binding *kb = &keymaps[(int)key];
	kb->flags &= ~(KEY_SW | KEY_NODE);
	kb->flags |= KEY_SHOOTER;
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
	struct key_binding *kb;
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
		are fed into the simulated serial port... meaning it is interpreted
		by the game program itself, and not the simulator.  Use the
		tilde to toggle between the two modes. */
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
			/* Carriage returns and line feeds are ignored so that you can
			put these commands into a script file. */
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
				kb = &keymaps[(int)*inbuf];
#ifdef MACHINE_SHOOTER_SWITCH
				if (kb->flags & KEY_SHOOTER)
				{
					node_kick (&shooter_node);
				}
				else
#endif
				if (kb->flags & KEY_NODE)
				{
					node_move (kb->node, &open_node);
				}
				else if (kb->flags & KEY_SW)
				{
					if ((switch_table[kb->sw].flags & SW_EDGE) || !toggle_mode)
					{
						simlog (SLC_DEBUG, "switch %d toggled", kb->sw);
						sim_switch_toggle (kb->sw);
						toggle_mode = 1;
					}
#if (MACHINE_FLIPTRONIC == 1)
					else if (kb->sw >= 72)
					{
						flipper_button_depress (kb->sw);
					}
#endif
					else
					{
						sim_switch_depress (kb->sw);
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

	/* Install platform-specific key bindings */
#ifdef MACHINE_START_SWITCH
	sim_key_install ('1', MACHINE_START_SWITCH);
#endif
#ifdef MACHINE_BUYIN_SWITCH
	sim_key_install ('2', MACHINE_BUYIN_SWITCH);
#endif
	sim_key_install ('3', SW_LEFT_COIN);
	sim_key_install ('4', SW_CENTER_COIN);
	sim_key_install ('5', SW_RIGHT_COIN);
	sim_key_install ('6', SW_FOURTH_COIN);
	sim_key_install ('7', SW_ESCAPE);
	sim_key_install ('8', SW_DOWN);
	sim_key_install ('9', SW_UP);
	sim_key_install ('0', SW_ENTER);
	sim_key_install (',', SW_LEFT_BUTTON);
	sim_key_install ('.', SW_RIGHT_BUTTON);
#ifdef SW_COIN_DOOR_CLOSED
	sim_key_install ('-', SW_COIN_DOOR_CLOSED);
#endif
#ifdef MACHINE_TILT_SWITCH
	sim_key_install ('T', MACHINE_TILT_SWITCH);
#endif
#ifdef MACHINE_SLAM_TILT_SWITCH
	sim_key_install ('!', MACHINE_SLAM_TILT_SWITCH);
#endif
#ifdef MACHINE_LAUNCH_SWITCH
	sim_key_install (' ', MACHINE_LAUNCH_SWITCH);
#else
	sim_key_install_shooter (' ');
#endif
#if MAX_DEVICES > 1
	sim_key_install_node ('w', &device_nodes[1]);
#endif
#if MAX_DEVICES > 2
	sim_key_install_node ('e', &device_nodes[2]);
#endif
#if MAX_DEVICES > 3
	sim_key_install_node ('r', &device_nodes[3]);
#endif
#if MAX_DEVICES > 4
	sim_key_install_node ('t', &device_nodes[4]);
#endif

}

