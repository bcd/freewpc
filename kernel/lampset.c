
#include <freewpc.h>

/* The default lampset for all lamps */
const lampnum_t lampset_all[] = {
	LAMP_MACRO_RANGE (0, NUM_LAMPS - 1),
	LAMP_END,
};


/*
 * Lampsets are statically registered in the following table.
 */
const lampnum_t *lampset_table[] = {
	[LAMPSET_ALL] = lampset_all,
};


__fastram__ task_ticks_t lampset_apply_delay;


static inline void lampset_invoke_operator (
	lampnum_t lamp, lamp_operator_t op )
{
	(*op) (lamp);
	if (lampset_apply_delay > 0)
		task_sleep (lampset_apply_delay);
}


void lampset_set_apply_delay (task_ticks_t delay)
{
	lampset_apply_delay = delay;
}


/* Apply an operator to each element of a lampset, one by one */
void lampset_apply (lampset_id_t id, lamp_operator_t op)
{
	register uint8_t opcode;
	register const lampnum_t *lset = lampset_table[id];

	while ((opcode = *lset++) != LAMP_END)
	{
		switch (opcode)
		{
			case LAMP_MACRO_RANGE_OP:
				for (opcode = lset[0]; opcode < lset[1]; opcode++)
					lampset_invoke_operator (opcode, op);
				lset += 2;
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

/* Step functions.  These routines treat the lampset of length N as
 * an integer in the range of 0 to N-1.  When the 'value' is k, that
 * means the kth lamp is on, and all other lamps are off.
 */
void lampset_step_increment (lampset_id_t id)
{
}

void lampset_step_decrement (lampset_id_t id)
{
}

/*
 * Build functions.  These routines are similar to the step functions,
 * except that when the value of the lampset is k, all lamps from 1
 * k are on.
 */
void lampset_build_increment (lampset_id_t id)
{
}

void lampset_build_decrement (lampset_id_t id)
{
}


/*
 * Rotate functions.  These routines take the existing lamp values and
 * rotate them either up or down (by which we are referring to the
 * order of the lamps in the array).
 */
void lampset_rotate_next (lampset_id_t id)
{
}

void lampset_rotate_previous (lampset_id_t id)
{
}


