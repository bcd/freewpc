;;;
;;; Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
;;;
;;; This file is part of FreeWPC.
;;;
;;; FreeWPC is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;
;;; FreeWPC is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with FreeWPC; if not, write to the Free Software
;;; Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
;;;

#include <platform/wpc-mmap.h>

STACK_BASE     = 6133
WPC_WATCHDOG_REG = WPC_ZEROCROSS_IRQ_CLEAR
WPC_RAM_UNLOCKED = 0xB4
WPC_RAM_LOCK_2K = 0x1
; Writing a 6 here does not turn off blanking.
WPC_WATCHDOG_RESET = 0x06

; Writing 0x16 here caused the sound board to bong almost immediately.
; But the blanking LED did not go off.  The diag LED did not flicker,
; so this apparently did not enable the IRQ.
;WPC_WATCHDOG_RESET = 0x16

; This had similar effect???
;WPC_WATCHDOG_RESET = 0x86

FLASH_DELAY    = 100

PAGED_REGION   = 0x4000
PAGED_SIZE     = 0x4000
FIXED_REGION   = 0x8000
FIXED_SIZE     = 0x8000

CKSUM_REV      = 0xFFEE

	.module start.s


	; Soft registers m0-m3 can be used by GCC's register allocator
	; for functions that need lots of regs.
	; Even when this feature is not used, these fastram variables
	; are used by some of the assembler routines.
	.area direct
	.globl m0
m0: .blkb 1
	.globl m1
m1: .blkb 1
	.globl m2
m2: .blkb 1
	.globl m3
m3: .blkb 1

;;;
;;; Perform basic diagnostics to ensure that everything is
;;; more or less working.  (Diags must be in the system page
;;; since the paging hardware may be broken.)
;;; 1. Verify ROM is good first, since that ensures that this
;;; code is not corrupted somehow.  Take care NOT to use RAM
;;; at all during this stage; this is tricky and may require
;;; assembler macros.
;;;
;;; 2. Verify RAM next, using a read-write test.
;;;
;;; 3. Verify WPC ASIC functions.
;;; At any point, if something goes wrong, we go into a hard
;;; loop and pulse the diagnostic LED with a flash code to
;;; report the error.  We can't rely on the DMD working
;;; properly to help us here.
;;;

	;;;
	;;; start - reset entry point
	;;;
	.area	.text
	.globl _start
_start:
	; Disable interrupts (IRQ and FIRQ) until the system is
	; initialized.
	orcc	#0x50

	; Initialize the direct page pointer.  This hardware register
	; determines where 'direct' addressing instructions are targeted.
	; By setting to zero, direct addresses are mapped to 0000h-00FFh.
	; We can use shorter instructions when referencing variables here.
	clra
	tfr	a,dp

#ifndef FASTBOOT
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   ROM POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rom_test:
	; Initialize 16-bit checksum
	ldd	#0

	; Value to be periodically written to reset the watchdog
	ldu	#WPC_WATCHDOG_RESET

	; Compute checksum for fixed region (0x8000-0xFFFF).
	; TODO - it is possible to do these checks like just
	; any other bank - they can be mapped to 0x4000.
	ldx	#0x8000
fixed_loop:
	addb	,x
	adca	#0
	addb	1,x
	adca	#0
	addb	2,x
	adca	#0
	addb	3,x
	adca	#0

	exg	d,u
	stb	WPC_WATCHDOG_REG
	exg	d,u

	leax	4,x
	cmpx	#0x0000
	bne	fixed_loop

	; Compute checksum for each paged region (0x4000-0x7FFF).
	; Y is used to iterate over each bank of ROM.
	ldy	#TOP_BANK
paged_loop:
	ldx	#0x4000

	; Switch to the next bank of ROM.
	exg	d,y
	stb	WPC_ROM_BANK
	exg	d,y

	; Sum the entire bank (16KB).
paged_inner_loop:
	addb	,x
	adca	#0
	addb	1,x
	adca	#0
	addb	2,x
	adca	#0
	addb	3,x
	adca	#0
	addb	4,x
	adca	#0
	addb	5,x
	adca	#0
	addb	6,x
	adca	#0
	addb	7,x
	adca	#0

	exg	d,u
	stb	WPC_WATCHDOG_REG
	exg	d,u

	leax	8,x
	cmpx	#0x8000
	bne	paged_inner_loop
	leay	-1,y
	cmpy	#BOTTOM_BANK
	bge	paged_loop

	; Did checksum validate?
	subd	CKSUM_REV
	cmpd	#0
	beq	ram_test
rom_checksum_error:
	ldx	#1
	jmp	diag_error
#endif /* FASTBOOT */

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   RAM POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ram_test:
	lda	#WPC_RAM_UNLOCKED
	sta	WPC_RAM_LOCK
	lda	#WPC_RAM_LOCK_2K
	sta	WPC_RAM_LOCKSIZE
	clr	WPC_RAM_LOCK

	ldx	#0
ram_loop:
	ldd	#0x55AA
	std	,x
	cmpd	,x
	bne	ram_error

	coma
	comb
	std	,x
	cmpd	,x
	bne	ram_error

	clra
	clrb
	std	,x
	cmpd	,x++
	bne	ram_error

	ldb	#WPC_WATCHDOG_RESET
	stb	WPC_WATCHDOG_REG

	/* TODO - upper limit hardcoded */
	cmpx	#0x1600
	blo	ram_loop
	bra	asic_test

ram_error:
	ldx	#2
	jmp	diag_error


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   ASIC POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
asic_test:
	/* TODO */

	;;; END OF DIAGNOSTICS

test_done:
	; Initialize the stack pointer.  We can now make
	; function calls!
1$:
	lds	#STACK_BASE
	jmp	_main   ; Jump into C code
	bra	1$

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;; diag_error
	;;; Handles a diagnstics error by flashing the diag LED.
   ;;;
	;;; Input: X = diagnostic error code
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
diag_error:
	tfr	x,d             ; Double X
	leax	d,x

	tfr	x,u             ; Copy count to iterator U

flash_loop:
	ldb	WPC_LEDS        ; Toggle the LED
	eorb	#-128
	stb	WPC_LEDS

	ldy	#15000
outer_flash_loop:        ; Hold the LED state
	mul
	mul
	mul
	ldb	#WPC_WATCHDOG_RESET
	stb	WPC_WATCHDOG_REG
	ldb	WPC_LEDS
	stb	WPC_LEDS
	leay	-1,y
	cmpy	#0
	bne	outer_flash_loop

	leau	-1,u
	cmpu	#0
	bne	flash_loop

	ldy	#55000
delay_loop:
	; Use 'mul' instructions here because they are the
	; shortest instructions that give the longest delay.
	; The actual values computed don't matter (but
	; note that it destroys values in D).
	mul
	mul
	mul
	ldb	#WPC_WATCHDOG_RESET
	stb	WPC_WATCHDOG_REG
	leay	-1,y
	cmpy	#0
	bne	delay_loop

	tfr	x,u
	bra	flash_loop

