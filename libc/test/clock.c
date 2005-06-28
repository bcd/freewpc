/* test program for the following objects:
 *  clock()     function        time.h
 */

#include "conio.h"
#include "stdio.h"
#include "time.h"


int main(void)
{
   clock_t start, tick;
   int i=0;

   start = clock();

   while (!kbhit()) {
      tick = clock();

      if (tick - start > CLOCKS_PER_SEC) {
         printf("%d seconds elapsed.\r", ++i);
         start = tick;
      }
   }

   putchar('\n');

   return 0;
}
