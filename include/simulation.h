/*
 * Copyright 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SIMULATION_H
#define _SIMULATION_H


/** The number of IRQs per second. */ 
#define IRQS_PER_SEC 1024

/** The frequency of the realtime thread, in milliseconds */
#define RT_THREAD_FREQ 50

/** The number of IRQs that need to be asserted on every
 * iteration of the realtime thread. */
#define RT_ITERATION_IRQS ((IRQS_PER_SEC * RT_THREAD_FREQ) / 1000)

/** A simulation log class.  All output from the simulator is
categorized into one of the following classes; the output is then
preceded with a class identifier.  This makes the output consumable
by other utilities, which can filter the stream for specific content. */
enum sim_log_class
{
	/** Debug information from the simulator itself */
	SLC_DEBUG,
	/** Debug information written by the game ROM to the debugger */
	SLC_DEBUG_PORT,
	SLC_SOUNDCALL,
};


/** The callback type for time handlers.  A time handler is a function
that is invoked after a certain amount of real time has expired, in the
context of the simulation and not the running system. */
typedef void (*time_handler_t) (void *);

typedef unsigned long simulated_time_interval_t;

/** An instance of a time handler */
struct time_handler
{
	struct time_handler *next;
	int periodicity;
	time_handler_t fn;
	void *data;
};


/* Don't use this function inside the simulator! */
#define switch_poll_logical dont_use_switch_poll_logical


void simlog (enum sim_log_class class, const char *format, ...);

void ui_init (void);
void ui_write_debug (enum sim_log_class c, const char *format, va_list ap);
void ui_write_solenoid (int, int);
void ui_write_lamp (int, int);
void ui_write_triac (int, int);
void ui_write_switch (int, int);
void ui_write_sound_call (unsigned int x);
void ui_write_dmd_text (int x, int y, const char *text);
void ui_clear_dmd_text (int n);
void ui_update_ball_tracker (unsigned int ballno, unsigned int location);
void ui_exit (void);


void sim_switch_effects (int swno);

void sim_time_register (int n_ticks, int periodic_p, time_handler_t fn, void *data);
void sim_time_step (void);

void sim_watchdog_reset (void);
void sim_watchdog_init (void);


typedef enum
{
	SIGNO_SOL=0,
	SIGNO_TRIAC=100,
	SIGNO_LAMP=200,
	SIGNO_ZEROCROSS=300,
	SIGNO_DIAG_LED=301,
	SIGNO_IRQ=302,
	SIGNO_FIRQ=303,
	SIGNO_RESET=304,
	SIGNO_BLANKING=305,
	SIGNO_5V=306,
	SIGNO_12V=307,
	SIGNO_18V=308,
	SIGNO_20V=309,
	SIGNO_50V=310,
	SIGNO_COINDOOR_INTERLOCK=311,
	SIGNO_JUMPERS=350,
	SIGNO_SWITCH=400,
} signal_number_t;

void signal_update (signal_number_t signo, unsigned int state);
void signal_init (void);

/* For simulation of broken hardware.  Each of these bit indicates an
induced error condition that is created by the simulator, to see how
the software handles it. */
#define SIM_BAD_NOZEROCROSS   0x2
#define SIM_BAD_NOOPTOPOWER   0x4
#define SIM_BAD_NOSWITCHPOWER 0x8


extern inline bool sim_test_badness (unsigned int err)
{
	extern unsigned long sim_badness;
	return sim_badness & err;
}

extern inline void sim_set_badness (unsigned long err)
{
	extern unsigned long sim_badness;
	sim_badness |= err;
}

extern inline void sim_clear_badness (unsigned long err)
{
	extern unsigned long sim_badness;
	sim_badness &= ~err;
}

/** The maximum number of balls that can be tracked in simulation */
#define SIM_MAX_BALLS 8

#define MAX_BALL_LOCATIONS 128

#define SIM_LOCATION_NONE 0
#define SIM_NO_BALL_HERE -1


#endif /* _SIMULATION_H */
