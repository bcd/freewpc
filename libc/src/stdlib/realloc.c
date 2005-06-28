#include "stdlib.h"
#include "string.h"
#include "libc/malloc.h"
#include "sys/types.h"


void *realloc(void *ptr, size_t size)
{
   struct BLOCK *bp;
   void *p;

   if (!ptr)
      return malloc(size);

   if (!size) {
      free(ptr);
      return NULL;
   }

   bp = BLOCK_FROM_DATA(ptr);

   if (size <= bp->size) {
      bp->size = size;
      return ptr;
   }

   p = malloc(size);
   if (p) {
      memcpy(p, ptr, size);
      free(ptr);
   }

   return p;
}
