
;;; Simulated 7-segment displays on the dot matrix controller

#include "wpc.h"

/*
 * Segment names
 */
#define SEG_TOP			0x1
#define SEG_UPR_RIGHT	0x2
#define SEG_LWR_RIGHT	0x4
#define SEG_BOT			0x8
#define SEG_LWR_LEFT		0x10
#define SEG_UPR_LEFT		0x20
#define SEG_MID			0x40
#define SEG_VERT			0x80

SEG_RIGHT = (SEG_UPR_RIGHT+SEG_LWR_RIGHT)
SEG_LEFT = (SEG_UPR_LEFT+SEG_LWR_LEFT)

SEG_ROWS = 4
SEG_COLS = 16
SEG_DATA_SIZE = (SEG_ROWS * SEG_COLS)
SEG_PAGES = 4

.area ram

// seg_data::					.blkb	SEG_DATA_SIZE * SEG_PAGES
// seg_active_offset::		.blkb 1
// seg_allocated::			.blkb 1


.area sysrom

msg_hello_world::
	.asciz "FREEWPC"


seg_row_offset::
   .dw   128 * 0
   .dw   128 * 1
   .dw   128 * 2
   .dw   128 * 3


seg_digit_table::
   .db   SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT							; 0
   .db   SEG_RIGHT															; 1
   .db   SEG_TOP+SEG_UPR_RIGHT+SEG_MID+SEG_LWR_LEFT+SEG_BOT		; 2
   .db   SEG_TOP+SEG_MID+SEG_BOT+SEG_RIGHT							; 3
   .db   SEG_UPR_LEFT+SEG_MID+SEG_RIGHT								; 4
   .db   SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT		; 5
   .db   SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID			; 6
   .db   SEG_TOP+SEG_RIGHT													; 7
   .db   SEG_TOP+SEG_MID+SEG_BOT+SEG_LEFT+SEG_RIGHT				; 8
   .db   SEG_TOP+SEG_MID+SEG_BOT+SEG_UPR_LEFT+SEG_RIGHT			; 9

seg_alpha_table::
   .db   SEG_LEFT+SEG_TOP+SEG_MID+SEG_RIGHT							; A
   .db   SEG_LEFT+SEG_MID+SEG_BOT+SEG_LWR_RIGHT						; B
   .db   SEG_TOP+SEG_LEFT+SEG_BOT										; C
   .db   SEG_RIGHT+SEG_BOT+SEG_LWR_LEFT+SEG_MID						; D
   .db   SEG_LEFT+SEG_TOP+SEG_MID+SEG_BOT								; E
   .db   SEG_LEFT+SEG_TOP+SEG_MID										; F
   .db   SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID			; G
	.db	SEG_LEFT+SEG_RIGHT+SEG_MID										; H
	.db	SEG_TOP+SEG_VERT+SEG_BOT										; I
	.db	SEG_RIGHT+SEG_BOT													; J
	.db	0																		; K
	.db	SEG_LEFT+SEG_BOT													; L
	.db	SEG_LEFT+SEG_TOP+SEG_VERT+SEG_RIGHT							; M
	.db	SEG_LEFT+SEG_TOP+SEG_RIGHT										; N
   .db   SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT							; O
	.db	SEG_LEFT+SEG_TOP+SEG_UPR_RIGHT+SEG_MID						; P
	.db	SEG_UPR_LEFT+SEG_TOP+SEG_MID+SEG_RIGHT						; Q
	.db	SEG_LEFT+SEG_TOP													; R
   .db   SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT		; S
	.db	SEG_TOP+SEG_VERT													; T
	.db	SEG_LEFT+SEG_BOT+SEG_RIGHT										; U
	.db	0																		; V
	.db	SEG_LEFT+SEG_BOT+SEG_VERT+SEG_RIGHT							; W
	.db	0																		; X
	.db	SEG_UPR_LEFT+SEG_MID+SEG_BOT+SEG_RIGHT						; Y 
   .db   SEG_TOP+SEG_UPR_RIGHT+SEG_MID+SEG_LWR_LEFT+SEG_BOT		; Z


proc(seg_init)
	uses(a,x,y)

	//ldx	#seg_data
	//ldy	#SEG_DATA_SIZE * SEG_PAGES
	//jsr	bzerol
	//clra
	//sta	seg_active_offset
	//lda	#0x01
	//sta	seg_allocated
	ldy	#msg_hello_world
	jsr	seg_write_string
	
	//ldx	#seg_update
	//jsr	task_create
endp


#if 0
	; A = active page code
proc(seg_set_active)
	uses(a)
	anda	#0xC0
	sta	seg_active_offset
endp

	; Returns A = active page code
proc(seg_get_active)
	lda	seg_active_offset
endp

	; Returns A = free page code
proc(seg_alloc)
	lda	seg_allocated

	bita	#0x01
	ifz	
		ora	#0x01
		sta	seg_allocated
		lda	#0
		return
	endif

	bita	#0x02
	ifz
		ora	#0x02
		sta	seg_allocated
		lda	#0x40
		return
	endif

	bita	#0x04
	ifz
		ora	#0x04
		sta	seg_allocated
		lda	#0x80
		return
	endif

	bita	#0x08
	ifz
		ora	#0x08
		sta	seg_allocated
		lda	#0xC0
	endif
endp


proc(seg_free)
	uses(a)
	switch(a)
		case(0x0)
			lda	#0x01
		endcase
		case(0x40)
			lda	#0x02
		endcase
		case(0x80)
			lda	#0x04
		endcase
		case(0xC0)
			lda	#0x08
		endcase
	endswitch
	coma
	anda	seg_allocated
	sta	seg_allocated
endp


proc(seg_update)
	loop
		jsr	c_task_sleep(TIME_66MS)
		
		ldy	#seg_data
		lda	seg_active_offset
		leay	a,y

		ldx	#DMD_LOW_BASE

		clra
		loop
			ldb	,y+
			jsr	seg_set
			inca
			cmpa	#SEG_DATA_SIZE
		while(lt)

	endloop
endp
#endif


   ; A = segment number (row in upper nibble, col in lower nibble)
   ; B = segments to set (all others will be cleared)
proc(seg_set)
	requires(a,b)
	uses(a,b,x,y)
   
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   ; Calculate address of upper left corner byte, based
   ; on the row/col desired
   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   lsra
   lsra
   lsra
   lsra                    ; A = row number
   lsla                    ; A = byte offset into seg_row_offset
	ldx	#DMD_LOW_BASE		/* Always write to low mapped page */
	ldy	#seg_row_offset
   leay  a,y
	ldd	,y						; Y = row offset into page
   leax  d,x               ; X = pointer to beginning of row
   lda   saved(a)
   anda  #0x0f             ; A = col number
   leax  a,x               ; X = pointer to beginning of segments

   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   ; Check each bit in the segment mask and draw as necessary
   ; (on or off)
   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   clra
	ldb	saved(b)
   bitb  #SEG_TOP
   ifnz
      lda   #0xfc
   endif
   sta   ,x

   clra
   bitb  #SEG_UPR_LEFT
   ifnz
      ora   #0x4
   endif
	bitb	#SEG_VERT
	ifnz
		ora	#0x10
	endif
   bitb  #SEG_UPR_RIGHT
   ifnz
      ora   #0x80
   endif
   sta   16 * 1,x
   sta   16 * 2,x

   clra
   bitb  #SEG_MID
   ifnz
      lda   #0xfc
   endif
   sta   16 * 3,x

   clra
   bitb  #SEG_LWR_LEFT
   ifnz
      ora   #0x4
   endif
	bitb	#SEG_VERT
	ifnz
		ora	#0x10
	endif
   bitb  #SEG_LWR_RIGHT
   ifnz
      ora   #0x80
   endif
   sta   16 * 4,x
   sta   16 * 5,x

   clra
   bitb  #SEG_BOT
   ifnz
      lda   #0xfc
   endif
   sta   16 * 6,x
endp


	;A = segment number
	; Returns pointer to data bits in X
proc(seg_get_data)
	uses(a,b)
endp

	; B = character
	; X = display page pointer
	; Returns segment data in B
proc(seg_translate_char)
	returns(b)
	uses(u)
	cmpb	#'0;'
	ifhs
		cmpb	#'9;'
		ifls
			;;; Digit (0-9)
			subb 	#'0;'
			ldu	#seg_digit_table
			leau	b,u
			ldb	,u
			return
		endif
	endif

	cmpb	#'A;'
	ifhs
		cmpb	#'Z;'
		ifls
			;;; Alphabetic (A-Z)
			subb	#'A;'
			ldu	#seg_alpha_table
			leau	b,u
			ldb	,u
			return
		endif
	endif

	cmpb	#0x20
	ifeq
		clrb
		return
	endif

	ldb	#SEG_MID
endp


	; A = segment number
	; B = binary decimal number (two hex digits)
proc(seg_write_bcd)
   uses(a,b)
   lsrb
   lsrb
   lsrb
   lsrb                 ; B = upper nibble of byte value
   jsr   seg_write_digit
   inca                 ; Advance to next column position
   ldb   saved(b)       ; Reload original byte value
   andb  #0x0f          ; B = lower nibble of byte value
   jsr   seg_write_digit
endp


	; A = segment number
	; B = integer value (0-99)
proc(seg_write_short)
	uses(a,b)
	local(byte,tmp)
	endlocal

	tfr	b,a
	jsr	div10						/* A = first digit, B = second digit */
	exg	a,b						/* B = first digit, A = second digit */
	sta	tmp						/* Save second digit for later */
	lda	saved(a)
	jsr	seg_write_digit		/* Write first digit */
	inca
	ldb	tmp
	jsr	seg_write_digit		/* Write second digit */
endp


	; A = starting segment number
	; B = hex digit
proc(seg_write_digit)
	uses(b)

	cmpb	#10
	iflt
		addb	#'0;'
	else
		subb	#10
		addb	#'A;'
	endif
	bsr	seg_write_char
endp


	; A = starting segment number
	; B = character
proc(seg_write_char)
	uses(b,x)
	tstb
	ifnz
		jsr	seg_translate_char
		jsr	seg_set
	endif
endp


	; A = starting segment number
	; Y = string pointer
proc(seg_write_string)
	uses(a,b,y)
	loop
		ldb	,y+

		tstb
		ifz
			return
		endif

		jsr	seg_translate_char
		jsr	seg_set
		inca
	endloop
endp

	; A = starting segment number
	; Y = length
proc(seg_erase)
	uses(a,b,y)
	clrb
	loop
		jsr	seg_set
		inca
		leay	-1,y
		cmpy	#0
	while(nz)
endp


	; A = source segment
	; B = destination segment
	; Y = length
proc(seg_copy)
endp

