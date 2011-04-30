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

#ifndef _HWSIM_SIGNAL_H
#define _HWSIM_SIGNAL_H

/*	Each hardware signal that is simulated has a unique "signal number",
	which is given in this enum.  Groups of related signals have
	consecutive values, then only the first signal in the group is named
	here.

	We define a signal for any value that we might want to monitor,
	even if it is not directly driven by the CPU.  For example, the
	wall voltages, the generated voltages, blanking, etc.

	The maximum number of hardware signals defined this way is MAX_SIGNALS.

	We also support 'automatic signals', which are computed on the fly rather
	than stored in memory.  Those have a signal number above MAX_SIGNALS.
	See signo.c for more information.
*/
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


/*	The signal module implements scoping of the signals much like a logic
	analyzer.  You can provide an expression that can act as a trigger
	to begin tracing.

	Signal expressions are parsed and stored internally as a tree of
	'struct signal_expression' objects.  The 'op' field, of type
	'signal_operator', defines the type of the expression and determines
	how its child nodes are interpreted.
 */
enum signal_operator
{
	/* Leaf expressions */
	SIG_SIGNO,
	SIG_TIME,
	SIG_TIMEDIFF,
	SIG_CONST,

	/* Expressions with two child nodes */
	SIG_BINARY,
	SIG_EQ,
	SIG_AND,
	SIG_OR,
	SIG_LAST_BINARY,

	/* Expressions with one child node */
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

#define expr_binary_p(ex)   (ex->op > SIG_BINARY && ex->op < SIG_LAST_BINARY)
struct signal_expression *expr_alloc (void);
void expr_free (struct signal_expression *ex);

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


#endif /* _HWSIM_SIGNAL_H */
