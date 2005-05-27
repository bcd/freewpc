
.area sysrom

; Set = list of byte values terminated by 0xFF
; Range = two byte values denoting inclusive range

   ; Y = address of lamp set
   ; X = function to apply
proc(lamp_apply_set)
   uses(a,y)
   loop
      lda   ,y+
      switch(a)
         case(0xff)
            return
         endcase

         case(0xfe)
            lda   ,y+
            ldb   ,y+
            jsr   lamp_apply_range
         endcase

			case(0xfd)
				lda	#0
				ldb	#64
            jsr   lamp_apply_range
			endcase

	      jsr   ,x

      endswitch
   endloop
endp


   ; A = minimum value
   ; B = maximum value
   ; X = function to apply
proc(lamp_apply_range)
   uses(a,b)
   loop
      jsr   ,x
      cmpa  saved(b)
      ifeq
         return
      endif
      inca
   endloop
endp
