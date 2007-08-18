;;;
;;; Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

STACK_BASE     = 6133

;;; Hardware registers needed
WPC_DEBUG_PORT = 0x3D60
WPC_LEDS       = 0x3FF2
WPC_ROM_BANK   = 0x3FFC
WPC_RAM_LOCK   = 0x3FFD
WPC_RAM_LOCKSIZE = 0x3FFE
WPC_RAM_UNLOCKED = 0xB4
WPC_RAM_LOCK_2K = 0x1

;;; The ROM bank value for the lowest page this ROM uses
;;; TODO : this has to be configurable.
BOTTOM_BANK    = 0x20

;;; The ROM bank value for the highest page this ROM uses,
;;; excluding the fixed system pages.
TOP_BANK       = 0x3D

FLASH_DELAY    = 100

PAGED_REGION   = 0x4000
PAGED_SIZE     = 0x4000
FIXED_REGION   = 0x8000
FIXED_SIZE     = 0x8000

CKSUM_REV      = 0xFFEE

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
;;; Once diags prove the system working, ALL of the remaining
;;; code should be moved into a separate page and run from there,
;;; as it is only run once.
;;;


	;;;
	;;; start - reset entry point
	;;;
	.module start.s
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

	; Compute checksum for fixed region (0x8000-0xFFFF).
	ldx	#0
fixed_loop:
	addb	,-x
	adca	#0
	addb	,-x
	adca	#0
	addb	,-x
	adca	#0
	addb	,-x
	adca	#0
	cmpx	#0x8000
	bhi	fixed_loop

	; Compute checksum for each paged region
	ldy	#TOP_BANK
paged_loop:
	ldx	#0x8000
	exg	d,y
	stb	WPC_ROM_BANK
	exg	d,y

	; Toggle LED occasionally
	tfr	d,u
	ldb	WPC_LEDS
	eorb	#-128
	stb	WPC_LEDS
	tfr	u,d
paged_inner_loop:
	addb	,-x
	adca	#0
	addb	,-x
	adca	#0
	addb	,-x
	adca	#0
	addb	,-x
	adca	#0
	cmpx	#0x4000
	bhi	paged_inner_loop
	leay	-1,y
	cmpy	#BOTTOM_BANK
	bhi	paged_loop

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
	ldu	#0x55AA
ram_loop1:
	stu	,x
	ldu	,x++
	cmpu	#0x55AA
	bne	ram_error
	cmpx	#0x1700
	blo	ram_loop1

	ldx	#0
	ldu	#0xAA55
ram_loop2:
	stu	,x
	ldu	,x++
	cmpu	#0xAA55
	bne	ram_error
	cmpx	#0x1700
	blo	ram_loop2

	ldx	#0
	ldu	#0
ram_loop3:
	stu	,x
	ldu	,x++
	cmpu	#0
	bne	ram_error
	cmpx	#0x1700
	blo	ram_loop3
	bra	asic_test

ram_error:
	ldx	#2
	jmp	diag_error


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   ASIC POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
asic_test:

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   IRQ POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
irq_test:
	; TODO : set irq_function to a custom IRQ handler that
	; does nothing but increment a counter.  Use hand-tuned
	; delays, then verify that the IRQ is firing at (about)
	; the expected rate.

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   FIRQ POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
firq_test:
	; TODO

	;;; END OF DIAGNOSTICS
test_done:
	; Initialize the stack pointer.  We can now make
	; function calls!  Note that this stack is only used
	; for execution that is not task-based.  Once tasks
	; can be run, each task will use its own stack pointer
	; separate from this one.
	;
	; The initial stack pointer is shifted down from the
	; available stack size, because do_reset() may
	; use local variables, and will assume that it already
	; has space allocated for them; the naked attribute prevents
	; them from being allocated explicitly.
	lds	#STACK_BASE-8
	jmp	_do_reset   ; Jump into C code


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

	ldy	#0x6000
outer_flash_loop:        ; Hold the LED state
	mul
	ldb	WPC_LEDS
	stb	WPC_LEDS
	leay	-1,y
	cmpy	#0
	bne	outer_flash_loop

	leau	-1,u
	cmpu	#0
	bne	flash_loop

	ldy	#0xFFFF
delay_loop:
	; Use 'mul' instructions here because they are the
	; shortest instructions that give the longest delay.
	; The actual values computed don't matter (but
	; note that it destroys values in D).
	mul
	mul
	mul
	leay	-1,y
	cmpy	#0
	bne	delay_loop

	tfr	x,u
	bra	flash_loop

