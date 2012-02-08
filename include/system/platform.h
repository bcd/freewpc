
#ifndef __SYS_PLATFORM_H
#define __SYS_PLATFORM_H

extern __fastram__ switch_bits_t sw_raw;
extern __fastram__ switch_bits_t sw_edge;
extern __fastram__ switch_bits_t sw_stable;
extern __fastram__ switch_bits_t sw_unstable;
extern __fastram__ switch_bits_t sw_logical;

extern inline void platform_switch_input (const U8 col, U8 value)
{
	sw_raw[col] = value;
}

extern inline void platform_switch_debounce (const U8 col)
{
	U8 edge = sw_raw[col] ^ sw_logical[col];
	sw_stable[col] |= edge & sw_edge[col];
	sw_unstable[col] |= ~edge & sw_stable[col];
	sw_edge[col] = edge;
}

extern __fastram__ U8 lamp_matrix[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_leff1_matrix[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_leff1_allocated[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_leff2_matrix[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_leff2_allocated[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_strobe_mask;
extern __fastram__ U8 lamp_strobe_column;
extern __fastram__ U8 lamp_power_timer;
extern U8 lamp_power_level;
extern U16 lamp_power_idle_timer;

extern inline U8 platform_lamp_compute (const U8 col)
{
	/* Grab the default lamp values */
	U8 bits = lamp_matrix[col];

	/* OR in the flashing lamp values.  These are guaranteed to be
	 * zero for any lamps where the flash is turned off.
	 * Otherwise, these bits are periodically inverted by the
	 * (slower) flash rtt function above.
	 * This means that for the flash to work, the default bit
	 * must be OFF when the flash bit is ON.  (Use the tristate
	 * macros to ensure this.)
	 */
	bits |= lamp_flash_matrix_now[col];

	/* TODO : implement lamp strobing, like the newer Stern games
	do.  Implement like DMD page flipping, alternating between 2
	different lamp matrices rapidly to present 4 different
	intensities.  A background task, like the flash_rtt above,
	would toggle the intensities at a slower rate. */

	/* Override with the lamp effect lamps.
	 * Leff2 bits are low priority and used for long-running
	 * lamp effects.  Leff1 is higher priority and used
	 * for quick effects.  Therefore leff2 is applied first,
	 * and leff1 may override it.
	 */
	bits &= lamp_leff2_allocated[col];
	bits |= lamp_leff2_matrix[col];
	bits &= lamp_leff1_allocated[col];
	bits |= lamp_leff1_matrix[col];
	return bits;
}

#endif /* __SYS_PLATFORM_H */
