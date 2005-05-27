

;;; Bitmap format
;;; Byte 0 = width in bits (equals number of col bytes / 8)
;;; Byte 1 = height in bits (equals number of row bytes)
;;; Bytes 2+ = binary data to fill in, across then down


;;; Font range
;;; Byte 0 = Starting character in ASCII
;;; Byte 1 = Number of characters covered by this font range
;;; Bytes 2+ = pointers to bitmaps for these characters


;;; Font = array of pointers to font ranges with NULL termination entry


   ; X = pointer to bitmap
   ; Y = pointer to destination buffer
   ; A = starting column position
   ; B = starting row position
proc(bitmap_write)
   uses(a,b,x,y)
endp


   ; U = pointer to font
   ; A = character to be displayed
   ; returns X = pointer to bitmap
proc(font_get_bitmap)
   returns(x)
   uses(b,u,y)
   ; Loop all over font ranges to find our character
   loop
      ldy   ,u          ; Get font range pointer
      cmpy  #0000       ; Are we at the end of the list?
      beq   loopexit    ; Yes, exit the loop
      ldb   ,y          ; Get first character in this range
      cmpb  saved(a)    ; Compare with input character
      ifle
         ; Input character is >= first character, it may match
         addb  1,y      ; Get last character in this range
         cmpb  saved(a) ; Compare with input character
         ifge
            ; Input character is <= last character, it is in the range!
            ldb   1,y      ; Get character offset in bytes
            lslb           ; Convert to pointer offset
            ldx   b,y      ; Load the bitmap pointer
            return         ; Exit function
         endif
      endif
   
      ; Not in this range : continue to next range
      leau  2,u
   endloop

   ; Not in any ranges, return error
endp



