
#include <freewpc.h>

/* The default lampset for all lamps */
const lampnum_t lampset_all[] = {
	LAMP_MACRO_RANGE (0, NUM_LAMPS - 1),
	LAMP_END,
};

/********** Game specific lamp sets *****************/


const lampnum_t lampset_door_panel_awards[] = {
	LM_PANEL_TSM,
	LM_PANEL_EB,
	LM_PANEL_SUPER_SLOT,
	LM_PANEL_CLOCK_MIL,
	LM_PANEL_SPIRAL,
	LM_PANEL_BATTLE_POWER,
	LM_PANEL_10M,
	LM_PANEL_GREED,
	LM_PANEL_CAMERA,
	LM_PANEL_HH,
	LM_PANEL_CLOCK_CHAOS,
	LM_PANEL_SUPER_SKILL,
	LM_PANEL_FAST_LOCK,
	LM_PANEL_GUMBALL,
	LAMP_END
};

const lampnum_t lampset_door_panels_and_handle[] = {
	LAMP_MACRO_REF(LAMPSET_DOOR_PANELS),
	LM_PANEL_LITZ,
	LAMP_END
};

const lampnum_t lampset_door_locks[] = {
	LM_LOCK1, LM_LOCK2, LAMP_END
};


const lampnum_t lampset_door_gumball[] = {
	LM_GUM, LM_BALL, LAMP_END
};


const lampnum_t lampset_left_ramp_awards[] = {
	LM_BONUS_X,
	LM_MULTIBALL,
	LM_SUPER_SKILL,
	LAMP_END
};

const lampnum_t lampset_lock_awards[] = {
	LM_LOCK_ARROW,
	LM_LOCK_EB,
	LAMP_END
};

const lampnum_t lampset_piano_awards[] = {
	LM_PIANO_PANEL,
	LM_PIANO_JACKPOT,
	LAMP_END
};

const lampnum_t lampset_greed_targets[] = {
	LM_LL_5M,
	LM_ML_5M,
	LM_UL_5M,
	LM_UR_5M,
	LM_MR2_5M,
	LM_MR1_5M,
	LM_LR_5M,
	LAMP_END
};

const lampnum_t lampset_powerfield_values[] = {
	LM_MPF_500K,
	LM_MPF_750K,
	LM_MPF_1M,
	LAMP_END
};

const lampnum_t lampset_jets[] = {
	LM_LEFT_JET,
	LM_RIGHT_JET,
	LM_LOWER_JET,
	LAMP_END
};


const lampnum_t lampset_lower_lanes[] = {
	LM_LEFT_OUTLANE,
	LM_LEFT_INLANE1,
	LM_LEFT_INLANE2,
	LM_RIGHT_INLANE,
	LM_RIGHT_OUTLANE,
	LAMP_END
};


const lampnum_t lampset_spiral_awards[] = {
	LM_SPIRAL_2M,
	LM_SPIRAL_3M,
	LM_SPIRAL_4M,
	LM_SPIRAL_5M,
	LM_SPIRAL_10M,
	LM_SPIRAL_EB,
	LAMP_END
};

const lampnum_t lampset_amode_all[] = {
	LAMP_MACRO_REF(LAMPSET_DOOR_PANELS_AND_HANDLE),
	LAMP_MACRO_REF(LAMPSET_DOOR_LOCKS),
	LAMP_MACRO_REF(LAMPSET_DOOR_GUMBALL),
	LAMP_MACRO_REF(LAMPSET_SPIRAL_AWARDS),
	LAMP_MACRO_REF(LAMPSET_LOWER_LANES),
	LAMP_MACRO_REF(LAMPSET_JETS),
	LAMP_MACRO_REF(LAMPSET_POWERFIELD_VALUES),
	LM_LEFT_SPIRAL,
	LM_LEFT_POWERBALL,
	LAMP_MACRO_REF(LAMPSET_LEFT_RAMP_AWARDS),
	LM_RAMP_BATTLE,
	LAMP_MACRO_REF(LAMPSET_LOCK_AWARDS),
	LAMP_MACRO_REF(LAMPSET_PIANO_AWARDS),
	LM_RIGHT_SPIRAL,
	LM_RIGHT_POWERBALL,
	LAMP_MACRO_REF(LAMPSET_GREED_TARGETS),
	LAMP_END
};


/*
 * Lampsets are statically registered in the following table.
 */
const lampnum_t *lampset_table[] = {
	[LAMPSET_ALL] = lampset_all,
	[LAMPSET_DOOR_PANELS] = lampset_door_panel_awards,
	[LAMPSET_DOOR_PANELS_AND_HANDLE] = lampset_door_panels_and_handle,
	[LAMPSET_LEFT_RAMP_AWARDS] = lampset_left_ramp_awards,
	[LAMPSET_LOCK_AWARDS] = lampset_lock_awards,
	[LAMPSET_PIANO_AWARDS] = lampset_piano_awards,
	[LAMPSET_GREED_TARGETS] = lampset_greed_targets,
	[LAMPSET_POWERFIELD_VALUES] = lampset_powerfield_values,
	[LAMPSET_JETS] = lampset_jets,
	[LAMPSET_LOWER_LANES] = lampset_lower_lanes,
	[LAMPSET_SPIRAL_AWARDS] = lampset_spiral_awards,
	[LAMPSET_DOOR_LOCKS] = lampset_door_locks,
	[LAMPSET_DOOR_GUMBALL] = lampset_door_gumball,
	[LAMPSET_AMODE_ALL] = lampset_amode_all,
};


__fastram__ task_ticks_t lampset_apply_delay;

__fastram__ U8 lampset_alternation_state;

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
#pragma long_branch
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

			case LAMP_MACRO_REF_OP:
				lset_stack[lset_stack_offset++] = lset+1;
				lset = lampset_table[lset[0]];
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
#pragma short_branch

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


