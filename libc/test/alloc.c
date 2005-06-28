/* test program for the following objects:
 *  malloc()     function        stdlib.h
 *  free()       function        stdlib.h
 */

#include "stdio.h"
#include "stdlib.h" 

#define N_CHUNKS  100
#define SIZE      1024

int main(void)
{
   void *p;
   void *ap[N_CHUNKS];
   int i, j, max;

   for (i=0; i<N_CHUNKS; i++) {
      p = malloc(SIZE);
      if (!p) {
         puts("alloc.c: test1 failed");
         exit(1);
      }

      free(p);
   }

   for (i=0; i<N_CHUNKS; i++) {
      ap[i] = malloc(SIZE);
      if (!ap[i])
         break;
   }

   if (i == N_CHUNKS) {
      puts("alloc.c: test2 failed");
      exit(1);
   }

   max = i;

   for (i=0; i<max; i++) {
      for (j=0; j<i; j++)
         free(ap[j]);

      for (j=0; j<i; j++) {
         ap[j] = malloc(SIZE);
         if (!ap[j]) {
            puts("alloc.c: test3 failed");
            exit(1);
         }
      }
   }

   return 0;
}
