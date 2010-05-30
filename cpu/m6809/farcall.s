;;; Copyright 2006, 2007, 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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

;;; Offset from the stack pointer to the farcall parameters.
;;; This should equal the total size of all saved registers at the
;;; point that the parameters are read.
RETADDR=3

;;; The I/O address of the bank switching register.
#if defined (CONFIG_PLATFORM_WPC)
WPC_ROM_PAGE_REG=0x3FFC
#elif defined (CONFIG_PLATFORM_WPCSOUND)
WPC_ROM_PAGE_REG=0x2000
#elif defined (CONFIG_PLATFORM_WHITESTAR)
WPC_ROM_PAGE_REG=0x3200
#else
#error "unknown bank switching register"
#endif

	.module farcall.s

	;;; Declare a fast variable to hold the target address
	.area direct
	.globl __far_call_address
__far_call_address: .blkb 2

	.globl __far_call_page
__far_call_page: .blkb 1

	;;; Notes:
	;;; 1. The 'A' register is clobbered by the call.  This is OK
	;;; since the ABI only define B and X for return codes.
	;;;
	;;; 2. The 'CC' register is not preserved after the call.
	;;;
	;;; 3. This was changed recently to store the call address in memory
	;;; rather than referencing it from ,U.  This allowed us to free U
	;;; before the call, thereby keeping the total stack space down
	;;; at the cost of an extra store.  (In most cases, this is still a
	;;; win as any sleep done by the target will require saving the stack,
	;;; so keeping this small is a big plus.  The call happens once
	;;; but the task might sleep many times over its lifetime.)
	;;;
	;;; Overhead is about 70 cycles as compared to a normal function
	;;; call, plus 3 extra bytes on the stack.
	;;;
	;;; TODO? - put the saved bank into a global so that the callee can
	;;; get to it, for accessing caller data?
	;;;
	.area .text
	.globl __far_call_handler
__far_call_handler:
	pshs	b,u                   ; Save all registers used for parameters
	ldu	RETADDR,s             ; Get pointer to the parameters
	ldd	,u++                  ; Get the called function offset
	std   *__far_call_address   ; Move function offset to memory
	ldb	,u+                   ; Get the called function page
	stu	RETADDR,s			    ; Update return address
	lda	*_wpc_rom_bank        ; Read current bank switch register value
	stb	*_wpc_rom_bank        ; Set new bank switch register value
	stb	WPC_ROM_PAGE_REG      ; Set new bank switch register value
	puls	b,u                   ; Restore parameters
	pshs	a                     ; Save bank switch value to be restored
	jsr	[__far_call_address]  ; Call function
	puls	a                     ; Restore A
	sta	*_wpc_rom_bank        ; Restore bank switch register
	sta	WPC_ROM_PAGE_REG      ; Restore bank switch register
	rts


	;;; Call through a far pointer.  This is identical to above, except the
	;;; address is already in X/B and is not loaded from the program inline.
	;;; Note: this only works if the function does not take parameters.
	;;; Normally it is void (*)(void); it also works if it returns a value
	;;; such as bool (*)(void) .
	.globl _far_indirect_call_handler
	.globl _far_indirect_call_value_handler
_far_indirect_call_handler:
_far_indirect_call_value_handler:
	lda	*_wpc_rom_bank        ; Read current bank switch register value
	stx   *__far_call_address   ; Move function offset to memory
	stb	*_wpc_rom_bank        ; Set new bank switch register value
	stb	WPC_ROM_PAGE_REG      ; Set new bank switch register value
	pshs	a                     ; Save bank switch value to be restored
	jsr	[__far_call_address]  ; Call function
	puls	a                     ; Restore A
	sta	*_wpc_rom_bank        ; Restore bank switch register
	sta	WPC_ROM_PAGE_REG      ; Restore bank switch register
	rts


	.globl _far_call_pointer_handler
_far_call_pointer_handler:
	pshs	b                     ; Save all registers used for parameters
	lda	*_wpc_rom_bank        ; Read current bank switch register value
	ldb	__far_call_page
	stb	*_wpc_rom_bank        ; Set new bank switch register value
	stb	WPC_ROM_PAGE_REG      ; Set new bank switch register value
	puls	b                     ; Restore parameters
	pshs	a                     ; Save bank switch value to be restored
	jsr	[__far_call_address]  ; Call function
	puls	a                     ; Restore A
	sta	*_wpc_rom_bank        ; Restore bank switch register
	sta	WPC_ROM_PAGE_REG      ; Restore bank switch register
	rts


	;;; Read 8-bit value at a far address.  The offset is in X and the page in B.
	;;; The return value is in B.
	.globl _far_read8
_far_read8:
	pshs	a
	lda	*_wpc_rom_bank        ; Read current bank switch register value
	stb	*_wpc_rom_bank        ; Set new bank switch register value
	stb	WPC_ROM_PAGE_REG      ; Set new bank switch register value
	ldb	,x                    ; Read the value
	sta	*_wpc_rom_bank        ; Restore bank switch register
	sta	WPC_ROM_PAGE_REG      ; Restore bank switch register
	puls	a,pc


	;;; Read 16-bit value at a far address.  The offset is in X and the page in B.
	;;; The return value is in X.
	.globl _far_read_pointer
_far_read_pointer:
	.globl _far_read16
_far_read16:
	pshs	a
	lda	*_wpc_rom_bank        ; Read current bank switch register value
	stb	*_wpc_rom_bank        ; Set new bank switch register value
	stb	WPC_ROM_PAGE_REG      ; Set new bank switch register value
	ldx	,x                    ; Read the value
	sta	*_wpc_rom_bank        ; Restore bank switch register
	sta	WPC_ROM_PAGE_REG      ; Restore bank switch register
	puls	a,pc


