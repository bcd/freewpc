/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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
 * \brief Routines for working with groups of related lamps
 *
 * A lampset is a descriptor that represents a set of lamps.
 * Often, related lamps are modified together; these routines
 * provide the common logic.
 */

#include <freewpc.h>

extern const lampnum_t *lampset_table[];

U8 lampset_alternation_state;

static inline void lampset_invoke_operator (
	lampnum_t lamp, lamp_operator_t op )
{
	(*op) (lamp);

	if (lampset_apply_delay > 0)
		task_sleep (lampset_apply_delay);
}


void lampset_set_apply_delay (task_ticks_t delay)
{
	task_set_thread_data (task_getpid (), L_PRIV_APPLY_DELAY, delay);
}


const U8 *lampset_lookup (lampset_id_t id)
{
	return lampset_table[id];
}


/* Apply an operator to each element of a lampset, one by one */
void lampset_apply (lampset_id_t id, lamp_operator_t op)
{
	register uint8_t opcode;
	register const lampnum_t *lset = lampset_table[id];
	U8 lampset_intermediate_delay = 0;

	while ((opcode = *lset++) != LAMP_END)
	{
		switch (opcode)
		{
			case LAMP_MACRO_SLEEP_OP:
				if (lampset_intermediate_delay == 0)
				{
					lampset_intermediate_delay = lampset_apply_delay;
					task_set_thread_data (task_getpid (), L_PRIV_APPLY_DELAY, 0);
				}
				task_sleep (lampset_intermediate_delay);
				break;

			default:
				lampset_invoke_operator (opcode, op);
				break;
		}
	}
}

/* Common uses of apply */
void lampset_apply_on (lampset_id_t id)
{
	lampset_apply (id, lamp_on);
}

void lampset_apply_off (lampset_id_t id)
{
	lampset_apply (id, lamp_off);
}

void lampset_apply_toggle (lampset_id_t id)
{
	lampset_apply (id, lamp_toggle);
}



void lampset_apply_leff_on (lampset_id_t id)
{
	lampset_apply (id, leff_on);
}

void lampset_apply_leff_off (lampset_id_t id)
{
	lampset_apply (id, leff_off);
}

void lampset_apply_leff_toggle (lampset_id_t id)
{
	lampset_apply (id, leff_toggle);
}

void lamp_alternating (lampnum_t lamp)
{
	if (lampset_alternation_state)
		leff_on (lamp);
	else
		leff_off (lamp);
	lampset_alternation_state = ~lampset_alternation_state;
}


void lampset_apply_leff_alternating (lampset_id_t id, U8 initially_on)
{
	lampset_alternation_state = initially_on;
	lampset_apply (id, lamp_alternating);
}

/* Step functions.  These routines treat the lampset of length N as
 * an integer in the range of 0 to N-1.  When the 'value' is k, that
 * means the kth lamp is on, and all other lamps are off.
 * The private data field holds the number of the previous lamp.
 */
void lampset_step_increment_handler (lampnum_t lamp)
{
	/* Find the first lamp that is on; turn it off, and turn the
	 * next lamp in the sequence on */
	if (lamp_test (lamp))
	{
		lamp_off (lamp);
		lamp_on (task_get_thread_data (task_getpid (), L_PRIV_DATA));
	}

	task_set_thread_data (task_getpid (), L_PRIV_DATA, lamp);
}

void lampset_leff_step_increment_handler (lampnum_t lamp)
{
	U8 prev = task_get_thread_data (task_getpid (), L_PRIV_DATA);

	dbprintf ("lamp = %d, prev = %d\n", lamp, prev);
	if (prev != 0)
		leff_off (prev);
	leff_on (lamp);
	task_set_thread_data (task_getpid (), L_PRIV_DATA, lamp);
}


void lampset_step_increment (lampset_id_t id)
{
	lampset_apply (id, lampset_step_increment_handler);
}

void lampset_leff_step_increment (lampset_id_t id)
{
	lampset_apply (id, lampset_leff_step_increment_handler);
}


void lampset_step_decrement_handler (lampnum_t lamp)
{
	/* Find the first lamp that is on; turn it off, and turn the
	 * previous lamp in the sequence on */
	if (task_get_thread_data (task_getpid (), L_PRIV_DATA))
	{
		lamp_on (lamp);
		task_set_thread_data (task_getpid (), L_PRIV_DATA, 0);
	}
	else if (lamp_test (lamp))
	{
		lamp_off (lamp);
		task_set_thread_data (task_getpid (), L_PRIV_DATA, 1);
	}
}


void lampset_step_decrement (lampset_id_t id)
{
	lampset_apply (id, lampset_step_decrement_handler);
}


/*
 * Build functions.  These routines are similar to the step functions,
 * except that when the value of the lampset is k, all lamps from 1
 * k are on.
 */
void lampset_build_increment (lampset_id_t id)
{
	/* Turn on the first lamp that is off, and then stop */
}

void lampset_build_decrement (lampset_id_t id)
{
	/* Going in reverse, turn off the first lamp that is on, and
	 * then stop */
}


/*
 * Rotate functions.  These routines take the existing lamp values and
 * rotate them either up or down (by which we are referring to the
 * order of the lamps in the array).
 *
 * This function is implemented similar to 'memmove'.
 */
void lampset_rotate_next (lampset_id_t id)
{
}

void lampset_rotate_previous (lampset_id_t id)
{
}

