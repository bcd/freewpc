/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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
 * \brief Entry point to the builtin simulator for the WPC hardware.
 *
 * When CONFIG_NATIVE is defined at build time, access to WPC I/O registers
 * is redirected to function calls here that simulate the behavior.  This
 * allows FreeWPC to be tested directly on a Linux or Windows development machine,
 * even when there is no PinMAME.
 *
 * If CONFIG_UI is also defined, then various activities are displayed to the
 * user.  Several different UIs are available.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdarg.h>
#include <freewpc.h>
#include <simulation.h>
#include <hwsim/io.h>

extern void do_firq (void);
extern void do_irq (void);
extern void exit (int);


/** The actual time at which the simulation was started */
static time_t sim_boot_time;

/** The rate at which the simulated clock should run */
int linux_irq_multiplier = 1;

/** True if the IRQ is enabled */
bool linux_irq_enable;

/** True if the FIRQ is enabled */
bool linux_firq_enable;

/** When nonzero, the system is held in reset afer power on.  This lets
you fire up gdb and debug the early initialization.  From the debugger,
you should clear this flag, e.g. "set sim_debug_init 0".  You set the
flag by passing the --debuginit command-line option. */
static volatile int sim_debug_init = 0;

/** The initial number of balls to 'install' as given on the command-line. */
#ifdef DEVNO_TROUGH
int sim_installed_balls = MACHINE_TROUGH_SIZE;
#else
int sim_installed_balls = 0;
#endif

/** The stream to write to for output */
FILE *sim_output_stream;

unsigned int signo_under_trace = SIGNO_SOL + 0;

const char *exec_file = NULL;

int exec_late_flag = 0;

int crash_on_error = 0;


/** Prints log messages, requested status, etc. to the console.
 * This is the only function that should use printf.
 */
void simlog (enum sim_log_class class, const char *format, ...)
{
	va_list ap;
	FILE *ofp;
	char buf[256];

	/* Get all of the arguments and format into a single buffer */
	va_start (ap, format);
	vsprintf (buf, format, ap);
	va_end (ap);

#ifdef CONFIG_UI
	ui_write_debug (class, buf);

	if (sim_output_stream == stdout)
		ofp = NULL;
	else
		ofp = sim_output_stream;

#else
	ofp = sim_output_stream;
#endif

	if (ofp)
	{
		if (class != SLC_DEBUG_PORT)
			fprintf (ofp, "[SIM] ");
#ifdef CONFIG_SIM_TIMESTAMP
		fprintf (ofp, "(%08ld) ", realtime_read ());
#endif
		fprintf (ofp, "%s", buf);
		fputc ('\n', ofp);
		fflush (ofp);
	}

}


/*	Called to shutdown the simulation.
	This performs all cleanup before exiting back to the native OS. */
__noreturn__ void sim_exit (U8 error_code)
{
	simlog (SLC_DEBUG, "Shutting down simulation.");
	protected_memory_save ();
#ifdef CONFIG_UI
	ui_exit ();
#endif
	if (crash_on_error && error_code)
		*(int *)0 = 1;
	exit (error_code);
}



/** Realtime callback function.
 *
 * This event simulates an elapsed 1ms.
 */
CALLSET_ENTRY (native, realtime_tick)
{
#define FIRQ_FREQ 8
#define PERIODIC_FREQ 16

	static unsigned long next_firq_time = FIRQ_FREQ;
	static unsigned long next_periodic_time = PERIODIC_FREQ;

	/* Update all of the simulator modules that need periodic processing */
	sim_time_step ();

	/* Simulate an IRQ every 1ms */
	if (linux_irq_enable)
		tick_driver ();

	/* Simulate an FIRQ every 8ms */
	if (linux_firq_enable)
	{
		while (realtime_read () >= next_firq_time)
		{
			do_firq ();
			next_firq_time += FIRQ_FREQ;
		}
	}

	/* Call periodic processes every 16ms */
	if (realtime_read () >= next_periodic_time)
	{
		db_periodic ();
		if (likely (periodic_ok))
			do_periodic ();
		next_periodic_time += PERIODIC_FREQ;
	}
}


/**
 * Return the current wall clock time in minutes.
 */
unsigned int
sim_get_wall_clock (void)
{
	time_t now = time (NULL);
	return ((now - sim_boot_time) * linux_irq_multiplier) / 60;
}


/** Initialize the Linux simulation.
 *
 * This is called during normal initialization, during the hardware
 * bringup.  This performs final initialization before the system
 * is ready.
 */
void sim_init (void)
{
	void realtime_loop (void);

	/* This is done here, because the task subsystem isn't ready
	inside main () */
	task_create_gid_while (GID_LINUX_REALTIME, realtime_loop, TASK_DURATION_INF);

	/* Initialize the keyboard handler */
	keyboard_init ();

	/* Initial the trough to contain all the balls.  By default,
	 * it will fill the trough, based on its actual size.  You
	 * can use the --balls option to override this. */
	node_init ();
}


/** Entry point into the simulation.
 *
 * This function substitutes for the reset vector being thrown on actual
 * hardware.
 */
int main (int argc, char *argv[])
{
	int argn = 1;

	/* Parse command-line arguments */
	sim_output_stream = stdout;

	while (argn < argc)
	{
		const char *arg = argv[argn++];
		if (!strcmp (arg, "-h"))
		{
			printf ("Syntax: freewpc [<options>]\n");
			printf ("-f <file>           Read input commands from file (default : stdin)\n");
			printf ("-o <file>           Log debug messages to file (default : stdout)\n");
			printf ("--debuginit         Wait for GDB attach during init (default: no)\n");
			printf ("--exec <file>       Read script commands from file\n");
			exit (0);
		}
		else if (!strcmp (arg, "-f"))
		{
			keyboard_open (argv[argn++]);
		}
		else if (!strcmp (arg, "-o"))
		{
			sim_output_stream = fopen (argv[argn++], "w");
			if (sim_output_stream == NULL)
			{
				printf ("Error: could not open log file\n");
				exit (1);
			}
		}
		else if (!strcmp (arg, "--debuginit"))
		{
			sim_debug_init = 1;
		}
		else if (!strcmp (arg, "--exec"))
		{
			exec_file = argv[argn++];
		}
		else if (!strcmp (arg, "--late"))
		{
			exec_late_flag = 1;
		}
		else if (!strcmp (arg, "--error-crash"))
		{
			crash_on_error = 1;
		}
		else if (strchr (arg, '='))
		{
			char varval[64];
			unsigned int val;
			char *s;

			strcpy (varval, arg);
			s = strchr (varval, '=');
			*s = '\0';
			val = strtoul (s+1, NULL, 0);
			//conf_set_later (varval, val);
		}
		else
		{
			printf ("invalid argument %s\n", arg);
			exit (1);
		}
	}

#ifdef CONFIG_UI
	/* Initialize the user interface */
#ifdef CONFIG_GTK
	gtk_init (&argc, &argv);
#endif
	ui_init ();
#endif

	/* Initialize signal tracker */
	signal_init ();

	/** Do initialization that the hardware would normally do before
	 * the reset vector is invoked. */
	signal_update (SIGNO_RESET, 1);
	disable_interrupts ();
	sim_zc_init ();
#if (MACHINE_PIC == 1)
	simulation_pic_init ();
#endif
	sim_watchdog_init ();
#if (MACHINE_DMD == 1)
	asciidmd_init ();
#endif

	/* Initialize the I/O */
	io_init ();

	/* Set the hardware registers to their initial values. */
	writeb (WPC_LAMP_COL_STROBE, 0);
#if !(MACHINE_PIC == 1)
	writeb (WPC_SW_COL_STROBE, 0);
#endif
#if (MACHINE_DMD == 1)
	writeb (WPC_DMD_LOW_PAGE, 0);
	writeb (WPC_DMD_HIGH_PAGE, 0);
	writeb (WPC_DMD_ACTIVE_PAGE, 0);
#endif

	/* Initialize the state of the switches; optos are backwards */
	sim_switch_init ();

	/* Force always closed */
#ifdef SW_ALWAYS_CLOSED
	sim_switch_toggle (SW_ALWAYS_CLOSED);
#endif

	/* Close the coin door */
#ifdef SW_COIN_DOOR_CLOSED
	sim_switch_toggle (SW_COIN_DOOR_CLOSED);
#endif

	/* Load the protected memory area */
	protected_memory_load ();

	/* Initialize the simulated ball tracker */
	sim_coil_init ();

	/* Invoke the machine-specific simulation function */
#ifdef CONFIG_MACHINE_SIMULATOR
	(*CONFIG_MACHINE_SIMULATOR) ();
#endif

	/* Jump to the reset function */
	while (sim_debug_init)
		usleep (10000);

	signal_update (SIGNO_RESET, 0);
	signal_update (SIGNO_BLANKING, 1);
	signal_update (SIGNO_5V, 1);
	signal_update (SIGNO_12V, 1);
	signal_update (SIGNO_18V, 1);
	signal_update (SIGNO_20V, 1);
	signal_update (SIGNO_50V, 1);

	/* Create more conf knobs */
	conf_add ("balls", &sim_installed_balls);
	conf_add ("sim.speed", &linux_irq_multiplier);

	/* Execute default script file.  First, load any global
	configuration in freewpc.conf.  Then, try to load a
	game-specific file, based on its shortname.  Last,
	execute any file provided on the command line. */
	exec_script_file ("conf/freewpc.conf");
	exec_script_file ("conf/" MACHINE_SHORTNAME ".conf");
	if (exec_file && !exec_late_flag)
		exec_script_file (exec_file);

	/* Save the time at which the simulation started, to implement
	the wall clock */
	sim_boot_time = time (NULL);

	/* Now start the pinball OS! */
	freewpc_init ();
	return 0;
}

