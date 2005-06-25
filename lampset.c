
#include <freewpc.h>

/* A lamp operator is any routine which abides to the following
 * prototype for modifying a simple lamp.  Standard routines
 * like lamp_on, lamp_off are lamp operators. */
typedef void (*lamp_operator_t) (lampnum_t);

/* A lampset is a sequence of lamp numbers, some of which are
 * "immediate lamp values", others are "lamp value macros".
 */
typedef const lampnum_t lampset_t[];

/* Lampsets are identified by small integers */
typedef uint8_t lampset_id_t;

/*
 * Lamp macros are lampset members which calculate actual
 * lamp values at runtime.
 *
 * Macro values start above the range of acutal lamp numbers.
 */

#define LAMP_MACRO_RANGE_OP		(NUM_LAMPS + 1)
#define LAMP_MACRO_REF_OP			(NUM_LAMPS + 2)
#define LAMP_MACRO_REVERSE_OP		(NUM_LAMPS + 3)

/*
 * When defining a lampset, you can use the following C macros
 * to emit proper lamp macro code into the array.
 */
#define LAMP_MACRO_RANGE(p,q) \
	LAMP_MACRO_RANGE_OP, (p), (q)

#define LAMP_MACRO_REF(id) \
	LAMP_MACRO_REF_OP, (id)

#define LAMP_MACRO_REVERSE_REF(id) \
	LAMP_MACRO_REF_OP, (id)

/*
 * The list of lampset IDs (this is mostly game-specific)
 */
#define LAMPSET_ALL		0

/* The default lampset for all lamps */
lampset_t lampset_all = {
	LAMP_MACRO_RANGE (0, NUM_LAMPS - 1),
};


/*
 * Lampsets are statically registered in the following table.
 */
lampset_t *lampset_table[] = {
	[LAMPSET_ALL] = lampset_all,
};

/* Apply an operator to each element of a lampset, one by one */
void lampset_apply (lampset_id_t id, lamp_operator_t op)
{
}

/* Common uses of apply */
void lampset_apply_on (lampset_id_t id)
{
}

void lampset_apply_off (lampset_id_t id)
{
}

void lampset_apply_toggle (lampset_id_t id)
{
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

