/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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
 *
 * A lampset is the minimum allocation unit for a lamp effect
 * Lamp effects specify the lampset that they want to use.
 *
 * Lampset themselves are declared in the machine configuration file,
 * which is compiled to produce mach-lampsets.c.  Internally, each
 * lampset is just an array of lamp IDs, terminated by the special
 * value LAMP_END.
 */

#include <freewpc.h>

/** A table of pointers to all of the defined lampsets */
extern const lampnum_t *lampset_table[];


U8 lampset_alternation_state;


/** Returns true if the current task is a lamp effect. */
static inline bool leff_caller_p (void)
{
	return ((task_getgid () == GID_LEFF) || (task_getgid () == GID_SHARED_LEFF));
}


/** Returns true if the given lamp ID value is not an actual lamp,
but a macro. */
static inline bool lampset_macro_entry (lampnum_t entry)
{
	return (entry == LAMP_MACRO_SLEEP_OP) ? TRUE : FALSE;
}


void lampset_set_apply_delay (task_ticks_t delay)
{
	if (!leff_caller_p ())
		fatal (ERR_MUST_BE_LEFF);

	(task_current_class_data (leff_data_t))->apply_delay = delay;
}


const lampnum_t *lampset_lookup (lampset_id_t id)
{
	wpc_push_page (MD_PAGE);
	return lampset_table[id];
	wpc_pop_page ();
}


const lampnum_t *lampset_first_entry (lampset_id_t id)
{
	register const lampnum_t *entry = lampset_table[id];
	while (lampset_macro_entry (*entry))
		entry++;
	return entry;
}


const lampnum_t *lampset_last_entry (lampset_id_t id)
{
	register const lampnum_t *entry = lampset_table[id];
	while (entry[1] != LAMP_END)
		entry++;
	while (lampset_macro_entry (*entry))
		entry--;
	return entry;
}


static inline const lampnum_t *
lampset_previous_entry (lampset_id_t id, const lampnum_t *entry)
{
	do {
		entry--;
	} while (lampset_macro_entry (*entry));
	return entry;
}


static inline const lampnum_t *
lampset_next_entry (lampset_id_t id, const lampnum_t *entry)
{
	do {
		entry++;
	} while (lampset_macro_entry (*entry));
	return entry;
}


static void lampset_leff_sleep (U8 delay)
{
	if (leff_caller_p () && (delay > 0))
		task_sleep (delay);
}


/** Apply an operator to each element of a lampset, without executing
any lamp macros. */
void lampset_apply_nomacro (lampset_id_t id, lamp_operator_t op)
{
	register const lampnum_t *entry;
	wpc_push_page (MD_PAGE);
	for (entry = lampset_table[id]; *entry != LAMP_END; entry++)
		if (!lampset_macro_entry (*entry))
			(*op) (*entry);
	wpc_pop_page ();
}


/** Apply an operator to each element of a lampset, one by one.
Macros are executed as they are encountered. */
void lampset_apply (lampset_id_t id, lamp_operator_t op)
{
	register const lampnum_t *entry;
	U8 lampset_apply_delay1 = 0;

	wpc_push_page (MD_PAGE);

	for (entry = lampset_table[id]; *entry != LAMP_END; entry++)
	{
		switch (*entry)
		{
			case LAMP_MACRO_SLEEP_OP:
				if (leff_caller_p () && (lampset_apply_delay != 0))
				{
					lampset_apply_delay1 = lampset_apply_delay;
					lampset_apply_delay = 0;
				}

				if (lampset_apply_delay1)
					lampset_leff_sleep (lampset_apply_delay1);
				break;

			default:
				(*op) (*entry);
				lampset_leff_sleep (lampset_apply_delay);
				break;
		}
	}

	if (lampset_apply_delay1 != 0)
		lampset_apply_delay = lampset_apply_delay1;
	wpc_pop_page ();
}


/** Returns a pointer to the first element of the lampset for which the given
 * operator returns TRUE. */
static const lampnum_t *lampset_find (lampset_id_t id,
	lamp_boolean_operator_t op)
{
	register const lampnum_t *entry;

	for (entry = lampset_table[id]; *entry != LAMP_END; entry++)
	{
		if (lampset_macro_entry (*entry))
			continue;
		if (op (*entry))
			break;
	}

	return entry;
}


/** Returns true if all of the lamps in the set return TRUE when the
given operator is applied.  This function will short-circuit as soon
as the result is known. */
bool lampset_test_all (lampset_id_t id, lamp_boolean_operator_t op)
{
	register const lampnum_t *entry;
	bool result = TRUE;

	wpc_push_page (MD_PAGE);

	for (entry = lampset_table[id]; *entry != LAMP_END; entry++)
	{
		if (lampset_macro_entry (*entry))
			continue;
		if (!op (*entry))
		{
			result = FALSE;
			break;
		}
	}

	wpc_pop_page ();
	return result;
}


/** Returns true if any of the lamps in the set return TRUE when the
given operator is applied.  This function will short-circuit as soon
as the result is known. */
bool lampset_test_any (lampset_id_t id, lamp_boolean_operator_t op)
{
	register const lampnum_t *entry;
	bool result = FALSE;

	wpc_push_page (MD_PAGE);

	for (entry = lampset_table[id]; *entry != LAMP_END; entry++)
	{
		if (lampset_macro_entry (*entry))
			continue;
		if (op (*entry))
		{
			result = TRUE;
			break;
		}
	}

	wpc_pop_page ();
	return result;
}


static __attribute__((pure))
lamp_boolean_operator_t matrix_test_operator (const bitset matrix)
{
	if (matrix == lamp_matrix)
	{
		return lamp_test;
	}
	else if (matrix == lamp_flash_matrix)
	{
		return lamp_flash_test;
	}
	else
	{
		return leff_test;
	}
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
void lampset_step_increment (lampset_id_t set, bitset matrix)
{
	const lampnum_t *entry;

	wpc_push_page (MD_PAGE);
	
	/* If all lamps are off, then turn on the first lamp.
	 * Else, find the first lamp that is on, turn it off, then
	 * turn on the next lamp.  If 'next' turns out to be the
	 * end of the lampset, then wrap around and turn on the
	 * first one. */
	if (bit_test_all_off (matrix))
	{
		entry = lampset_first_entry (set);
		bit_on (matrix, *entry);
	}
	else
	{
		entry = lampset_find (set, matrix_test_operator (matrix));
		bit_off (matrix, *entry);
		entry = lampset_next_entry (set, entry);
		if (*entry == LAMP_END)
			entry = lampset_first_entry (set);
		bit_on (matrix, *entry);
	}
	wpc_pop_page ();
	lampset_leff_sleep (lampset_apply_delay);
}


void lampset_step_decrement (lampset_id_t set, bitset matrix)
{
	const lampnum_t *entry;

	/* If all lamps are off, then turn on the last lamp.
	 * Else, find the first lamp that is on, turn it off,
	 * then turn on the _previous_ lamp. */

	wpc_push_page (MD_PAGE);
	if (bit_test_all_off (matrix))
	{
		entry = lampset_last_entry (set);
		lamp_on (*entry);
	}
	else
	{
		entry = lampset_find (set, matrix_test_operator (matrix));
		lamp_off (*entry);
		entry = lampset_previous_entry (set, entry);
		if (entry < lampset_first_entry (set))
			entry = lampset_last_entry (set);
		lamp_on (*entry);
	}
	wpc_pop_page ();
	lampset_leff_sleep (lampset_apply_delay);
}


/*
 * Build functions.  These routines are similar to the step functions,
 * except that when the value of the lampset is k, all lamps from 1
 * k are on.
 */
void lampset_build_increment (lampset_id_t set, bitset matrix)
{
	const lampnum_t *entry;

	/* Turn on the first lamp that is off, and then stop */
	wpc_push_page (MD_PAGE);
	for (entry = lampset_table[set]; *entry != LAMP_END; entry++)
	{
		if (lampset_macro_entry (*entry))
			continue;
		if (!((matrix_test_operator (matrix)) (*entry)))
		{
			bit_on (matrix, *entry);
			break;
		}
	}
	wpc_pop_page ();
}

void lampset_build_decrement (lampset_id_t set, bitset matrix)
{
	const lampnum_t *entry;

	/* Going in reverse, turn off the first lamp that is on, and
	 * then stop */
	wpc_push_page (MD_PAGE);
	for (entry = lampset_last_entry (set);
		entry >= lampset_first_entry (set);
		entry--)
	{
		if (lampset_macro_entry (*entry))
			continue;
		if (((matrix_test_operator (matrix)) (*entry)))
		{
			bit_off (matrix, *entry);
			break;
		}
	}
	wpc_pop_page ();
	lampset_leff_sleep (lampset_apply_delay);
}


/*
 * Rotate functions.  These routines take the existing lamp values and
 * rotate them either up or down (by which we are referring to the
 * order of the lamps in the array).
 *
 * This function is implemented similar to 'memmove'.
 */
void lampset_rotate_next (lampset_id_t set, bitset matrix)
{
	const lampnum_t *entry;
	bool state, newstate;

	/* Lamp states rotate up to higher numbers.
	 * L0 = old Ln
	 * L1 = old L0
	 * L2 = old L1
	 * etc.
	 * Ln = old Ln-1
	 */
	wpc_push_page (MD_PAGE);
	state = lamp_test (*(lampset_last_entry (set)));
	for (entry = lampset_table[set]; *entry != LAMP_END; entry++)
	{
		if (lampset_macro_entry (*entry))
			continue;
		newstate = ((matrix_test_operator (matrix)) (*entry));
		(state ? lamp_on : lamp_off) (*entry);
		state = newstate;
	}
	wpc_pop_page ();
	lampset_leff_sleep (lampset_apply_delay);
}


void lampset_rotate_previous (lampset_id_t set, bitset matrix)
{
	const lampnum_t *entry, *prev_entry = NULL;
	bool state;

	/* Lamp states rotate down to lower numbers.
	 * L0 = old L1
	 * L1 = old L2
	 * L2 = old L3
	 * etc.
	 * Ln-1 = old Ln
	 * Ln = old L0
	 */
	wpc_push_page (MD_PAGE);
	for (entry = lampset_table[set]; *entry != LAMP_END; entry++)
	{
		if (lampset_macro_entry (*entry))
			continue;
		if (prev_entry)
		{
			state = ((matrix_test_operator (matrix)) (*entry));
			(state ? lamp_on : lamp_off) (*prev_entry);
			prev_entry = entry;
		}
	}

	entry = lampset_first_entry (set);
	state = ((matrix_test_operator (matrix)) (*entry));
	entry = lampset_last_entry (set);
	(state ? lamp_on : lamp_off) (*entry);

	wpc_pop_page ();
	lampset_leff_sleep (lampset_apply_delay);
}

