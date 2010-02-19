;;;
;;; Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

	.module	breakpoint.s

RTS_OPCODE=0x39
JMP_OPCODE=0x7E
COMMON_PAGE=56

	.area direct
	;
	; bpt_handler is a buffer filled with executable code.
	; When a bpt() check is reached, the code here is called as a function.
	; By programming 'RTS' here, the breakpoint check can be kept short.
	;
bpt_handler:: .blkb 3


	.area	ram
	;
	; bpt_addr is the code address at which the system should be stopped
	; and the debugger entered.
	;
_bpt_addr::	.blkb 3

	;
	; These variables hold the values of the registers at the time of
	; a breakpoint, so they can be restored properly on exit.
	;
bpt_dreg:	.blkb 2
bpt_xreg:	.blkb 2
bpt_yreg:	.blkb 2
bpt_ureg:	.blkb 2

	;
	; Likewise, this is the system clock time when a breakpoint occurred.
	;
bpt_time:   .blkb 2


	.area .text

	; When the breakpoint is reset, the breakpoint handler gets an RTS
	; instruction installed there, so the overhead of the call is
	; minimal.  Each call is only 7+5=12 CPU cycles.
_bpt_init::
_bpt_reset::
	lda	#RTS_OPCODE
	sta	*bpt_handler
	rts

	; When a breakpoint is set, the bpt_check is installed in the buffer
	; via a JMP instruction.  The overhead of each check is 4 extra cycles
	; for the JMP, plus the time it takes bpt_check to see if it is the
	; desired break address.
_bpt_set::
	leax	2,x
	stx	_bpt_addr
	stb	_bpt_addr+2
	lda	#JMP_OPCODE
	sta	*bpt_handler
	ldx	#_bpt_check
	stx	*bpt_handler+1
	rts

	; Check if the caller's address matches the installed breakpoint.
	; Take care not to destroy any registers in the process.
_bpt_check::
	stx	bpt_xreg             ; Ensure X is preserved
	ldx	,s                   ; Get the return address
	cmpx	_bpt_addr            ; Does this match the installed breakpoint?
	bne	bpt_check_exit       ; Nope, return

_bpt_stop::
	sty	bpt_yreg
	stu	bpt_ureg
	std	bpt_dreg

	ldx	_sys_time            ; Save system time
	stx	bpt_time

	jsr	__far_call_handler   ; Handle the breakpoint hit
		.dw	_bpt_hit
		.db	COMMON_PAGE

	ldx	bpt_time             ; Restore system time
	stx	_sys_time
bpt_check_exit:
	ldx	bpt_xreg             ; Restore X and return
	rts

