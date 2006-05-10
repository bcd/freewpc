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

;;; Offset from the stack pointer to the farcall parameters.
;;; This should equal the total size of all saved registers at the
;;; point that the parameters are read.
RETADDR=5

;;; I/O address of the bank switching register.
WPC_ROM_PAGE_REG=0x3FFC

	.area sysrom
	.globl __far_call_handler
__far_call_handler:
	pshs	u                  ; Save U reg like prologue
	pshs	b,x                ; Save all registers used for parameters
	ldu	RETADDR,s          ; Get pointer to the parameters
	ldx	,u++               ; Get the called function offset
	ldb	,u+                ; Get the called function page
	stu	RETADDR,s			 ; Update return address
	tfr	x,u                ; Move function offset to non-parameter reg
	lda	WPC_ROM_PAGE_REG   ; Read current bank switch register value
	stb	WPC_ROM_PAGE_REG   ; Set new bank switch register value
	puls	b,x                ; Restore parameters
	pshs	a                  ; Save bank switch value to be restored
	jsr	,u                 ; Call function
	puls	a                  ; Restore A
	sta	WPC_ROM_PAGE_REG   ; Restore bank switch register
	puls	u,pc               ; Restore U reg, like epilogue, and return

