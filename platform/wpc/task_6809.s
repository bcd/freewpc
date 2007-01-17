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

STACK_BASE         = 6133
WPC_ROM_BANK       = 0x3FFC
PCREG_SAVE_OFF     = 5
YREG_SAVE_OFF      = 7
UREG_SAVE_OFF      = 9
ROMPAGE_SAVE_OFF   = 11
SAVED_STACK_SIZE   = 12
FLAGS_OFF          = 13
DELAY_OFF          = 14
ARG_OFF            = 17
STACK_SAVE_OFF     = 23

	;-----------------------------------------------------
	; task_save
	;-----------------------------------------------------
	.area sysrom	
	.globl _task_save
_task_save:
	;;; First, save all of the volatile registers: U, Y, and PC.
	;;; The PC value kept here is actually the address of the
	;;; caller to task_sleep(), since it does "jmp" here.
	stu	*_task_save_U
	puls	u
	ldx	*_task_current
	stu	PCREG_SAVE_OFF,x
	ldu	*_task_save_U
	stu	UREG_SAVE_OFF,x
	sty	YREG_SAVE_OFF,x

	;;; Copy the runtime stack into the task save area.
	;;; For efficiency, copy 4 bytes at a time, even if some of the
	;;; bytes are garbage.  After this loop, B contains the number
	;;; of 4-byte blocks saved.  Y points to the save area.
	leay	STACK_SAVE_OFF,x
	clrb
	bra	save_stack_check
save_stack:
	ldu	,s++
	stu	,y++
	ldu	,s++
	stu	,y++
	incb
save_stack_check:
	cmps	#STACK_BASE
	blt	save_stack

	;;; Convert B from blocks to bytes
	aslb
	aslb

	;;; We may have copied too much.  The S register needs to be
	;;; equal to STACK_BASE, and B needs to be adjusted down, too.
backtrack:
	cmps	#STACK_BASE
	beq   backtrack_not_necessary
	decb
	cmps	#STACK_BASE+1
	beq	backtrack_done
	decb
	cmps	#STACK_BASE+2
	beq	backtrack_done
	decb
	;;; S had better be STACK_BASE+3 here!

	;;; Save the number of bytes truly saved.
backtrack_done:
	lds	#STACK_BASE
backtrack_not_necessary:
	stb	SAVED_STACK_SIZE,x

	;;; Now it is possible that the task yielded control deep
	;;; into its stack, so that the copy above overflowed the
	;;; stack size within the task structure.  This is BAD.
	;;; Check for this now and halt the system immediately.
	;;; TODO: We could do this check before the loop, by
	;;; comparing U against STACK_BASE-size.
	;;;
	;;; TODO: Even better, don't allocate so many bytes by
	;;; default, since most tasks won't use this much.
	;;; Start with a stack size that works in most cases, and
	;;; allow it to "grow" larger if necessary.  However, a check
	;;; for exceeding this maximum is still required.
	cmpb  #56
	bgt   _stack_too_large

	;;; TODO??? would it make more sense to save this every time
	;;; the ROM bank value changes, i.e. have the farcall handler
	;;; do this?  It seems like most times when we come here, the
	;;; current ROM bank and the saved value are already equal.
	ldb	WPC_ROM_BANK
	stb	ROMPAGE_SAVE_OFF,x
	jmp   _task_dispatcher
_stack_too_large:
	ldb	#2    ; ERR_TASK_STACK_OVERFLOW
	jmp	_fatal

	;-----------------------------------------------------
	; task_restore
	;-----------------------------------------------------

	.area sysrom	
	.globl _task_restore
_task_restore:
	stx	*_task_current	
	orcc	#80
	lds	#STACK_BASE
	ldb	SAVED_STACK_SIZE,x
	beq	L42
	leay	b,x
	leay	STACK_SAVE_OFF,y
L43:
	;;; TODO : use X register to transfer data
	lda	,-y
	sta	,-s
	decb
	bne	L43
L42:
	andcc	#-81
	ldb	ROMPAGE_SAVE_OFF,x
	stb	WPC_ROM_BANK
	ldu	PCREG_SAVE_OFF,x
	pshs	u
	ldy	YREG_SAVE_OFF,x
	ldu	UREG_SAVE_OFF,x
	clr	DELAY_OFF,x
	rts

	;-----------------------------------------------------
	; task_create
	;
	; Input: X = initial PC for new task
	; Output: X = pointer to task block
	;-----------------------------------------------------

	.area sysrom	
	.globl _task_create
_task_create:
	pshs	u
	tfr	x,u
	jsr	_task_allocate
	stu	PCREG_SAVE_OFF,x
	puls	u
	clr	,x
	ldd	#0
	std	ARG_OFF,x
	ldb	WPC_ROM_BANK
	stb	ROMPAGE_SAVE_OFF,x

	;;; Note: we could push the address of task_exit onto the
	;;; stack so that the task could simply return and it
	;;; would exit automatically.  However, this only provides
	;;; convenience to the programmer (no need to remember to
	;;; call task_exit), and it saves code space by way of more
	;;; stack space.  Stack space is more precious and therefore
	;;; shouldn't be used where other means are possible.  So
	;;; don't consider that anymore.
	rts

