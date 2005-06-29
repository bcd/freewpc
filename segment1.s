
#include <freewpc.h>


.area sysrom

seg_row_offset::
   .dw   128 * 0
   .dw   128 * 1
   .dw   128 * 2
   .dw   128 * 3


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


