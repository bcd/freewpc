;;;
;;; Copyright 2006 by Brian Dominy <brian@oddchange.com>
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


WPC_DEBUG_PORT = 0x3D60
WPC_LEDS       = 0x3FF2
WPC_ROM_BANK   = 0x3FFC

BOTTOM_BANK    = 0x20
TOP_BANK       = 0x3D

FLASH_DELAY    = 100

PAGED_REGION   = 0x4000
PAGED_SIZE     = 0x4000
FIXED_REGION   = 0x8000
FIXED_SIZE     = 0x8000

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
;;; report the error.  We can't reply on the DMD working
;;; properly to help us here.
;;; 
;;; Once diags prove the system working, ALL of the remaining
;;; code should be moved into a separate page and run from there,
;;; as it is only run once.
;;;

	.area	sysrom
	.globl _start
_start:
	; Disable interrupts (IRQ and FIRQ)
	orcc	#0x50

	; Initialize the direct page pointer.  This hardware register
	; determines where 'direct' addressing instructions are targeted.
	; By setting to zero, direct addresses are mapped to 0000h-00FFh.
	; We can use shorter instructions when referencing variables here.
	clra
	tfr	a,dp

	jmp	ram_test    ; Bypass ROM test; there's a bug here

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   ROM POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rom_test:
	; Initialize checksum
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
	ble	paged_loop

	; Did checksum validate?
	cmpd	#0
	beq	ram_test
rom_checksum_error:
	ldx	#1
	jmp	diag_error


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   RAM POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ram_test:
	ldx	#0
	ldu	#0x55AA
ram_loop1:
	stu	,x
	ldu	,x++
	cmpu	#0x55AA
	bne	ram_error
	cmpx	#0x800
	blo	ram_loop1

	ldx	#0
	ldu	#0xAA55
ram_loop2:
	stu	,x
	ldu	,x++
	cmpu	#0xAA55
	bne	ram_error
	cmpx	#0x800
	blo	ram_loop2
	bra	asic_test

ram_error:
	ldx	#2
	jmp	diag_error


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   ASIC POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
asic_test:

test_done:
	jmp	_do_reset

	;;; Input: X = diagnostic error code
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
	mul
	mul
	mul
	leay	-1,y
	cmpy	#0
	bne	delay_loop

	tfr	x,u
	bra	flash_loop

