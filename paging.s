
.area fastram

page_loaded    .blkb 1


.area sysrom


proc(paged_lda)   ; X/B = far pointer
   local(prev_page)
   uses(b)

   cmpb  page_loaded
   ifne
      ldb   page_loaded
      stb   prev_page
      ldb   saved(b)
      stb   page_loaded
      stb   IO

      lda   ,x

      ldb   prev_page
      stb   page_loaded
      stb   IO
   else
      lda   ,x
   endif
endp


proc(paged_ldy)	; X/B = far pointer
endp


proc(paged_sta)
endp


proc(paged_sty)
endp


proc(paged_jsr)
endp


