/*
 * Copyright 2008-2011 by Brian Dominy <brian@oddchange.com>
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
#include <stdint.h>
#include <math.h>


/**
 * The signal module allows 'scoping' of binary I/O signals
 * and writing the results to a file which can be converted into
 * a waveform.
 */

#define MAX_READINGS 256
#define MAX_CAPTURES 16
#define MAX_EXPR 8

/**
 * A structure for tracking a single binary signal over a period
 * of time.
 *
 * When a signal is traced for a long period of time, and the
 * array of readings becomes full, then another one of these
 * objects is created and they are all chained together.
 * Thus, MAX_READINGS is not a true maximum, but simply a way
 * of not overallocating memory for short captures.
 */
typedef struct signal_readings
{
	unsigned int init_state;
	unsigned int count;
	unsigned int prev_state;
	simulated_time_interval_t t[MAX_READINGS];
	struct signal_readings *next;
} signal_readings_t;


/**
 * An array of physical readings, indexed by signal number.
 * Each entry is a linked list of 256-reading chunks;
 * TBD - should auto signals not be represented here?
 */
signal_readings_t *signal_readings[MAX_SIGNALS] = { NULL, };

/**
 * The last known value of each signal.  This does not
 * provide any history.  Each signal is stored as a single bit.
 */
uint32_t signal_states[(MAX_SIGNALS + 31) / 32] = { 0, };


/** A list of the signals currently being captured */
uint32_t signals_being_captured[MAX_CAPTURES];

/** The output file when capturing is enabled */
FILE *signal_capture_file;

signal_expression_t *signal_start_expr, *signal_stop_expr;

int signal_capture_active = 0;

uint64_t signal_trace_start_time;


double signal_value (uint32_t signo);


#ifdef CONFIG_AC
double signal_ac_angle_value (uint32_t offset)
{
	extern double sim_zc_angle (void);
	return sim_zc_angle ();
}
#endif

double signal_sol_voltage_value (uint32_t offset)
{
	return signal_value (SIGNO_SOL+offset) *
		fabs (signal_value (SIGNO_AC_ANGLE));
}


/**
 * The table of auto signal types.  Each type of auto-signal allows
 * for 256 instances of it.
 */
const value_signal signal_value_table[] = {
	/* SIGNO_SOL_VOLTAGE+n gives the current voltage level of solenoid N,
	accounting for the fact that it is AC voltage.  If the solenoid is off,
	then the value is always 0.0.  If it is on, then it will range from -1.0
	to 1.0, depending on the current phase of the AC cycle. */
	signal_sol_voltage_value,

#ifdef CONFIG_AC
	/* SIGNO_AC_ANGLE is similar, but just returns the -1.0 to 1.0 value
	that represents the phase angle, without consideration for any particular
	solenoid line. */
	signal_ac_angle_value,
#endif
};


/**
 * Return the value of a signal.  If it is a binary signal, then the value is always
 * 0 or 1.  However, we also support "auto signals", whose values are computed from
 * other signals.  The signal number encodes the computing function (type) and
 * instance number (offset).  The computation is done on the fly.
 */
double signal_value (uint32_t signo)
{
	if (signo >= SIGNO_FIRST_AUTO)
	{
		return signal_value_table[autosig_type (signo)] (autosig_offset (signo));
	}
	else
	{
		int state = signal_states[signo / 32] & (1 << (signo % 32));
		return 1.0 * !state;
	}
}


/**
 * Allocate a new signal expression tree node.
 */
struct signal_expression *expr_alloc (void)
{
	struct signal_expression *ex = malloc (sizeof (struct signal_expression));
	memset (ex, 0, sizeof (*ex));
	return ex;
}


/**
 * Free (recursively) a new signal expression tree node.
 */
void expr_free (struct signal_expression *ex)
{
	if (!ex)
		return;
	if (expr_binary_p (ex))
	{
		expr_free (ex->u.binary.left);
		expr_free (ex->u.binary.right);
	}
	free (ex);
}



/**
 * Allocate a new signal chunk to hold some more readings for
 * a signal.
 */
static signal_readings_t *signal_chunk_alloc (void)
{
	signal_readings_t *sigrd;
	sigrd = malloc (sizeof (signal_readings_t));
	sigrd->init_state = 0;
	sigrd->count = 0;
	sigrd->prev_state = 0;
	sigrd->next = NULL;
	return sigrd;
}


/**
 * Evaluate a signal expression.  The script parser converts the textual
 * expression into a parse tree, passed in EX.  SIG_CHANGED says whether or
 * not the current signal being scanned changed during the last time step.
 */
bool signal_expr_eval (unsigned int sig_changed, struct signal_expression *ex)
{
	if (!ex)
		return FALSE;

	switch (ex->op)
	{
		default:
			return FALSE;

		/* A signal name without any qualification is true when its value
		changes */
		case SIG_SIGNO:
			if (ex->u.signo == sig_changed)
				return TRUE;
			break;

		case SIG_TIMEDIFF:
			if (realtime_read () >= signal_trace_start_time + ex->u.value)
				return TRUE;
			break;

		case SIG_TIME:
			if (realtime_read () >= ex->u.value)
				return TRUE;
			break;

		/* Indicates a "<signal> is <value>" expression.  True if
		the signal has the exact value */
		case SIG_EQ:
			if (ex->u.binary.left->u.signo == sig_changed
				&& signal_readings[sig_changed]->prev_state ==
					ex->u.binary.right->u.value)
				return TRUE;
			break;

		case SIG_AND:
			if (signal_expr_eval (sig_changed, ex->u.binary.left)
				&& signal_expr_eval (sig_changed, ex->u.binary.right))
				return TRUE;
			break;

		case SIG_OR:
			if (signal_expr_eval (sig_changed, ex->u.binary.left)
				|| signal_expr_eval (sig_changed, ex->u.binary.right))
				return TRUE;
			break;

		case SIG_NOT:
			if (!signal_expr_eval (sig_changed, ex->u.unary))
				return TRUE;
			break;
	}
	return FALSE;
}


/**
 * Write the header to the capture file.  This is called once when
 * the file is created.
 */
void signal_write_header (void)
{
	int sigin;
	fprintf (signal_capture_file, "# Time");
	for (sigin = 0; sigin < MAX_CAPTURES; sigin++)
	{
		uint32_t signo = signals_being_captured[sigin];
		if (signo)
			fprintf (signal_capture_file, " %u", signo);
	}
	fprintf (signal_capture_file, "\n");
}


/**
 * Write the current values of all signals to the capture file.
 */
void signal_write (void)
{
	int sigin;
	fprintf (signal_capture_file, "%lu", realtime_read ());
	for (sigin = 0; sigin < MAX_CAPTURES; sigin++)
	{
		uint32_t signo = signals_being_captured[sigin];
		if (signo)
		{
			double state = signal_value (signo);
			fprintf (signal_capture_file, " %g", state);
		}
	}
	fprintf (signal_capture_file, "\n");
}


/**
 * Update the state of a binary signal.  SIGNO says which signal,
 * STATE is a zero/nonzero value reflecting its binary state.
 *
 * If a signal is known not to have changed state, it is not necessary
 * to call this function repeatedly; however it is harmless to do so.
 */
void signal_update (signal_number_t signo, unsigned int state)
{
	signal_readings_t *sigrd;

	/* Update last state */
	if (state)
		signal_states[signo / 32] &= ~(1 << (signo % 32));
	else
		signal_states[signo / 32] |= (1 << (signo % 32));

	/* Don't do anything else if we're not tracing this signal. */
	sigrd = signal_readings[signo];
	if (!sigrd)
		goto do_capture;

	/* Move to the last block in the list of signal data. */
	while (sigrd->next)
		sigrd = sigrd->next;

	/* Normalize state to 0/1 */
	state = !!state;

	/* Don't do anything if the state hasn't changed. */
	if (sigrd->prev_state == state)
		return;

	/* See what time the signal last changed state.  By comparing this
	to the current time, we can say how long it held its last value. */
#if 0
	simulated_time_interval_t last_change_time;
	if (sigrd->count == 0)
		last_change_time = 0;
	else
		last_change_time = sigrd->t[sigrd->count - 1];
	/* Print the last signal state */
	simlog (SLC_DEBUG, "Signo(%d) was %s for %ldms", signo, !state ? "high" : "low",
		realtime_read () - last_change_time);
#endif

	/* Allocate a new block if the previous block is full. */
	if (sigrd->count == MAX_READINGS)
	{
		signal_readings_t *new_sigrd;
		new_sigrd = signal_chunk_alloc ();
		new_sigrd->init_state = sigrd->prev_state;
		sigrd = sigrd->next = new_sigrd;
	}


	/* Save the new state along with the timestamp of the change */
	sigrd->t[sigrd->count++] = realtime_read ();
	sigrd->prev_state = state;

do_capture:
	if (signal_capture_active && signal_capture_file)
	{
		/* Also see if tracing should stop */
		if (signal_stop_expr && signal_expr_eval (signo, signal_stop_expr))
		{
			simlog (SLC_DEBUG, "Capture complete.");
			fflush (signal_capture_file);
			signal_capture_active = 0;
		}
	}
	else if (signal_start_expr && signal_expr_eval (signo, signal_start_expr))
	{
		/* Otherwise, should capture start now because we meet the start
		condition? */
		simlog (SLC_DEBUG, "Capture started.");
		signal_capture_active = 1;
		signal_trace_start_time = realtime_read ();
		signal_write_header ();
		signal_write ();
	}
}


/**
 * Set a start condition.  When this condition becomes true, capture will begin.
 * If NULL, then capture is disabled.
 */
void signal_capture_start (struct signal_expression *ex)
{
	if (signal_start_expr)
		expr_free (signal_start_expr);
	signal_start_expr = ex;
	simlog (SLC_DEBUG, "Capture start condition set. %p", signal_start_expr);
}


/**
 * Set a stop condition.  When this condition becomes true, capture that has
 * already started will then stop.  If NULL, then once started, a capture
 * will run forever.
 */
void signal_capture_stop (struct signal_expression *ex)
{
	if (signal_stop_expr)
		expr_free (signal_stop_expr);
	simlog (SLC_DEBUG, "Capture stop condition set.");
	signal_stop_expr = ex;
}


/**
 * Add a signal to the capture list.
 */
void signal_capture_add (uint32_t signo)
{
	int sigin;
	for (sigin = 0; sigin < MAX_CAPTURES; sigin++)
	{
		if (signals_being_captured[sigin] == 0)
		{
			simlog (SLC_DEBUG, "Signal %d added to capture (#%d).", signo, sigin);
			signals_being_captured[sigin] = signo;
			signal_readings[signo] = signal_chunk_alloc ();
			return;
		}
	}
}


/**
 * Delete a signal to the capture list.
 */
void signal_capture_del (uint32_t signo)
{
	int sigin;
	for (sigin = 0; sigin < MAX_CAPTURES; sigin++)
	{
		if (signals_being_captured[sigin] == signo)
		{
			simlog (SLC_DEBUG, "Signal %d removed from capture (#%d).", signo, sigin);
			signals_being_captured[sigin] = 0;
			signal_readings[signo] = NULL;
			return;
		}
	}
}

/**
 * The periodic handler runs every millisecond.  If a capture is active,
 * it writes out the values of all signals being traced in this time step.
 * If a stop expression has been defined, then it also checks to see if
 * tracing should finish.
 */
static void signal_trace_periodic (void *data __attribute__((unused)))
{
	if (signal_capture_active)
	{
		signal_write ();
		if (signal_stop_expr && signal_expr_eval (0, signal_stop_expr))
		{
			simlog (SLC_DEBUG, "Capture complete in periodic.");
			fflush (signal_capture_file);
			signal_capture_active = 0;
		}
	}
}


/**
 * Set the filename used to capture signal traces.  If filename is
 * NULL, then the capture file is flushed and closed.
 */
void signal_capture_set_file (const char *filename)
{
	if (filename)
	{
		signal_capture_file = fopen (filename, "w");
	}
	else if (signal_capture_file)
	{
		fclose (signal_capture_file);
	}
}


/**
 * Enable tracing for a signal.
 */
void signal_trace_start (signal_number_t signo)
{
	if (!signal_readings[signo])
		signal_readings[signo] = signal_chunk_alloc ();
}


/**
 * Disable tracing for a signal.
 */
void signal_trace_stop (signal_number_t signo)
{
	signal_readings_t *sigrd = signal_readings[signo];
	if (!sigrd)
		return;

	simlog (SLC_DEBUG, "Trace for signal %d:", signo);
	unsigned int last_time = sigrd->t[0];
	while (sigrd)
	{
		signal_readings_t *next = sigrd->next;
		int n;
		unsigned int state = sigrd->init_state;
		for (n = 0; n < sigrd->count; n++)
		{
			simlog (SLC_DEBUG, "  %d for %dms", state, sigrd->t[n] - last_time);
			state = !state;
			last_time = sigrd->t[n];
		}
		free (sigrd);
		sigrd = next;
	}
	signal_readings[signo] = NULL;
}


/**
 * Initializing the signal tracking module.
 */
void signal_init (void)
{
	signal_start_expr = signal_stop_expr = NULL;
	signal_capture_active = 0;
	sim_time_register (1, TRUE, signal_trace_periodic, NULL);
}

