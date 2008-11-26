/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

/**
 * The signal module allows 'scoping' of binary I/O signals
 * and writing the results to a file which can be converted into
 * a waveform.
 */

#define MAX_SIGNALS 1024
#define MAX_READINGS 256

typedef struct signal_readings
{
	unsigned int init_state;
	unsigned int count;
	unsigned int prev_state;
	simulated_time_interval_t t[MAX_READINGS];
	struct signal_readings *next;
} signal_readings_t;


/**
 * An array of readings, indexed by signal number.
 * Each entry is a linked list of 256-reading chunks;
 */
signal_readings_t *signal_readings[MAX_SIGNALS] = { NULL, };

/**
 * The last known value of each signal.  This does not
 * provide any history.
 */
uint32_t signal_states[MAX_SIGNALS / 32] = { 0, };


signal_readings_t *signal_chunk_alloc (void)
{
	signal_readings_t *sigrd;
	sigrd = malloc (sizeof (signal_readings_t));
	sigrd->init_state = 0;
	sigrd->count = 0;
	sigrd->prev_state = 0;
	sigrd->next = NULL;
	return sigrd;
}


void signal_update (signal_number_t signo, unsigned int state)
{
	signal_readings_t *sigrd;
	extern unsigned long sim_jiffies;

	/* Update last state */
	if (state)
		signal_states[signo / 32] &= ~(1 << (signo % 32));
	else
		signal_states[signo / 32] |= (1 << (signo % 32));

	/* Don't do anything else if we're not tracing this signal. */
	sigrd = signal_readings[signo];
	if (!sigrd)
		return;

	/* Move to the last block in the list. */
	while (sigrd->next)
		sigrd = sigrd->next;

	/* Normalize state to 0/1 */
	state = !!state;

	/* Don't do anything if the state hasn't changed. */
	if (sigrd->prev_state == state)
		return;

	/* Allocate a new block if the previous block is full. */
	if (sigrd->count == MAX_READINGS)
	{
		signal_readings_t *new_sigrd;
		new_sigrd = signal_chunk_alloc ();
		new_sigrd->init_state = sigrd->prev_state;
		sigrd = sigrd->next = new_sigrd;
	}

	/* Save the new state along with the timestamp of the change */
	simlog (SLC_DEBUG, "Signal %d now %d at %ld", signo, state, sim_jiffies);
	sigrd->t[sigrd->count++] = sim_jiffies;
	sigrd->prev_state = state;
}


void signal_trace_start (signal_number_t signo)
{
	if (!signal_readings[signo])
		signal_readings[signo] = signal_chunk_alloc ();
}


void signal_trace_stop (signal_number_t signo)
{
	signal_readings_t *sigrd = signal_readings[signo];
	if (!sigrd)
		return;

	simlog (SLC_DEBUG, "Trace for signal %d:", signo);
	simlog (SLC_DEBUG, "  Start at %d", sigrd->init_state);
	unsigned int last_time = sigrd->t[0];
	while (sigrd)
	{
		signal_readings_t *next = sigrd->next;
		int n;
		unsigned int state = sigrd->init_state;
		for (n = 0; n < sigrd->count; n++)
		{
			state = !state;
			simlog (SLC_DEBUG, "  Set to %d at time %ld (%d)", state, sigrd->t[n],
				sigrd->t[n] - last_time);
			last_time = sigrd->t[n];
		}
		free (sigrd);
		sigrd = next;
	}
	signal_readings[signo] = NULL;
}


void signal_init (void)
{
}
