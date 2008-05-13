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
#define WPCS_DSP_ADDR_STATUS   (WPCS_IOBASE + 0x400)
#define WPCS_DSP_DATA          (WPCS_IOBASE + 0x401)
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

#endif /* _WPCSOUND_H */
