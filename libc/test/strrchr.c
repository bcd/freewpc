/* test program for the following objects:
 *  strrchr()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str[31] = "Les sanglots longs des violons";


int main(void)
{
   char *p;

   p = strrchr(str, 'g');
   if (p != str+16)
      puts("strrchr.c: test1 failed");

   p = strrchr(str, 'k');
   if (p)
      puts("strrchr.c: test2 failed");

   p = strrchr(str, '\0');
   if (p != str+30)
      puts("strrchr.c: test3 failed");

   return 0;
}
