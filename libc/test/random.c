/* test program for the following objects:
 *  rand()      function        strlib.h
 *  srand()     function        strlib.h
 */

#include "stdio.h"
#include "stdlib.h"


int main(void)
{
   int i;

   srand(i);

   for (i=0; i<100; i++)
      printf("%d ", rand());

   return 0;
}
