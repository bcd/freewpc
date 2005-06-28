#include "stdlib.h"
#include "libc/malloc.h"


#define FIRST_BLOCK (struct BLOCK *)&_end
#define LAST_BLOCK  (struct BLOCK *)0xE000

static struct BLOCK *head = NULL;


void *malloc(size_t size)
{
   struct BLOCK *bp, *new_bp;
   size_t free_space;

   if (!size)
      return NULL;

   /* initialize head block */
   if (!head) {
      head = FIRST_BLOCK;
      head->prev = NULL;
      head->next = NULL;
      head->size = 0;
   }

   bp = head;

   /* find a free block */
   while (bp) {
      free_space = (char *)(bp->next ? bp->next : LAST_BLOCK) - (char *)NEXT_CONTIGUOUS_BLOCK(bp);
      if (free_space >= size + sizeof(BLOCK)) {
         new_bp = NEXT_CONTIGUOUS_BLOCK(bp);

         /* connect to next block */
         if (bp->next)
            bp->next->prev = new_bp;
         new_bp->next = bp->next;

         /* connect to previous block */
         bp->next = new_bp;
         new_bp->prev = bp;

         new_bp->size = size;
         return (void *)DATA_FROM_BLOCK(new_bp);
      }

      bp = bp->next;
   }

   return NULL; 
}
