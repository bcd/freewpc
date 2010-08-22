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
void ui_write_sound_reset (void);
void ui_write_sound_command (unsigned int x);
void ui_write_dmd_text (int x, int y, const char *text);
void ui_clear_dmd_text (int n);
void ui_update_ball_tracker (unsigned int ballno, const char *location);
void ui_exit (void);


void sim_switch_effects (int swno);

void sim_time_register (int n_ticks, int periodic_p, time_handler_t fn, void *data);
void sim_time_step (void);

void sim_watchdog_reset (void);
void sim_watchdog_init (void);


typedef enum
{
	SIGNO_NONE=0,
	SIGNO_TRIAC=0x10,
	SIGNO_JUMPERS=0x20,
	SIGNO_LAMP=0x80,
	SIGNO_SWITCH=0x100,
	SIGNO_SOL=0x180,
	SIGNO_ZEROCROSS=0x1C0,
	SIGNO_DIAG_LED,
	SIGNO_IRQ, SIGNO_FIRQ,
	SIGNO_RESET, SIGNO_BLANKING,
	SIGNO_5V, SIGNO_12V, SIGNO_18V, SIGNO_20V, SIGNO_50V,
	SIGNO_COINDOOR_INTERLOCK,
	MAX_SIGNALS,

	SIGNO_FIRST_AUTO=0x1000,
	SIGNO_SOL_VOLTAGE=0x1000,
	SIGNO_AC_ANGLE=0x1100,
} signal_number_t;

enum signal_operator
{
	SIG_SIGNO,
	SIG_TIME,
	SIG_TIMEDIFF,
	SIG_CONST,

	SIG_BINARY,
	SIG_EQ,
	SIG_AND,
	SIG_OR,
	SIG_LAST_BINARY,

	SIG_UNARY,
	SIG_NOT,
	SIG_LAST_UNARY,
};

typedef struct signal_expression
{
	enum signal_operator op;
	union {
		uint32_t signo;
		uint32_t value;
		uint64_t timer;
		struct {
			struct signal_expression *left, *right;
		} binary;
		struct signal_expression *unary;
	} u;
} signal_expression_t;


typedef double (*value_signal) (uint32_t offset);

#define autosig_type(signo)	((signo - SIGNO_FIRST_AUTO) / 0x100)
#define autosig_offset(signo) ((signo - SIGNO_FIRST_AUTO) % 0x100)

extern unsigned int signo_under_trace;

void signal_update (signal_number_t signo, unsigned int state);
void signal_init (void);
void signal_capture_start (struct signal_expression *ex);
void signal_capture_stop (struct signal_expression *ex);
void signal_capture_add (uint32_t signo);
void signal_capture_del (uint32_t signo);
void signal_capture_set_file (const char *filename);
void signal_trace_start (signal_number_t signo);
void signal_trace_stop (signal_number_t signo);


/** The maximum number of balls that can be tracked in simulation */
#define SIM_MAX_BALLS 8

#define MAX_BALL_LOCATIONS 128

#define SIM_LOCATION_NONE 0
#define SIM_NO_BALL_HERE -1

#define expr_binary_p(ex)   (ex->op > SIG_BINARY && ex->op < SIG_LAST_BINARY)
struct signal_expression *expr_alloc (void);
void expr_free (struct signal_expression *ex);

void exec_script_file (const char *filename);

unsigned long realtime_read (void);

unsigned char *sim_switch_matrix_get (void);
void sim_switch_toggle (int sw);
void sim_switch_set (int sw, int on);
void sim_switch_depress (int sw);
int sim_switch_read (int sw);
void sim_switch_init (void);

void conf_add (const char *name, int *valp);
int conf_read (const char *name);
void conf_write (const char *name, int val);
void conf_push (int val);
void conf_pop (unsigned int count);
int conf_read_stack (int offset);

/*************************
 * Ball tracking
 *************************/

struct ball;
struct ball_node;

/*	The node type is used to subclass a node's behavior. */
struct ball_node_type
{
	void (*insert) (struct ball_node *node, struct ball *ball);
	void (*remove) (struct ball_node *node, struct ball *ball);
};

#define MAX_BALLS_PER_NODE 8

/* A node reflects a position on the playfield where a pinball
	may rest indefinitely.  Each node implements a first-in
	first-out queue of ball objects, defined below. */
struct ball_node
{
	/* A pointer to the next node, which is the default location
	that a kick operation will move a ball to. */
	struct ball_node *next;

	/* Likewise, pointer to the previous node.  Note that now,
	there can only be 1 previous node. */
	struct ball_node *prev;

	/* The maximum queue depth here; how many balls can stack up.
	When this limit is reached, nodes that feed into this will
	begin to back up. */
	unsigned int size;

	/* The actual number of balls here now */
	unsigned int count;

	/* Pointers to the ball objects that are stored here */
	struct ball *ball_queue[MAX_BALLS_PER_NODE];

	/* The offset in the queue of the next ball to be kicked */
	unsigned int head;

	/* The type (subclass) structure for this node */
	struct ball_node_type *type;

	/* A type-dependent value */
	unsigned int index;

	/* The time delay before a ball transitions to the next node.
	When a kick occurs, the ball is removed from the node immediately,
	but does not appear in the next node until this delay completes. */
	unsigned int delay;

	/* If nonzero, the node is 'unlocked', and balls will transition
	to the next node automatically */
	unsigned int unlocked;

	/* The name of the node used for debugging */
	const char *name;
};


/* A ball object represents a physical pinball, and tracks its
	whereabouts through the machine. */
struct ball
{
	struct ball_node *node;
	unsigned int pos;
	unsigned int flags;
	unsigned int index;
	char name[8];
};

/* Some common nodes present on most machines */
extern struct ball_node open_node;
extern struct ball_node device_nodes[];
extern struct ball_node shooter_node;
extern struct ball_node outhole_node;
#define trough_node device_nodes[DEVNO_TROUGH]

void device_node_kick (unsigned int devno);
void node_kick (struct ball_node *node);
void node_move (struct ball_node *dst, struct ball_node *src);

#endif /* _SIMULATION_H */
