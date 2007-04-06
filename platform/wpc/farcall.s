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

;;; Offset from the stack pointer to the farcall parameters.
;;; This should equal the total size of all saved registers at the
;;; point that the parameters are read.
RETADDR=5

;;; I/O address of the bank switching register.
WPC_ROM_PAGE_REG=0x3FFC

	.module farcall.s

	;;; Declare a fast variable to hold the target address
	.area direct
	.globl __far_call_address
__far_call_address: .blkb 2

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
	;;; call.
	;;;
	.area sysrom
	.globl __far_call_handler
__far_call_handler:
	pshs	b,u,x                 ; Save all registers used for parameters
	ldu	RETADDR,s             ; Get pointer to the parameters
	ldx	,u++                  ; Get the called function offset
	ldb	,u+                   ; Get the called function page
	stu	RETADDR,s			    ; Update return address
	stx   *__far_call_address   ; Move function offset to memory
	lda	WPC_ROM_PAGE_REG      ; Read current bank switch register value
	stb	WPC_ROM_PAGE_REG      ; Set new bank switch register value
	puls	b,u,x                 ; Restore parameters
	pshs	a                     ; Save bank switch value to be restored
	jsr	[__far_call_address]  ; Call function
	puls	a                     ; Restore A
	sta	WPC_ROM_PAGE_REG      ; Restore bank switch register
	rts


	.globl __far_indirect_call_handler
__far_indirect_call_handler:
	pshs	b,u,x                 ; Save all registers used for parameters
	stx   *__far_call_address   ; Move function offset to memory
	lda	WPC_ROM_PAGE_REG      ; Read current bank switch register value
	stb	WPC_ROM_PAGE_REG      ; Set new bank switch register value
	puls	b,u,x                 ; Restore parameters
	pshs	a                     ; Save bank switch value to be restored
	jsr	[__far_call_address]  ; Call function
	puls	a                     ; Restore A
	sta	WPC_ROM_PAGE_REG      ; Restore bank switch register
	rts
