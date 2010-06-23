/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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

#ifndef _WPC_MMAP_H
#define _WPC_MMAP_H

/*
 * The WPC ASIC decodes all addresses placed on the address bus by the
 * processor.
 */


/*
 * The WPC ASIC registers can be broadly into several categories, based
 * on the upper 12 bits of the address.
 */
#define WPC_DEBUG_BASE              0x3D60
#define WPC_OLD_DEBUG_BASE          0x3E60
#define WPC_DMD_BASE                0x3FB0
#define WPC_PRN_TICKET_BASE         0x3FC0
#define WPC_IO_BASE                 0x3FD0
#define WPC_DRIVER_BASE             0x3FE0
#define WPC_ASIC_BASE               0x3FF0

/*
 * Early Funhouse ROMs used a different debug widget with different
 * addressing.
 */
#ifdef CONFIG_OLD_DEBUGGER
#define WPC_DEBUG_CONTROL_PORT      0x3E66
#define WPC_DEBUG_DATA_PORT         0x3E67
#else
#define WPC_DEBUG_DATA_PORT         0x3D60
#define WPC_DEBUG_CONTROL_PORT      0x3D61
#endif

#if (MACHINE_WPC95 == 1)
#define WPC_DMD_3200_PAGE           0x3FB8
#define WPC_DMD_3000_PAGE           0x3FB9
#define WPC_DMD_3600_PAGE           0x3FBA
#define WPC_DMD_3400_PAGE           0x3FBB
#endif
#define WPC_DMD_HIGH_PAGE           0x3FBC
#define WPC_DMD_FIRQ_ROW_VALUE      0x3FBD
#define WPC_DMD_LOW_PAGE            0x3FBE
#define WPC_DMD_ACTIVE_PAGE         0x3FBF

#define WPC_PARALLEL_STATUS_PORT    0x3FC0
#define WPC_PARALLEL_DATA_PORT      0x3FC1
#define WPC_PARALLEL_STROBE_PORT    0x3FC2
#define WPC_SERIAL_DATA_OUTPUT      0x3FC3
#define WPC_SERIAL_CONTROL_OUTPUT   0x3FC4
#define WPC_SERIAL_BAUD_SELECT      0x3FC5
#define WPC_TICKET_DISPENSE         0x3FC6

/*
 * Funhouse used a System 11 sound board which slightly
 * overlaps the register range used by the Fliptronic
 * board, so these cannot be used at the same time.
 *
 * Port B on the Fliptronic board is unused.
 */
#ifdef MACHINE_SYS11_SOUND
#define WPCS11_READY_OUT            0x3FD0
#define WPCS11_DATA_OUT             0x3FD1
#define WPCS11_DATA_IN              0x3FD2
#define WPCS11_READY_IN             0x3FD3
#define WPCS11_RESET                0x3FD4

#else

#ifdef MACHINE_FLIPTRONIC
#define WPC_FLIPTRONIC_PORT_A       0x3FD4
#define WPC_FLIPTRONIC_PORT_B       0x3FD5
#endif
#define WPCS_DATA                   0x3FDC
#define WPCS_CONTROL_STATUS         0x3FDD

#endif /* MACHINE_SYS11_SOUND */

#define WPC_SOL_GEN_OUTPUT          0x3FE0
#define WPC_SOL_HIGHPOWER_OUTPUT    0x3FE1
#define WPC_SOL_FLASHER_OUTPUT      0x3FE2
#define WPC_SOL_LOWPOWER_OUTPUT     0x3FE3
#define WPC_LAMP_ROW_OUTPUT         0x3FE4
#define WPC_LAMP_COL_STROBE         0x3FE5
#define WPC_GI_TRIAC                0x3FE6
#define WPC_SW_JUMPER_INPUT         0x3FE7
#define WPC_SW_CABINET_INPUT        0x3FE8

#define WPC_SW_ROW_INPUT            0x3FE9
#define WPCS_PIC_READ               0x3FE9
#define WPC_SW_COL_STROBE           0x3FEA
#define WPCS_PIC_WRITE              0x3FEA

#if (MACHINE_DMD == 0)
#define WPC_ALPHA_POS               0x3FEB
#define WPC_ALPHA_ROW1              0x3FEC
#else
#define WPC_EXTBOARD1               0x3FEB
#define WPC_EXTBOARD2               0x3FEC
#define WPC_EXTBOARD3               0x3FED
#endif

#if (MACHINE_WPC95 == 1)
#define WPC95_FLIPPER_COIL_OUTPUT   0x3FEE
#define WPC95_FLIPPER_SWITCH_INPUT  0x3FEF
#else
#endif

#if (MACHINE_DMD == 0)
#define WPC_ALPHA_ROW2              0x3FEE
#else
#endif

#define WPC_LEDS                    0x3FF2
#define WPC_RAM_BANK                0x3FF3
#define WPC_SHIFTADDR               0x3FF4
#define WPC_SHIFTBIT                0x3FF6

/* After writing a value from 0-7 to this register, reading it
back will return a single bit set (0x1, 0x2, etc. up to 0x80). */
#define WPC_SHIFTBIT2               0x3FF7
#define WPC_PERIPHERAL_TIMER_FIRQ_CLEAR 0x3FF8
#define WPC_ROM_LOCK                0x3FF9
#define WPC_CLK_HOURS_DAYS          0x3FFA
#define WPC_CLK_MINS                0x3FFB
#define WPC_ROM_BANK                0x3FFC
#define WPC_RAM_LOCK                0x3FFD
#define WPC_RAM_LOCKSIZE            0x3FFE
#define WPC_ZEROCROSS_IRQ_CLEAR     0x3FFF


#endif /* _WPC_MMAP_H */
