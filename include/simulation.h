/*
 * Copyright 2007-2010 by Brian Dominy <brian@oddchange.com>
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

#include <stdint.h>
#include "hwsim/signal.h"
#include "hwsim/ball.h"


/*****
 *****	Timing control
 *****/

/** The number of IRQs per second. */ 
#define IRQS_PER_SEC 1024

/** The frequency of the realtime thread, in milliseconds */
#define RT_THREAD_FREQ 50

/** The number of IRQs that need to be asserted on every
 * iteration of the realtime thread. */
#define RT_ITERATION_IRQS ((IRQS_PER_SEC * RT_THREAD_FREQ) / 1000)

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

void sim_time_register (int n_ticks, int periodic_p, time_handler_t fn, void *data);
void sim_time_step (void);
unsigned long realtime_read (void);
unsigned int sim_get_wall_clock (void);


/*****
 *****	Simulator logging
 *****/

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
};

void simlog (enum sim_log_class class, const char *format, ...);


#include "hwsim/ui.h"

void wpc_key_press (char);

typedef void (*mux_ui) (int, int);

void mux_write (mux_ui ui_update, int index, unsigned char *memp, unsigned char newval, unsigned int sigbase);

/*****
 *****	Watchdog mechanism
 *****/

void sim_watchdog_reset (void);
void sim_watchdog_init (void);

/* Don't use this function inside the simulator! */
#define switch_poll_logical dont_use_switch_poll_logical

void sim_switch_effects (int swno);
unsigned char *sim_switch_matrix_get (void);
void sim_switch_toggle (int sw);
void sim_switch_set (int sw, int on);
void sim_switch_depress (int sw);
void flipper_button_depress (int sw);
int sim_switch_read (int sw);
void sim_switch_init (void);

void exec_script (char *cmd);
void exec_script_file (const char *filename);

void conf_add (const char *name, int *valp);
int conf_read (const char *name);
void conf_write (const char *name, int val);
void conf_push (int val);
void conf_pop (unsigned int count);
int conf_read_stack (int offset);

void asciidmd_map_page (int mapping, int page);
void asciidmd_refresh (void);
void asciidmd_set_visible (int page);
void asciidmd_init (void);

void sim_coil_init (void);
void sim_coil_change (unsigned int coil, unsigned int on);
bool sim_coil_is_active (unsigned int coil);
void diverter_coil_init (unsigned int id, struct ball_node *node);
void coil_clone (unsigned int parent_id, unsigned int child_id);

unsigned char hwtimer_read (void);
void hwtimer_write (unsigned char val);

void sim_zc_init (void);
int sim_zc_read (void);

void sim_key_install (char key, unsigned int swno);
void keyboard_open (const char *filename);
void keyboard_init (void);

void protected_memory_load (void);
void protected_memory_save (void);

void mach_node_init (void);

#endif /* _SIMULATION_H */
