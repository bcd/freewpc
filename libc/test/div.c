/* test program for the following objects:
 *  div()     function        stdlib.h
 */

#include "stdlib.h"
#include "stdio.h"


int main(void)
{
   div_t result;

   result = div(-25327, 251);

   if (result.quot != -100)
      puts("div.c: test1 failed");

   if (result.rem != -227)
      puts("div.c: test2 failed");

   return 0;
}
