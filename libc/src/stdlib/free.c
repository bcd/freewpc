#include "stdlib.h"
#include "libc/malloc.h"


void free(void *ptr)
{
   struct BLOCK *bp;

   if (!ptr)
      return;

   bp = BLOCK_FROM_DATA(ptr);

   bp->prev->next = bp->next;

   if (bp->next)
      bp->next->prev = bp->prev;
}
