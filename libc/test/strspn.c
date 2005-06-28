/* test program for the following objects:
 *  strspn()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str[31] = "Les sanglots longs des violons";


int main(void)
{
   size_t len;

   len = strspn(str, "sangles et L");
   if (len != 9)
      puts("strspn.c: test1 failed");

   len = strspn(str, str);
   if (len != 30)
      puts("strspn.c: test2 failed");

   return 0;
}
