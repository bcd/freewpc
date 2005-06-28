/* test program for the following objects:
 *  abs()     function        stdlib.h
 */

#include "errno.h"
#include "stdlib.h"
#include "stdio.h"


int main(void)
{
   if (abs(15620) != 15620)
      puts("abs.c: test1 failed");

   if (abs(-254) != 254)
      puts("abs.c: test2 failed");

   return 0;
}
