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

#define FM_ADDR_TEST 0x1
#define FM_ADDR_KEY_ON 0x8
#define FM_ADDR_NOISE_ENABLE 0xF
#define FM_ADDR_CLOCK_A1 0x10
#define FM_ADDR_CLOCK_A2 0x11
#define FM_ADDR_CLOCK_B 0x12
#define FM_ADDR_CLOCK_CTRL 0x14

	#define FM_TIMER_CSM 0x80
	#define FM_TIMER_FRESETB 0x20
	#define FM_TIMER_FRESETA 0x10
	#define FM_TIMER_IRQENB 0x08
	#define FM_TIMER_IRQENA 0x04
	#define FM_TIMER_LOADB 0x02
	#define FM_TIMER_LOADA 0x01

	#define FM_TIMER_ISTB 0x2
	#define FM_TIMER_ISTA 0x1

#define FM_ADDR_LFRQ 0x18

#define FM_ADDR_PMD_AMD 0x19

	#define PMD_AMD_SELECT 0x80
	#define SELECT_PMD 0x80
	#define SELECT_AMD 0x0

#define FM_ADDR_CT_W 0x1B

	#define FM_CT2 0x80
	#define FM_CT1 0x40
	#define FM_WAVE_SELECT 0x3

		#define PM_JAGGED 0x0
		#define PM_SQUARE 0x1
		#define PM_SAW 0x2
		#define PM_NOISE 0x3

		#define AM_RIGHT_TRIANGLE 0x0
		#define AM_SQUARE 0x1
		#define AM_TRIANGLE 0x2
		#define AM_NOISE 0x3

#define FM_ADDR_CHANNEL_CTRL 0x20
#define FM_ADDR_KEYCODE(n) (0x28 + (n))
#define FM_ADDR_KEYFRAC(n) (0x30 + (n))

#endif /* _WPCSOUND_H */
