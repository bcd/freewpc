
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

/* The default lampset for all lamps */
#define LMSET_ALL_LAMPS		LAMP_MACRO_RANGE (0, NUM_LAMPS - 1)


/* The list of all system-defined lampsets */
#define SYSTEM_LAMPSETS \
	DECL_LAMPSET (LAMPSET_ALL, LMSET_ALL_LAMPS)

/*
 * Declare the lampset arrays.  Here, the actual storage space is
 * defined.  Each lampset appears as an array of lamp values, terminated
 * by the special value LAMP_END.
 */

#undef DECL_LAMPSET
#define DECL_LAMPSET(idx, lampdata)	DECL_LAMPSET_INSTANCE(idx, lampdata)

SYSTEM_LAMPSETS
#ifdef MACHINE_LAMPSETS
MACHINE_LAMPSETS
#endif

/*
 * Declare the lampset table.  This is an array of pointers to each of
 * the lampsets defined above.  Lampsets can be referred to by index
 * instead of pointer this way.
 */

#undef DECL_LAMPSET
#define DECL_LAMPSET(idx, lampdata)	DECL_LAMPSET_TABLE_ENTRY(idx, lampdata)

const lampnum_t *lampset_table[] = {
	SYSTEM_LAMPSETS
#ifdef MACHINE_LAMPSETS
	MACHINE_LAMPSETS
#endif
};


task_ticks_t lampset_apply_delay;

U8 lampset_alternation_state;

/* During a lampset_apply operation, this variable contains the
 * index of the lamp that is currently being processed.
 * The apply callbacks can scan this value to control the actions.
 *
 * Note once we support multiple leffs running simultaneously, this
 * will have to be changed.
 */
U8 lampset_apply_count;

U8 lampset_private_data;


static inline void lampset_invoke_operator (
	lampnum_t lamp, lamp_operator_t op )
{
	(*op) (lamp);
	lampset_apply_count++;
	if (lampset_apply_delay > 0)
		task_sleep (lampset_apply_delay);
}


void lampset_set_apply_delay (task_ticks_t delay)
{
	lampset_apply_delay = delay;
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
	static const lampnum_t *lset_stack[4];
	int lset_stack_offset = 0;

	lset_stack[lset_stack_offset++] = 0;
	lampset_apply_count = 0;
	lampset_private_data = 0;

	while (lset)
	{
		opcode = *lset++;

		switch (opcode)
		{
			case LAMP_MACRO_RANGE_OP:
				/* TODO: lampset ranges should be deprecated.
				 * These should be expanded at compile time. */
				for (opcode = lset[0]; opcode < lset[1]; opcode++)
					lampset_invoke_operator (opcode, op);
				lset += 2;
				break;

			case LAMP_MACRO_SLEEP_OP:
				task_sleep (*lset++);
				break;

			case LAMP_END:
				lset = lset_stack[--lset_stack_offset];
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
		lamp_on (lampset_private_data);
	}

	lampset_private_data = lamp;
}

void lampset_step_increment (lampset_id_t id)
{
	lampset_apply (id, lampset_step_increment_handler);
}



void lampset_step_decrement_handler (lampnum_t lamp)
{
	/* Find the first lamp that is on; turn it off, and turn the
	 * previous lamp in the sequence on */
	if (lampset_private_data)
	{
		lamp_on (lamp);
		lampset_private_data = 0;
	}
	else if (lamp_test (lamp))
	{
		lamp_off (lamp);
		lampset_private_data = 1;
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

