/* test program for the following objects:
 *  abort()     function        stdlib.h
 */

#include "stdlib.h"


int func1(int n, int val)
{
   if (--n < 0)
      abort();

   func1(n, val);

   return 0;
}


int main(void)
{
   func1(8, 0xFF);

   return 0;
}
