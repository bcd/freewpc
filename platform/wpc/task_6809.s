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
	stu	5,x
	ldu	*_task_save_U
	stu	UREG_SAVE_OFF,x
	sty	YREG_SAVE_OFF,x
	leau	,s
	leay	23,x
	clrb
	bra	L32
L33:
	;;; TODO : use X register to transfer data faster
	;;; TODO : check for overflow during copy
	;;; Q: can we use S directly in this loop instead of U?
	lda	,u+
	sta	,y+
	incb
L32:
	cmpu	#STACK_BASE
	blt	L33
	stb	SAVED_STACK_SIZE,x
	ldb	WPC_ROM_BANK
	stb	ROMPAGE_SAVE_OFF,x
	jmp _task_dispatcher


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
	ldu	5,x
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
	;;; clr	SAVED_STACK_SIZE,x

	;;; TODO?? : push the address of task_exit onto the
	;;; stack so that the task can simply return and it
	;;; will exit automatically.  All tasks will need
	;;; to do this and this will save the code space of
	;;; function call (though replaced by stack space)
	puls	u,y,pc

