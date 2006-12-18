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

STACK_BASE         = 6133
WPC_ROM_BANK       = 0x3FFC
PCREG_SAVE_OFF     = 5
YREG_SAVE_OFF      = 7
UREG_SAVE_OFF      = 9
ROMPAGE_SAVE_OFF   = 11
SAVED_STACK_SIZE   = 12

	;-----------------------------------------------------
	; task_save
	;-----------------------------------------------------
	.area sysrom	
	.globl _task_save
_task_save:
	stu	*_task_save_U
	puls	u
	ldx	*_task_current
	stu	PCREG_SAVE_OFF,x
	ldu	*_task_save_U
	stu	UREG_SAVE_OFF,x
	sty	YREG_SAVE_OFF,x
	leau	,s
	leay	23,x
	clrb
	bra	L32
L33:
	;;; TODO : use X register to transfer data faster
	;;; Q: can we use S directly in this loop instead of U?
	lda	,u+
	sta	,y+
	incb
L32:
	cmpu	#STACK_BASE
	blt	L33
	stb	SAVED_STACK_SIZE,x

	;;; Now it is possible that the task yielded control deep
	;;; into its stack, so that the copy above overflowed the
	;;; stack size within the task structure.  This is BAD.
	;;; Check for this now and halt the system immediately.
	;;; TODO: We could do this check before the loop, by
	;;; comparing U against STACK_BASE-size.
	cmpb  #56
	bgt   _stack_too_large

	;;; TODO??? would it make more sense to save this every time
	;;; the ROM bank value changes, i.e. have the farcall handler
	;;; do this?  It seems like most times when we come here, the
	;;; current ROM bank and the saved value are already equal.
	ldb	WPC_ROM_BANK
	stb	ROMPAGE_SAVE_OFF,x
	jmp _task_dispatcher
_stack_too_large:
	ldb	#99
	jmp	_fatal

	;-----------------------------------------------------
	; task_restore
	;-----------------------------------------------------

	.area sysrom	
	.globl _task_restore
_task_restore:
	stx	*_task_current	
	orcc	#80
	ldb	12,x
	clra
	leau	d,x
	leay	23,u
	ldu	#STACK_BASE
	ldb	SAVED_STACK_SIZE,x
	bra	L42
L43:
	;;; TODO : use X register to transfer data
	lda	,-y
	sta	,-u
	decb
L42:
	tstb
	bne	L43
	leas	,u
	andcc	#-81
	ldb	ROMPAGE_SAVE_OFF,x
	stb	WPC_ROM_BANK
	ldu	PCREG_SAVE_OFF,x
	pshs	u
	ldy	YREG_SAVE_OFF,x
	ldu	UREG_SAVE_OFF,x
	clr	14,x
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
	pshs	u,y
	pshs	u
	tfr	x,u
	jsr	_task_allocate
	stu	5,x
	puls	u
	sty	YREG_SAVE_OFF,x
	stu	UREG_SAVE_OFF,x
	clr	,x
	ldd	#0
	std	17,x
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
	puls	u,y,pc

