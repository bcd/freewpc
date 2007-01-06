
	.area		sysrom

WPC_LEDS       = 0x3FF2
WPC_ROM_BANK   = 0x3FFC

BOTTOM_BANK    = 0x20
TOP_BANK       = 0x3D

FLASH_DELAY    = 100

PAGED_REGION   = 0x4000
PAGED_SIZE     = 0x4000
FIXED_REGION   = 0x8000
FIXED_SIZE     = 0x8000

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   ROM POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.globl	_rom_diag
_rom_diag:
	ldx	#4
	jmp	diag_error

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
	blt	fixed_loop

	; Compute checksum for each paged region
	ldy	#TOP_BANK
paged_loop:
	ldx	#0x8000
	exg	d,y
	sta	WPC_ROM_BANK
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
	cmpx	#4000
	blt	paged_inner_loop
	leay	-1,y
	cmpy	#BOTTOM_BANK
	ble	paged_loop

	; Did checksum validate?
	cmpd	#0
	bne	rom_checksum_error
	rts
rom_checksum_error:
	ldx	#1
	jmp	diag_error


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;   RAM POST DIAGNOSTIC CHECK
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.globl	_ram_diag
_ram_diag:
	ldx	#0
	ldu	#0x55AA
ram_loop1:
	stu	,x
	ldu	,x++
	cmpu	#0x55AA
	bne	ram_error
	cmpx	#0x1800
	blt	ram_loop1

	ldx	#0
	ldu	#0xAA55
ram_loop2:
	stu	,x
	ldu	,x++
	cmpu	#0xAA55
	bne	ram_error
	cmpx	#0x1800
	blt	ram_loop2

	rts
ram_error:
	ldx	#2
	jmp	diag_error


	;;; Input: X = diagnostic error code
diag_error:
	ldy	#500
	ldu	#500

flash_loop:
	lda	WPC_LEDS        ; Toggle the LED
	eora	#-128
	sta	WPC_LEDS
	tfr	y,d             ; Save the loop count

outer_flash_loop:        ; Hold the LED state
	ldx	#5000
inner_flash_loop:
	leax	-1,x
	cmpx	#0
	bne	inner_flash_loop
	leay	-1,y
	cmpy	#0
	bne	outer_flash_loop

	tfr	d,y             ; Restore the loop count
	exg	y,u             ; Switch with second count value
	bra	flash_loop      ; Repeat

