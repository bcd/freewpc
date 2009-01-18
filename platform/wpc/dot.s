;;; Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

DMD_LOW_BUFFER=0x3800
DMD_HIGH_BUFFER=0x3A00

	.module dot_6809.s

; Keep loop_count in a fast direct memory slot.
#define loop_count *m0

	.area	.text

	;--------------------------------------------------------
	;
	; void dmd_or_page (void);
	;
	;--------------------------------------------------------
	.globl _dmd_or_page
_dmd_or_page:
	pshs	u
	ldx	#DMD_LOW_BUFFER
	ldu	#DMD_HIGH_BUFFER

	ldb	#64
	stb	loop_count
1$:
	ldd	,x
	ora	,u
	orb	1,u
	std	,x

	ldd	2,x
	ora	2,u
	orb	3,u
	std	2,x

	ldd	4,x
	ora	4,u
	orb	5,u
	std	4,x

	ldd	6,x
	ora	6,u
	orb	7,u
	std	6,x

	leax	8,x
	leau	8,u
	dec	loop_count
	bne	1$
	puls	u,pc

	;--------------------------------------------------------
	;
	; void dmd_and_page (void);
	;
	;--------------------------------------------------------
	.globl _dmd_and_page
_dmd_and_page:
	pshs	u
	ldx	#DMD_LOW_BUFFER
	ldu	#DMD_HIGH_BUFFER

	ldb	#64
	stb	loop_count
1$:
	ldd	,x
	anda	,u
	andb	1,u
	std	,x

	ldd	2,x
	anda	2,u
	andb	3,u
	std	2,x

	ldd	4,x
	anda	4,u
	andb	5,u
	std	4,x

	ldd	6,x
	anda	6,u
	andb	7,u
	std	6,x

	leax	8,x
	leau	8,u
	dec	loop_count
	bne	1$
	puls	u,pc

	;--------------------------------------------------------
	;
	; void dmd_xor_page (void);
	;
	;--------------------------------------------------------
	.globl _dmd_xor_page
_dmd_xor_page:
	pshs	u
	ldx	#DMD_LOW_BUFFER
	ldu	#DMD_HIGH_BUFFER

	ldb	#64
	stb	loop_count
1$:
	ldd	,x
	eora	,u
	eorb	1,u
	std	,x

	ldd	2,x
	eora	2,u
	eorb	3,u
	std	2,x

	ldd	4,x
	eora	4,u
	eorb	5,u
	std	4,x

	ldd	6,x
	eora	6,u
	eorb	7,u
	std	6,x

	leax	8,x
	leau	8,u
	dec	loop_count
	bne	1$
	puls	u,pc

