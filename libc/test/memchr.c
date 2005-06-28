/* test program for the following objects:
 *  memchr()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str[31] = "Les sanglots longs des violons";


int main(void)
{
   char *p;

   p = memchr(str, 'g', 31);
   if (p != str+7)
      puts("memchr.c: test1 failed");

   p = memchr(str, 'k', 31);
   if (p)
      puts("memchr.c: test2 failed");

   p = memchr(str, '\0', 31);
   if (p != str+30)
      puts("memchr.c: test3 failed");

   return 0;
}
