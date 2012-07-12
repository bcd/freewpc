/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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
 * A lamplist is an ordered group of lamps.
 * Often, related lamps are modified together; these routines
 * provide the common logic.
 *
 * Some lamplists are also encoded without ordering; these are
 * called lampsets.  A lampset is stored as a bitmap as opposed
 * to a list.
 *
 * A lamplist is the minimum allocation unit for a lamp effect.
 * Lamp effects specify the lamplist that they want to use, and
 * only those lamps are modifiable.
 *
 * Lamplists themselves are declared in the machine configuration file,
 * which is compiled to produce mach-lamplists.c.  Internally, each
 * lamplist is just an array of lamp IDs, terminated by the special
 * value LAMP_END.  Within a lamplist you can also encode "breaks",
 * which separate one lamplist into multiple sections.  This allows
 * an additional delay to be applied.
 */

#include <freewpc.h>

/** A table of pointers to all of the defined lamplists */
extern const lampnum_t *lamplist_table[];


U8 lamplist_alternation_state;


/** Returns true if the current task is a lamp effect. */
static inline bool leff_caller_p (void)
{
	return TRUE;
}


/** Returns true if the given lamp ID value is not an actual lamp,
but a macro. */
static inline bool lamp_macro (lampnum_t entry)
{
	if (entry == LAMP_BREAK)
		return TRUE;
	return FALSE;
}


void lamplist_set_apply_delay (task_ticks_t delay)
{
	if (!leff_caller_p ())
		fatal (ERR_MUST_BE_LEFF);

	(task_current_class_data (leff_data_t))->apply_delay = delay;
}


const lampnum_t *lamplist_lookup (lamplist_id_t id)
{
	const lampnum_t *lptr;
	page_push (MD_PAGE);
	lptr = lamplist_table[id];
	page_pop ();
	return lptr;
}


lampnum_t lamplist_index (lamplist_id_t id, U8 n)
{
	lampnum_t lamp;
	page_push (MD_PAGE);
	lamp = lamplist_table[id][n];
	page_pop ();
	return lamp;
}


const lampnum_t *lamplist_first_entry (lamplist_id_t id)
{
	register const lampnum_t *entry = lamplist_table[id];
	/* We don't check for LAMP_BREAK here, because it is illegal to
	start a list with a break. */
	return entry;
}


const lampnum_t *lamplist_last_entry (lamplist_id_t id)
{
	register const lampnum_t *entry = lamplist_table[id];
	while (entry[1] != LAMP_END)
		entry++;
	while (lamp_macro (*entry))
		entry--;
	return entry;
}


static inline const lampnum_t *
lamplist_previous_entry (lamplist_id_t id, const lampnum_t *entry)
{
	do {
		entry--;
	} while (lamp_macro (*entry));
	return entry;
}


static inline const lampnum_t *
lamplist_next_entry (lamplist_id_t id, const lampnum_t *entry)
{
	do {
		entry++;
	} while (lamp_macro (*entry));
	return entry;
}


static void lamplist_leff_sleep (U8 delay)
{
	if (leff_caller_p () && (delay > 0))
		task_sleep (delay);
}


/** Apply an operator to each element of a lamplist, without executing
any lamp macros. */
void lamplist_apply_nomacro (lamplist_id_t id, lamp_operator_t op)
{
	register const lampnum_t *entry;
	page_push (MD_PAGE);
	for (entry = lamplist_table[id]; *entry != LAMP_END; entry++)
		if (!lamp_macro (*entry))
			(*op) (*entry);
	page_pop ();
}


/** Apply an operator to each element of a lamplist, one by one.
Macros are executed as they are encountered. */
void lamplist_apply (lamplist_id_t id, lamp_operator_t op)
{
	register const lampnum_t *entry;
	U8 lamplist_apply_delay1 = 0;

	page_push (MD_PAGE);

	for (entry = lamplist_table[id]; *entry != LAMP_END; entry++)
	{
		switch (*entry)
		{
			case LAMP_BREAK:
				if (leff_caller_p () && (lamplist_apply_delay != 0))
				{
					lamplist_apply_delay1 = lamplist_apply_delay;
					lamplist_apply_delay = 0;
				}

				if (lamplist_apply_delay1)
					lamplist_leff_sleep (lamplist_apply_delay1);
				break;

			default:
				(*op) (*entry);
				lamplist_leff_sleep (lamplist_apply_delay);
				break;
		}
	}

	if (lamplist_apply_delay1 != 0)
		lamplist_apply_delay = lamplist_apply_delay1;
	page_pop ();
}


/** Returns a pointer to the first element of the lamplist for which the given
 * operator returns TRUE. */
static const lampnum_t *lamplist_find (lamplist_id_t id,
	lamp_boolean_operator_t op)
{
	register const lampnum_t *entry;

	for (entry = lamplist_table[id]; *entry != LAMP_END; entry++)
	{
		if (lamp_macro (*entry))
			continue;
		if (op (*entry))
			break;
	}

	return entry;
}


/** Returns true if all of the lamps in the set return TRUE when the
given operator is applied.  This function will short-circuit as soon
as the result is known. */
bool lamplist_test_all (lamplist_id_t id, lamp_boolean_operator_t op)
{
	register const lampnum_t *entry;
	bool result = TRUE;

	page_push (MD_PAGE);

	for (entry = lamplist_table[id]; *entry != LAMP_END; entry++)
	{
		if (lamp_macro (*entry))
			continue;
		if (!op (*entry))
		{
			result = FALSE;
			break;
		}
	}

	page_pop ();
	return result;
}


/** Returns true if any of the lamps in the set return TRUE when the
given operator is applied.  This function will short-circuit as soon
as the result is known. */
bool lamplist_test_any (lamplist_id_t id, lamp_boolean_operator_t op)
{
	register const lampnum_t *entry;
	bool result = FALSE;

	page_push (MD_PAGE);

	for (entry = lamplist_table[id]; *entry != LAMP_END; entry++)
	{
		if (lamp_macro (*entry))
			continue;
		if (op (*entry))
		{
			result = TRUE;
			break;
		}
	}

	page_pop ();
	return result;
}


static __pure__
lamp_boolean_operator_t matrix_test_operator (const lamp_set matrix)
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
	if (lamplist_alternation_state)
		leff_on (lamp);
	else
		leff_off (lamp);
	lamplist_alternation_state = ~lamplist_alternation_state;
}


void lamplist_apply_leff_alternating (lamplist_id_t id, U8 initially_on)
{
	lamplist_alternation_state = initially_on;
	lamplist_apply (id, lamp_alternating);
}


/* Step functions.  These routines treat the lamplist of length N as
 * an integer in the range of 0 to N-1.  When the 'value' is k, that
 * means the kth lamp is on, and all other lamps are off.
 * The private data field holds the number of the previous lamp.
 */
void lamplist_step_increment (lamplist_id_t set, bitset matrix)
{
	const lampnum_t *entry;

	page_push (MD_PAGE);
	
	/* If all lamps are off, then turn on the first lamp.
	 * Else, find the first lamp that is on, turn it off, then
	 * turn on the next lamp.  If 'next' turns out to be the
	 * end of the lamplist, then wrap around and turn on the
	 * first one. */
	if (bit_test_all_off (matrix))
	{
		entry = lamplist_first_entry (set);
		bit_on (matrix, *entry);
	}
	else
	{
		entry = lamplist_find (set, matrix_test_operator (matrix));
		bit_off (matrix, *entry);
		entry = lamplist_next_entry (set, entry);
		if (*entry == LAMP_END)
			entry = lamplist_first_entry (set);
		bit_on (matrix, *entry);
	}
	page_pop ();
	lamplist_leff_sleep (lamplist_apply_delay);
}


void lamplist_step_decrement (lamplist_id_t set, bitset matrix)
{
	const lampnum_t *entry;

	/* If all lamps are off, then turn on the last lamp.
	 * Else, find the first lamp that is on, turn it off,
	 * then turn on the _previous_ lamp. */

	page_push (MD_PAGE);
	if (bit_test_all_off (matrix))
	{
		entry = lamplist_last_entry (set);
		lamp_on (*entry);
	}
	else
	{
		entry = lamplist_find (set, matrix_test_operator (matrix));
		lamp_off (*entry);
		entry = lamplist_previous_entry (set, entry);
		if (entry < lamplist_first_entry (set))
			entry = lamplist_last_entry (set);
		lamp_on (*entry);
	}
	page_pop ();
	lamplist_leff_sleep (lamplist_apply_delay);
}


/*
 * Build functions.  These routines are similar to the step functions,
 * except that when the value of the lamplist is k, all lamps from 1
 * k are on.
 */
void lamplist_build_increment (lamplist_id_t set, bitset matrix)
{
	const lampnum_t *entry;

	/* Turn on the first lamp that is off, and then stop */
	page_push (MD_PAGE);
	for (entry = lamplist_table[set]; *entry != LAMP_END; entry++)
	{
		if (lamp_macro (*entry))
			continue;
		if (!((matrix_test_operator (matrix)) (*entry)))
		{
			bit_on (matrix, *entry);
			break;
		}
	}
	page_pop ();
}

void lamplist_build_decrement (lamplist_id_t set, bitset matrix)
{
	const lampnum_t *entry;

	/* Going in reverse, turn off the first lamp that is on, and
	 * then stop */
	page_push (MD_PAGE);
	for (entry = lamplist_last_entry (set);
		entry >= lamplist_first_entry (set);
		entry--)
	{
		if (lamp_macro (*entry))
			continue;
		if (((matrix_test_operator (matrix)) (*entry)))
		{
			bit_off (matrix, *entry);
			break;
		}
	}
	page_pop ();
	lamplist_leff_sleep (lamplist_apply_delay);
}


/*
 * Rotate functions.  These routines take the existing lamp values and
 * rotate them either up or down (by which we are referring to the
 * order of the lamps in the array).
 *
 * This function is implemented similar to 'memmove'.
 */
void lamplist_rotate_next (lamplist_id_t set, bitset matrix)
{
	const lampnum_t *entry;
	bool state, newstate;
	lamp_boolean_operator_t test = matrix_test_operator (matrix);

	/* Lamp states rotate up to higher numbers.
	 * L0 = old Ln
	 * L1 = old L0
	 * L2 = old L1
	 * etc.
	 * Ln = old Ln-1
	 */
	page_push (MD_PAGE);
	state = test (*(lamplist_last_entry (set)));
	for (entry = lamplist_table[set]; *entry != LAMP_END; entry++)
	{
		if (lamp_macro (*entry))
			continue;
		newstate = test (*entry);
		(state ? bit_on : bit_off) (matrix, *entry);
		state = newstate;
	}
	page_pop ();
	lamplist_leff_sleep (lamplist_apply_delay);
}


void lamplist_rotate_previous (lamplist_id_t set, bitset matrix)
{
	const lampnum_t *entry, *prev_entry = NULL;
	bool state, first;
	lamp_boolean_operator_t test = matrix_test_operator (matrix);

	/* Lamp states rotate down to lower numbers.
	 * L0 = old L1
	 * L1 = old L2
	 * L2 = old L3
	 * etc.
	 * Ln-1 = old Ln
	 * Ln = old L0
	 */
	page_push (MD_PAGE);
	first = test (*(lamplist_first_entry (set)));
	for (entry = lamplist_table[set]; *entry != LAMP_END; entry++)
	{
		if (lamp_macro (*entry))
			continue;
		if (prev_entry)
		{
			state = test (*entry);
			(state ? bit_on : bit_off) (matrix, *prev_entry);
		}
		prev_entry = entry;
	}

	(first ? bit_on : bit_off) (matrix, *prev_entry);

	page_pop ();
	lamplist_leff_sleep (lamplist_apply_delay);
}

