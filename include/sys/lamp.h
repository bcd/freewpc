#ifndef _SYS_LAMP_H
#define _SYS_LAMP_H

#define NUM_LAMPS 64

#define NUM_LAMP_COLS	8

#define MAKE_LAMP(col,row)	(((col-1) * 8) + row-1)

/* The lamp flash state is updated every 32ms, so this will
 * toggle the lamp flash state about 8 times/sec */
#define LAMP_DEFAULT_FLASH_RATE 4


/* Lamp effect opcodes
 */
#define LAMP_OP_SKIP_RANGE	0xFD
#define LAMP_OP_RANGE		0xFE
#define LAMP_OP_EXIT			0xFF


/* Small integer type for a lamp number */
typedef uint8_t lampnum_t;

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
#define LAMP_END_OP					(NUM_LAMPS + 4)


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

#define LAMP_END LAMP_END_OP


void lamp_init (void);
void lamp_flash_rtt (void);
void lamp_rtt (void);
void lamp_on (lampnum_t lamp);
void lamp_off (lampnum_t lamp);
void lamp_toggle (lampnum_t lamp);
int lamp_test (lampnum_t lamp);
void lamp_flash (lampnum_t lamp);
void lamp_noflash (lampnum_t lamp);
void lamp_all_on (void);
void lamp_all_off (void);

void lampset_apply (lampset_id_t id, lamp_operator_t op);
void lampset_apply_on (lampset_id_t id);
void lampset_apply_off (lampset_id_t id);
void lampset_apply_toggle (lampset_id_t id);
void lampset_set_apply_delay (task_ticks_t ticks);
void lampset_step_increment (lampset_id_t id);
void lampset_step_decrement (lampset_id_t id);
void lampset_build_increment (lampset_id_t id);
void lampset_build_decrement (lampset_id_t id);
void lampset_rotate_next (lampset_id_t id);
void lampset_rotate_previous (lampset_id_t id);


#endif /* _SYS_LAMP_H */
