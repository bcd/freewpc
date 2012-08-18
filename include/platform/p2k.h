/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

#ifndef P2K_H
#define P2K_H

#undef DEBUGGER
/* TODO */
#define WPC_DMD_LOW_PAGE 0xF0
#define WPC_DMD_HIGH_PAGE 0xF1
#define WPC_DMD_FIRQ_ROW_VALUE 0xF2
#define WPC_DMD_ACTIVE_PAGE 0xF3
extern U8 *pinio_dmd_low_page, *pinio_dmd_high_page;

/* TODO : we shouldn't need to declare anything if there is no banking... */

#define PINIO_BANK_ROM 0
#define PINIO_BANK_RAM 1
extern inline void pinio_set_bank (U8 bankno, U8 val) { }
extern inline U8 pinio_get_bank (U8 bankno) { return 0; }

/* TODO : why do I have to say this? */
#define LOCAL_SIZE 64


/*
 * Parallel port registers
 */

#define LPT_BASE         0x27F

#define LPT_DATA         (LPT_BASE + 0)
#define LPT_STATUS       (LPT_BASE + 1)
	#define LPT_ID   0xF8
#define LPT_CONTROL      (LPT_BASE + 2)
	#define LPT_REG_OE     0x01
	#define LPT_REG_LATCH  0x04
	#define LPT_DIR_CTRL   0x08

/*
 * P2K I/O register set
 */

#define P2K_COIN_INPUT       0
#define P2K_FLIPPER_INPUT    1
	#define P2K_SLAM_TILT        0x1
	#define P2K_COIN_DOOR_CLOSED 0x2
	#define P2K_TILT             0x4
	#define P2K_RIGHT_BUTTON     0x10
	#define P2K_LEFT_BUTTON      0x20
	#define P2K_RIGHT_ACTION     0x40
	#define P2K_LEFT_ACTION      0x80
#define P2K_DIP_INPUT        2
#define P2K_EOS_DIR_INPUT    3
	#define P2K_ESCAPE 0x1
	#define P2K_DOWN   0x2
	#define P2K_UP     0x4
	#define P2K_ENTER  0x8
	#define P2K_LR_EOS 0x10
	#define P2K_LL_EOS 0x20
	#define P2K_UR_EOS 0x40
	#define P2K_UL_EOS 0x80
#define P2K_SWITCH_ROW_INPUT 4
#define P2K_SWITCH_COL_OUTPUT  5
#define P2K_LAMP_ROW_A_OUTPUT  6
#define P2K_LAMP_ROW_B_OUTPUT  7
#define P2K_LAMP_COL_OUTPUT    8
#define P2K_SOLC_OUTPUT        9
#define P2K_SOLB_OUTPUT        10
#define P2K_SOLA_OUTPUT        11
#define P2K_FLIPPER_OUTPUT     12
#define P2K_SOLD_OUTPUT        13
	#define P2K_SOLD_HEALTH        0x10
	#define P2K_SOLD_POWER_RELAY   0x20
	#define P2K_SOLD_COIN_COUNTER  0x40
	#define P2K_SOLD_LAMP_TEST     0x80
#define P2K_LOGIC_OUTPUT       14
#define P2K_SYS_INPUT          15
	#define P2K_SYS_TICKET_NOTCH   0x10
	#define P2K_SYS_TICKET_LOW     0x20
	#define P2K_SYS_BLANKING       0x40
	#define P2K_SYS_ZERO_CROSS     0x80
#define P2K_LAMP_TEST_A_INPUT  16
#define P2K_LAMP_TEST_B_INPUT  17
#define P2K_FUSE_TEST_A_INPUT  18
#define P2K_FUSE_TEST_B_INPUT  19


extern U8 p2k_write_cache[];

void p2k_write (U8 reg, U8 val);
U8 p2k_read (U8 reg);


/**
 * Write '1' to certain bits of a P2K output register.
 */
extern inline void p2k_set_bits (U8 reg, U8 bits)
{
	p2k_write (reg, p2k_write_cache[reg] | bits);
}


/**
 * Write '0' to certain bits of a P2K output register.
 */
extern inline void p2k_clear_bits (U8 reg, U8 bits)
{
	p2k_write (reg, p2k_write_cache[reg] & ~bits);
}


/*
 * LED and Fuse numbers
 */

#define P2K_LED_WATCHDOG      1
#define P2K_LED_HEALTH        2
#define P2K_LED_LAMPA         3
#define P2K_LED_LR_FLIP_POWER 4
#define P2K_LED_LL_FLIP_POWER 5
#define P2K_LED_UR_FLIP_POWER 6
#define P2K_LED_UL_FLIP_POWER 7
#define P2K_LED_LAMPB         8
#define P2K_LED_50V_SOL       9
#define P2K_LED_20V_FLASH     10
#define P2K_LED_SOL_1         11
#define P2K_LED_SOL_2         12
#define P2K_LED_SOL_3         13
#define P2K_LED_SOL_4         14
#define P2K_LED_20V           15
#define P2K_LED_12V           16
#define P2K_LED_5V            17

/*
 * Pinball I/O (pinio) functions
 */

extern inline void pinio_active_led_toggle (void)
{
	p2k_set_bits (P2K_SOLD_OUTPUT, P2K_SOLD_HEALTH);
}

extern inline U8 pinio_read_ac_zerocross (void)
{
	return p2k_read (P2K_SYS_INPUT) & P2K_SYS_ZERO_CROSS;
}

#define PINIO_NUM_LAMPS 128

extern inline void pinio_write_lamp_strobe (U8 val)
{
	p2k_write (P2K_LAMP_COL_OUTPUT, val);
}

extern inline void pinio_write_lamp_data (U16 val)
{
	p2k_write (P2K_LAMP_ROW_A_OUTPUT, val & 0xFF);
	p2k_write (P2K_LAMP_ROW_B_OUTPUT, val << 8);
}

#define PINIO_NUM_SOLS 48

extern inline void pinio_write_solenoid_set (U8 set, U8 val)
{
	switch (set)
	{
		case 0:
			p2k_write (P2K_SOLA_OUTPUT, val);
			break;
		case 1:
			p2k_write (P2K_SOLB_OUTPUT, val);
			break;
		case 2:
			p2k_write (P2K_SOLC_OUTPUT, val);
			break;
		case 3:
			p2k_write (P2K_SOLD_OUTPUT, val);
			break;
		case 4:
			p2k_write (P2K_FLIPPER_OUTPUT, val);
			break;
		case 5:
			p2k_write (P2K_LOGIC_OUTPUT, val);
			break;
	}
}

extern inline IOPTR sol_get_write_reg (U8 sol)
{
}

#define PINIO_NUM_SWITCHES 88

extern inline void pinio_write_switch_column (U8 val)
{
	p2k_write (P2K_SWITCH_COL_OUTPUT, 1 << val);
}

extern inline U8 pinio_read_switch_rows (void)
{
	return p2k_read (P2K_SWITCH_ROW_INPUT);
}

extern inline U8 pinio_read_dedicated_switches (void)
{
	return p2k_read (P2K_EOS_DIR_INPUT);
}

extern inline void pinio_enable_flippers (void)
{
}

extern inline void pinio_disable_flippers (void)
{
}

extern inline void pinio_watchdog_reset (void)
{
}

extern inline void pinio_reset_sound (void)
{
}

extern inline void pinio_write_sound (U8 val)
{
}

extern inline bool pinio_sound_ready_p (void)
{
	return FALSE;
}

extern inline U8 pinio_read_sound (void)
{
	return 0xFF;
}

extern inline U8 pinio_read_locale (void)
{
	return 0;
}

#define pinio_nvram_unlock()
#define pinio_nvram_lock()

#endif /* P2K_H */
