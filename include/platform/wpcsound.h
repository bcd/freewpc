/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef _WPCSOUND_H
#define _WPCSOUND_H

/* Memory map */

#define WPCS_IOBASE 0x2000

#define WPCS_ROM_BANK          (WPCS_IOBASE + 0)
#define WPCS_FM_ADDR_STATUS    (WPCS_IOBASE + 0x400)
#define WPCS_FM_DATA           (WPCS_IOBASE + 0x401)
#define WPCS_DAC               (WPCS_IOBASE + 0x800)
#define WPCS_CVSD_DATA         (WPCS_IOBASE + 0xC00)
#define WPCS_HOST_INPUT        (WPCS_IOBASE + 0x1000)
#define WPCS_CVSD_CLK          (WPCS_IOBASE + 0x1400)
#define WPCS_EPOT              (WPCS_IOBASE + 0x1800)
#define WPCS_HOST_OUTPUT       (WPCS_IOBASE + 0x1C00)

/* When writing to the e-pot, bit 1 must be set in order for
data to be clocked in.  Bit 2 determines if the volume is
increased or decreased.  EPOT_MAX says what the maximum
hardware supported volume is. */

#define EPOT_LOUDER 0x0
#define EPOT_SOFTER 0x2
#define EPOT_CLOCK 0x1
#define EPOT_MAX 128

/* FM chip (Yamaha 2151) registers */

#define DEV_MOD1 0
#define DEV_MOD2 1
#define DEV_CAR1 2
#define DEV_CAR2 3

#define FM_ADDR_TEST 0x1

	#define FM_LFO_RESET 0x2

#define FM_ADDR_KEY_ON 0x8

	#define FM_MOD1 0x40
	#define FM_CAR1 0x20
	#define FM_MOD2 0x10
	#define FM_CAR2 0x8
	#define FM_KEYCH_MASK 0x7

#define FM_ADDR_NOISE_ENABLE 0xF

	#define FM_NOISE_ENABLE 0x80
	#define FM_NOISE_FREQ_MASK 0x1F

#define FM_ADDR_CLOCK_A1 0x11
#define FM_ADDR_CLOCK_A2 0x12
#define FM_ADDR_CLOCK_B 0x13
#define FM_ADDR_CLOCK_CTRL 0x14

	/* Values for writing to the clock control register */
	#define FM_TIMER_CSM 0x80
	#define FM_TIMER_FRESETB 0x20
	#define FM_TIMER_FRESETA 0x10
	#define FM_TIMER_IRQENB 0x08
	#define FM_TIMER_IRQENA 0x04
	#define FM_TIMER_LOADB 0x02
	#define FM_TIMER_LOADA 0x01

	/* Values for reading from the clock control register */
	#define FM_TIMER_ISTB 0x2
	#define FM_TIMER_ISTA 0x1

#define FM_ADDR_LFRQ 0x18

#define FM_ADDR_PMD_AMD 0x19

	#define FM_PMD_AMD_SELECT 0x80
	#define FM_SELECT_PMD 0x80
	#define FM_SELECT_AMD 0x0

#define FM_ADDR_CT_W 0x1B

	#define FM_CT2 0x80
	#define FM_CT1 0x40
	#define FM_WAVE_SELECT 0x3

		#define FM_WAVE_SAW 0x0
		#define FM_WAVE_SQUARE 0x1
		#define FM_WAVE_TRIANGLE 0x2
		#define FM_WAVE_NOISE 0x3

#define FM_ADDR_CHAN_CTRL 0x20

	#define FM_CHAN_RIGHT 0x80
	#define FM_CHAN_LEFT 0x40
	#define FM_FB_MASK 0x38
	#define FM_CONNECT_MASK 0x7

#define FM_ADDR_KEYCODE(ch) (0x28 + (ch))

#define FM_ADDR_KEYFRAC(ch) (0x30 + (ch))

#define FM_ADDR_SENS(ch) (0x38 + (ch))

#define FM_ADDR_DETUNE_MULT(dev,ch)  (0x40 + 8 * (dev) + (ch))

#define FM_ADDR_LEVEL(dev,ch)  (0x60 + 8 * (dev) + (ch))

#define FM_ADDR_ATTACK(dev,ch)  (0x80 + 8 * (dev) + (ch))

#define FM_ADDR_DECAY1(dev,ch)  (0xA0 + 8 * (dev) + (ch))

#define FM_ADDR_DECAY2(dev,ch)  (0xC0 + 8 * (dev) + (ch))

#define FM_ADDR_RELEASE(dev,ch)  (0xE0 + 8 * (dev) + (ch))

#endif /* _WPCSOUND_H */
