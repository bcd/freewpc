
#include <freewpc.h>

__fastram__ uint8_t page_loaded;



uint8_t paged_read_byte (uint8_t *byte_ptr, uint8_t page)
{
}


uint16_t paged_read_word (uint16_t *word_ptr, uint8_t page)
{
}

#if 0
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


#endif
