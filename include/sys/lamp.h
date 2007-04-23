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

#ifndef _SYS_LAMP_H
#define _SYS_LAMP_H

/** The maximum number of physical lamps supported */
#define NUM_LAMPS 64

/** Lamp numbers in this range refer to per-player flags */
#define FLAG_OFFSET					0x40

/** Lamp numbers in this range refer to the flash state of the lamps */
#define LAMP_FLASH_OFFSET			0x80

/** Lamp numbers in this range refer to global flags */
#define GLOBAL_FLAG_OFFSET	      0xC0

#define NUM_LAMP_COLS	8

#define NUM_VLAMP_COLS	8

/** Macro to create a lamp number from its row and column */
#define MAKE_LAMP(col,row)	((((col)-1) * 8) + (row)-1)

/** The lamp flash state is updated every 32ms, so this will
 * toggle the lamp flash state about 8 times/sec */
#define LAMP_DEFAULT_FLASH_RATE 4

/** Small integer type for a lamp number */
typedef U8 lampnum_t;

/** A lamp operator is any routine which abides to the following
 * prototype for modifying a simple lamp.  Standard routines
 * like lamp_on, lamp_off are lamp operators. */
typedef void (*lamp_operator_t) (lampnum_t);

/** A lampset is a sequence of lamp numbers, some of which are
 * "immediate lamp values", others are "lamp value macros".
 */
typedef const lampnum_t lampset_t[];

/** Lampsets are identified by small integers */
typedef U8 lampset_id_t;


/** Macros used to instantiate a lampset.  Lampsets are declared
 * as a simple list of lamp values; instantiation turns the
 * list into an actual array declaration, properly terminated. */
#define DECL_LAMPSET_INSTANCE(idx, lampdata...) \
	static const lampnum_t __ ## lampset_ ## idx ## _data[] = { lampdata, LAMP_END }; \


#define DECL_LAMPSET_TABLE_ENTRY(idx, lampdata...) \
	[idx] = __ ## lampset_ ## idx ## _data,


/**
 * Lamp macros are lampset members which calculate actual
 * lamp values at runtime.
 *
 * Macro values start above the range of acutal lamp numbers.
 * These macros will be deprecated and replaced by a compile-time
 * mechanism to construct new lampsets.
 */

/** Indicates the end of a lampset */
#define LAMP_END						(NUM_LAMPS + 2)

/** Indicates a time delay within a lampset */
#define LAMP_MACRO_SLEEP_OP		(NUM_LAMPS + 3)


void lamp_init (void);
void lamp_flash_rtt (void);
void lamp_rtt (void);
void lamp_on (lampnum_t lamp);
void lamp_off (lampnum_t lamp);
void lamp_toggle (lampnum_t lamp);
bool lamp_test (lampnum_t lamp);
void lamp_flash_on (lampnum_t lamp);
void lamp_flash_off (lampnum_t lamp);
bool lamp_flash_test (lampnum_t lamp);
void leff_on (lampnum_t lamp);
void leff_off (lampnum_t lamp);
void leff_toggle (lampnum_t lamp);
bool leff_test (lampnum_t lamp);

#define flag_on(lamp)		lamp_on (lamp + FLAG_OFFSET)
#define flag_off(lamp)		lamp_off (lamp + FLAG_OFFSET)
#define flag_toggle(lamp)	lamp_toggle (lamp + FLAG_OFFSET)
#define flag_test(lamp)		lamp_test (lamp + FLAG_OFFSET)

#define global_flag_on(lamp)		lamp_on (lamp + GLOBAL_FLAG_OFFSET)
#define global_flag_off(lamp)		lamp_off (lamp + GLOBAL_FLAG_OFFSET)
#define global_flag_toggle(lamp)	lamp_toggle (lamp + GLOBAL_FLAG_OFFSET)
#define global_flag_test(lamp)		lamp_test (lamp + GLOBAL_FLAG_OFFSET)

#define long_leff_allocate(lamp)		lamp_leff_allocate(lamp + 0x80)
#define long_leff_free(lamp)			lamp_leff_free(lamp + 0x80)
#define long_leff_on(lamp)				leff_on(lamp + 0x80)
#define long_leff_off(lamp)			leff_off(lamp + 0x80)

#define lamp_tristate_on(lamp) \
	do { lamp_flash_off(lamp); lamp_on(lamp); } while (0)
#define lamp_tristate_off(lamp) \
	do { lamp_flash_off(lamp); lamp_off(lamp); } while (0)
#define lamp_tristate_flash(lamp) \
	do { lamp_flash_on(lamp); lamp_off(lamp); } while (0)

void lamp_all_on (void);
void lamp_all_off (void);
void lamp_leff1_allocate_all (void);
void lamp_leff1_erase (void);
void lamp_leff1_free_all (void);
void lamp_leff2_erase (void);
void lamp_leff2_free_all (void);
void lamp_leff_allocate (lampnum_t lamp);
void lamp_leff_free (lampnum_t lamp);
void lamp_leff2_allocate (lampnum_t lamp);
bool lamp_leff2_test_allocated (lampnum_t lamp);
void lamp_leff2_free (lampnum_t lamp);

const U8 *lampset_lookup (lampset_id_t id);
void lampset_apply (lampset_id_t id, lamp_operator_t op);
void lampset_apply_on (lampset_id_t id);
void lampset_apply_off (lampset_id_t id);
void lampset_apply_toggle (lampset_id_t id);
void lampset_apply_leff_on (lampset_id_t id);
void lampset_apply_leff_off (lampset_id_t id);
void lampset_apply_leff_toggle (lampset_id_t id);
void lampset_apply_leff_alternating (lampset_id_t id, U8 initially_on);
void lampset_set_apply_delay (task_ticks_t ticks);
void lampset_step_increment (lampset_id_t id);
void lampset_leff_step_increment (lampset_id_t id);
void lampset_step_decrement (lampset_id_t id);
void lampset_build_increment (lampset_id_t id);
void lampset_build_decrement (lampset_id_t id);
void lampset_rotate_next (lampset_id_t id);
void lampset_rotate_previous (lampset_id_t id);


#endif /* _SYS_LAMP_H */
