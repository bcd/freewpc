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
	stu	9,x
	sty	7,x
	leau	,s
	leay	23,x
	leay	,y
	clrb
	bra	L32
L33:
	lda	,u+
	sta	,y+
	incb
L32:
	cmpu	#6135
	bls	L33
	stb	12,x
	ldb	16380
	stb	11,x
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
	leau	23,u
	leay	,u
	ldu	#6136
	ldb	12,x
	bra	L42
L43:
	lda	,-y
	sta	,-u
	decb
L42:
	tstb
	bne	L43
	leas	,u
	andcc	#-81
	ldb	11,x
	stb	16380
	ldu	5,x
	pshs	u
	ldy	7,x
	ldu	9,x
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
	pshs	d,u
	tfr	x,u
	jsr	_task_allocate
	stu	5,x
	puls	d,u
	sty	7,x
	stu	9,x
	clr	,x
	ldd	#0
	std	17,x
	ldb	16380
	stb	11,x
	clr	12,x
	puls	u,y,pc

