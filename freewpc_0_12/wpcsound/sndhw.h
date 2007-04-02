/*
 * Copyright 2007 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SNDHW_H
#define _SNDHW_H

/* Williams sound board I/O registers */

/** ROM bank register */
#define WPCS_ROM_BANK        0x2000

/** Address (W) / status (R) register for the FM chip (YM2151) */
#define WPCS_FM_ADDR_STATUS  0x2400

/** Data register (R/W) for the FM chip */
#define WPCS_FM_DATA         0x2401

/** DAC output register */
#define WPCS_DAC_DATA        0x2800

/** CVSD output register */
#define WPCS_CVSD_DATA       0x2C00

/** Input from the CPU board.  When the CPU writes a value to
the sound board, an IRQ is generated on the sound board and
the value is available here. */
#define WPCS_CPU_INPUT       0x3000

/** CVSD clear register */
#define WPCS_CVSD_CLEAR      0x3400

/** Volume control (e-pot) register */
#define WPCS_VOLUME          0x3800

/** Output to the CPU board.  The value is latched so that the CPU
can retrieve it when it is ready. */
#define WPCS_CPU_OUTPUT      0x3C00


/* YM2151 FM chip addresses */

#define FM_ADDR_TEST_LFO_RESET 0x1
#define FM_ADDR_KEY_ON 0x8
#define FM_ADDR_NOISE_ENABLE 0xF
#define FM_ADDR_CLOCK_A1 0x10
#define FM_ADDR_CLOCK_A2 0x11
#define FM_ADDR_CLOCK_B 0x12
#define FM_ADDR_CLOCK_FUNCTIONS 0x14
	#define FM_CLOCK_CSM 0x80
	#define FM_CLOCK_RESET_B 0x20
	#define FM_CLOCK_RESET_A 0x10
	#define FM_CLOCK_IRQ_B 0x8
	#define FM_CLOCK_IRQ_A 0x4
	#define FM_CLOCK_LOAD_B 0x2
	#define FM_CLOCK_LOAD_A 0x1
#define FM_ADDR_LOW_FREQUENCY 0x18
#define FM_ADDR_PHASE_AMP_MOD 0x19
#define FM_ADDR_CONTROL_WAVEFORM 0x1B
#define FM_ADDR_CHANNEL_CTRL 0x20
#define FM_ADDR_KEYCODE0 0x28
#define FM_ADDR_KEYCODE1 0x29
#define FM_ADDR_KEYCODE2 0x2A
#define FM_ADDR_KEYCODE3 0x2B
#define FM_ADDR_KEYCODE4 0x2C
#define FM_ADDR_KEYCODE5 0x2D
#define FM_ADDR_KEYCODE6 0x2E
#define FM_ADDR_KEYCODE7 0x2F
#define FM_ADDR_FRACTION0 0x30
#define FM_ADDR_FRACTION1 0x31
#define FM_ADDR_FRACTION2 0x32
#define FM_ADDR_FRACTION3 0x33
#define FM_ADDR_FRACTION4 0x34
#define FM_ADDR_FRACTION5 0x35
#define FM_ADDR_FRACTION6 0x36
#define FM_ADDR_FRACTION7 0x37

extern inline void hard_delay (const U16 loops)
{
	asm __volatile__ ("ldx\t%0" :: "n" (loops) : "x");
	asm __volatile__ ("1$: leax\t-1,x");
	asm __volatile__ ("bne\t1$");
}

extern inline void io_write (U16 addr, U8 val)
{
	*(volatile U8 *)addr = val;
}

extern inline U8 io_read (U16 addr)
{
	return *(volatile U8 *)addr;
}


extern inline void fm_write (U8 fm_reg, U8 val)
{
	while (io_read (WPCS_FM_DATA) & 0x80);
	io_write (WPCS_FM_ADDR_STATUS, fm_reg);
	while (io_read (WPCS_FM_DATA) & 0x80);
	io_write (WPCS_FM_DATA, val);
}

extern inline U8 cpu_latch_read (void)
{
	return io_read (WPCS_CPU_INPUT);
}

extern inline void cpu_latch_write (U8 val)
{
	io_write (WPCS_CPU_OUTPUT, val);
}

extern inline void dac_write (U8 val)
{
	io_write (WPCS_DAC_DATA, val);
}


extern inline void adjust_volume (const U8 up_down_flag)
{
	io_write (WPCS_VOLUME, (up_down_flag << 1) | 1);
	io_write (WPCS_VOLUME, (up_down_flag << 1) | 0);
}


#endif /* _SNDHW */
