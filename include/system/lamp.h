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

#ifndef _SYS_LAMP_H
#define _SYS_LAMP_H

/** Says how many bytes are needed to hold N bits */
#define BITS_TO_BYTES(n)  (((n) + 7) / 8)

/** The number of bytes for tracking the playfield lamps */
#define NUM_LAMP_COLS BITS_TO_BYTES(PINIO_NUM_LAMPS)

/** Macro to create a lamp number from its row and column */
#define MAKE_LAMP(col,row)	((((col)-1) * 8) + (row)-1)


/** Small integer type for a lamp number */
typedef U8 lampnum_t;

/** A lamp operator is any routine which abides to the following
 * prototype for modifying a simple lamp.  Standard routines
 * like lamp_on, lamp_off are lamp operators. */
typedef void (*lamp_operator_t) (lampnum_t);

/** Likewise for boolean operators, like lamp_test */
typedef bool (*lamp_boolean_operator_t) (lampnum_t);

/** A lamplist is a sequence of lamp numbers, some of which are
 * "immediate lamp values", others are "lamp value macros".
 */
typedef const lampnum_t lamplist_t[];

/** Lampsets are identified by small integers */
typedef U8 lamplist_id_t;

/** The set of all lamps, with one bit for each */
typedef U8 lamp_set[NUM_LAMP_COLS];


extern __fastram__ lamp_set lamp_matrix;
extern lamp_set lamp_flash_matrix;
extern __fastram__ lamp_set lamp_flash_matrix_now;

extern U8 bit_matrix[BITS_TO_BYTES (MAX_FLAGS)];
extern U8 global_bits[BITS_TO_BYTES (MAX_GLOBAL_FLAGS)];


typedef enum
{
	LMX_DEFAULT,
	LMX_FLASH,
	LMX_EFFECT1_ALLOC,
	LMX_EFFECT1_LAMPS,
	LMX_EFFECT2_ALLOC,
	LMX_EFFECT2_LAMPS,
} lamp_matrix_id_t;

/**
 * Special lamp numbers that don't refer to real lamps, but
 * to lamplist operations.
 */

/** Indicates the end of a lamplist */
#define LAMP_END (PINIO_NUM_LAMPS + 2)

/** Indicates a time delay within a lamplist */
#define LAMP_BREAK (PINIO_NUM_LAMPS + 3)


void lamp_init (void);
void lamp_flash_rtt (void);

__pure__ U8 *matrix_lookup (lamp_matrix_id_t id);

void lamp_on (lampnum_t lamp);
void lamp_off (lampnum_t lamp);
void lamp_toggle (lampnum_t lamp);
bool lamp_test (lampnum_t lamp);
bool lamp_test_off (lampnum_t lamp);
void lamp_flash_on (lampnum_t lamp);
void lamp_flash_off (lampnum_t lamp);
bool lamp_flash_test (lampnum_t lamp);
void lamp_power_set (U8 level);
void leff_on (lampnum_t lamp);
void leff_off (lampnum_t lamp);
void leff_toggle (lampnum_t lamp);
bool leff_test (lampnum_t lamp);


#define flag_on(f)      bit_on (bit_matrix, __addrval(&f))
#define flag_off(f)     bit_off (bit_matrix, __addrval(&f))
#define flag_toggle(f)  bit_toggle (bit_matrix, __addrval(&f))
#define flag_test(f)    bit_test (bit_matrix, __addrval(&f))

#define global_flag_on(gf)     bit_on (global_bits, __addrval(&gf))
#define global_flag_off(gf)    bit_off (global_bits, __addrval(&gf))
#define global_flag_toggle(gf) bit_toggle (global_bits, __addrval(&gf))
#define global_flag_test(gf)   bit_test (global_bits, __addrval(&gf))

#define flag_test_and_set(f) \
	({ U8 result = flag_test (f); if (!result) { flag_on (f); } result; })

#define lamp_tristate_on(lamp) \
	do { lamp_flash_off(lamp); lamp_on(lamp); } while (0)
#define lamp_tristate_off(lamp) \
	do { lamp_flash_off(lamp); lamp_off(lamp); } while (0)
#define lamp_tristate_flash(lamp) \
	do { lamp_flash_on(lamp); lamp_off(lamp); } while (0)

#define lamp_on_if(lamp, pred) \
	if (pred) { lamp_tristate_on (lamp); } \
	else { lamp_tristate_off (lamp); }

#define lamp_flash_if(lamp, pred) \
	if (pred) { lamp_tristate_flash (lamp); } \
	else { lamp_tristate_off (lamp); }

void bit_on (bitset matrix, U8 bit);
void bit_off (bitset matrix, U8 bit);
void bit_toggle (bitset matrix, U8 bit);
bool bit_test (const_bitset matrix, U8 bit);
bool bit_test_all_on (const_bitset matrix);
bool bit_test_all_off (const_bitset matrix);

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
bool lamp_leff2_test_and_allocate (lampnum_t lamp);
void lamp_leff2_free (lampnum_t lamp);

const U8 *lamplist_lookup (lamplist_id_t id);
lampnum_t lamplist_index (lamplist_id_t id, U8 n);
void lamplist_apply_nomacro (lamplist_id_t id, lamp_operator_t op);
void lamplist_apply (lamplist_id_t id, lamp_operator_t op);

void lamplist_apply_leff_alternating (lamplist_id_t id, U8 initially_on);
void lamplist_set_apply_delay (task_ticks_t ticks);
void lamplist_step_increment (lamplist_id_t id, bitset matrix);
void lamplist_step_decrement (lamplist_id_t id, bitset matrix);
void lamplist_build_increment (lamplist_id_t id, bitset matrix);
void lamplist_build_decrement (lamplist_id_t id, bitset matrix);
void lamplist_rotate_next (lamplist_id_t id, bitset matrix);
void lamplist_rotate_previous (lamplist_id_t id, bitset matrix);
void lamplist_set_count (lamplist_id_t set, U8 count);
bool lamplist_test_all (lamplist_id_t id, lamp_boolean_operator_t op);

__attribute__((noinline)) void lamp_set_on (lamp_set matrix);
__attribute__((noinline)) void lamp_set_off (lamp_set matrix);

#endif /* _SYS_LAMP_H */
