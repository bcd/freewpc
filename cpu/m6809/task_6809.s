;;;
;;; Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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

#include "system/errno.h"

;;; Defines that are in .h files which can't be included directly.
;;; TODO: Put only the constants in .h without the C-structs and it
;;; it *can* be included.
STACK_BASE         = 0x17F5
WPC_ROM_BANK       = 0x3FFC

STATE_OFF          = 0
PCREG_SAVE_OFF     = 3
YREG_SAVE_OFF      = 5
UREG_SAVE_OFF      = 7
ROMPAGE_SAVE_OFF   = 9
SAVED_STACK_SIZE   = 10
AUX_STACK_OFF      = 15
STACK_SAVE_OFF     = 18

; Because we save in multiples of 8 bytes at a time,
; this should always be a multiple of 8 also.
TASK_SMALL_SIZE    = 40
TASK_LARGE_SIZE    = 64

	.module task_6809.s

	;-----------------------------------------------------
	; task_save
	;-----------------------------------------------------
	.area .text
	.globl _task_save
_task_save:
	;;; First, save all of the volatile registers: U, Y, PC, and ROM bank.
	;;; The PC value kept here is actually the address of the
	;;; caller to task_sleep(), since it does "jmp" here. (51 cycles)
	stu	*_task_save_U          ; 5 cycles
	puls	u                      ; 7 cycles
	ldx	*_task_current         ; 5 cycles
	stu	PCREG_SAVE_OFF,x       ; 6 cycles
	ldu	*_task_save_U          ; 5 cycles
	stu	UREG_SAVE_OFF,x        ; 6 cycles
	sty	YREG_SAVE_OFF,x        ; 7 cycles
	ldb	*_wpc_rom_bank         ; 5 cycles
	stb	ROMPAGE_SAVE_OFF,x     ; 5 cycles

	;;; Copy the runtime stack into the task save area.
	;;; Y points to the save area (destination), while S points
	;;; to the live stack (source).  S is modified on the fly so
	;;; no local variables can be used inside this code.

#ifdef PARANOID
	cmps	#STACK_BASE
	bgt	_stack_underflow
#endif

	;;; The total number of bytes to be saved can be precomputed -- it
	;;; is STACK_BASE - s.  If this number is greater than
	;;; TASK_SMALL_SIZE, then more work needs to be done here.
	tfr	s,d                    ; 6 cycles
	subb	#<STACK_BASE           ; 4 cycles
	negb                         ; 2 cycles
	stb	SAVED_STACK_SIZE,x     ; 5 cycles

#ifdef CONFIG_DEBUG_STACK
	;;; For debugging we can track how often tasks sleep with
	;;; various stack sizes, in order to optimize the stack space
	;;; in the task structure.  At present three counters are kept:
	;;; small (0-15), medium (16-24), and large (25+).
	cmpb	#8
	ble	small_stack
	cmpb	#16
	ble	medium_stack

large_stack:
	inc	_task_large_stacks+1
	bne	stack_debug_done
	inc	_task_large_stacks
	bra	stack_debug_done

medium_stack:
	inc	_task_medium_stacks+1
	bne	stack_debug_done
	inc	_task_medium_stacks
	bra	stack_debug_done

small_stack:
	inc	_task_small_stacks+1
	bne	stack_debug_done
	inc	_task_small_stacks

stack_debug_done:
	cmpb	_task_largest_stack
	ble	2$
	stb	_task_largest_stack
2$:
#endif /* CONFIG_DEBUG_STACK */

	; Check for stack too large.  This is currently a hard stop.
	cmpb  #TASK_SMALL_SIZE       ; 2 cycles
	bgt   _stack_too_large       ; 2 cycles

	; Round number of bytes up to the next multiple of 8.
	; Note that this will normally cause some bytes off the real
	; stack (just above STACK_BASE) to be saved (harmless).
	addb	#7                     ; 4 cycles
	andb	#~7                    ; 4 cycles

	; Set the destination address to the top (high address) of
	; the task block.
	leau	STACK_SAVE_OFF+TASK_SMALL_SIZE,x ; 5 cycles

	;;; Copy b blocks of 8-bytes at a time.
	;;; This takes 42 cycles per 8 bytes (about twice as
	;;; fast as before!)
save_loop:
	puls	x,y                    ; 9 cycles
	pshu	x,y                    ; 9 cycles
	puls	x,y                    ; 9 cycles
	pshu	x,y                    ; 9 cycles
	subb	#8	                    ; 4 cycles
	bne	save_loop              ; 2 cycles

	; x was killed in the core copy loop, need to restore it
	ldx	*_task_current         ; 5 cycles

	; Reinitialize the stack pointer for the next task
	lds	#STACK_BASE            ; 4 cycles

	; All done -- dispatch the next task
	jmp   _task_dispatcher

_stack_too_large:
#if 0
	cmpb	#TASK_LARGE_SIZE
	bgt	_stack_large_error

	; Allocate a new block for the task's stack.
	jsr	_stack_expand_stack
	tfr	x,u
	bra	save_loop
#endif

_stack_large_error:
	; When debug support is builtin, dump the contents of
	; the large stack so we can see what is going on,
	; before halting the system.
#ifdef DEBUGGER
	ldx	#_sprintf_buffer
	ldb	,s+
	jsr	_do_sprintf_hex_byte
	ldb	#0x20
	stb	,x+
	stb	,x+
	clr	,x
	jsr	_dbprintf1
#endif
	ldb	#ERR_TASK_STACK_OVERFLOW
	jmp	_fatal

#ifdef PARANOID
_stack_underflow:
	ldb	#ERR_TASK_STACK_UNDERFLOW
	jmp	_fatal
#endif

	;-----------------------------------------------------
	; task_restore
	;-----------------------------------------------------

	.area .text
	.globl _task_restore
_task_restore:
	stx	*_task_current	

	;;; Check if stack restore needed before entering the critical section.
	;;; The only thing that needs to be protected is resetting S
	;;; to STACK_BASE and writing to the stack.
	ldb	SAVED_STACK_SIZE,x

	; Round number of bytes up to the next multiple of 8.
	tfr	b,a                    ; 6 cycles
	addb	#7                     ; 4 cycles
	andb	#~7                    ; 4 cycles

	; Compute the number of extra bytes that were saved
	; for adjusting the stack pointer
	stb	*m0
	suba	*m0
	nega

	; Set the destination address
	leau	STACK_SAVE_OFF+TASK_SMALL_SIZE,x ; 5 cycles
	negb                         ; 2 cycles
	leau	b,u                    ; 5 cycles

	;;; Reset the stack pointer.  Copy all bytes out of the
	;;; save area back to the stack.  At the end, S has the
	;;; correct value (points to top of stack).
	;;;
	;;; Because we may have copied too many bytes during
	;;; save, adjust this to where S was when the save finished.
	lds	#STACK_BASE
	leas	a,s

	;;; Restore stack 8 bytes at a time
1$:
	pulu	x,y                    ; 9 cycles
	pshs	x,y                    ; 9 cycles
	pulu	x,y                    ; 9 cycles
	pshs	x,y                    ; 9 cycles
	addb	#8	                    ; 4 cycles
	bne	1$                     ; 2 cycles

	; x was killed in the core copy loop, need to restore it
	ldx	*_task_current         ; 5 cycles

restore_stack_done:
	;;; Restore volatile registers
	ldb	ROMPAGE_SAVE_OFF,x
	stb	*_wpc_rom_bank
	stb	WPC_ROM_BANK
	ldu	PCREG_SAVE_OFF,x
	pshs	u
	ldy	YREG_SAVE_OFF,x
	ldu	UREG_SAVE_OFF,x

	; Return to the task again.  PC was pushed onto the
	; stack so RTS works normally
	rts

	;-----------------------------------------------------
	; task_create - low level routine to create and
	; initialize a new task block.
	;
	; Input: X = initial PC for new task
	; Output: X = pointer to task block
	;-----------------------------------------------------

	.area .text	
	.globl _task_create
_task_create:
	; This routine is only called from one place in the C code:
	; task_create_gid().  As much as possible, keep this function
	; small and do everything in task_create_gid() that you can.
	; This should only be used for things that need to be done
	; in assembly.
	pshs	u
	tfr	x,u
	jsr	_task_allocate
	stu	PCREG_SAVE_OFF,x
	ldb	*_wpc_rom_bank
	stb	ROMPAGE_SAVE_OFF,x

	;;; Note: we could push the address of task_exit onto the
	;;; stack so that the task could simply return and it
	;;; would exit automatically.  However, this only provides
	;;; convenience to the programmer (no need to remember to
	;;; call task_exit), and it saves code space by way of more
	;;; stack space.  Stack space is more precious and therefore
	;;; shouldn't be used where other means are possible.  So
	;;; don't consider that anymore.
	puls	u,pc


#ifdef CONFIG_TASK_FORK
	;-----------------------------------------------------
	; task_fork - an implementation of the UNIX fork()
	; system call.  It is similar to task_create,
	; but with an implied starting address and the
	; fork() return code semantics.
	;
	; Input: None
	; Output: None
	;-----------------------------------------------------

	.area .text
	.globl _task_fork
_task_fork:
	; Need a temporary in U.
	pshs	u

	; Allocate a new task block for the child,
	; returned in X.
	jsr	_task_allocate

	; Set the child's 'U' register to the
	; starting point, which is the PC on the stack.
	ldu	2,s
	stu	UREG_SAVE_OFF,x

	; Start the child at a thunk, which will
	; make sure the child returns with a different
	; return code (zero).
	ldu	#_task_fork_entry
	stu	PCREG_SAVE_OFF,x

	; Done with U.
	puls	u

	; Ensure the child runs in the same page as
	; the parent.
	ldb	*_wpc_rom_bank
	stb	ROMPAGE_SAVE_OFF,x

	; Return from the parent with nonzero.
	ldb	#1
	rts

	.area .text
	.globl _task_fork_entry
_task_fork_entry:
	; Get the actual start address in the 'U' save
	; area and jump there.  Set the return code to zero
	; so the caller can distinguish between parent and child.
	ldx	UREG_SAVE_OFF,x
	clrb
	jmp	,x
#endif /* CONFIG_TASK_FORK */


	;-----------------------------------------------------
	; task_stack_reset - truncate the stack.
	; This can be called by a function that does not
	; return.
	;
	; Input: None
	; Output: None
	;-----------------------------------------------------

	.area .text
	.globl _task_stack_reset
_task_stack_reset:
	puls	x
	lds	#STACK_BASE
	jmp	,x

