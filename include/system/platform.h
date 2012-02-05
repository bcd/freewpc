
#ifndef __SYS_PLATFORM_H
#define __SYS_PLATFORM_H

extern __fastram__ switch_bits_t sw_raw;
extern __fastram__ switch_bits_t sw_edge;
extern __fastram__ switch_bits_t sw_stable;
extern __fastram__ switch_bits_t sw_unstable;
extern __fastram__ switch_bits_t sw_logical;

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

#endif /* __SYS_PLATFORM_H */
