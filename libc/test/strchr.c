/* test program for the following objects:
 *  strchr()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str[31] = "Les sanglots longs des violons";


int main(void)
{
   char *p;

   p = strchr(str, 'g');
   if (p != str+7)
      puts("strchr.c: test1 failed");

   p = strchr(str, 'k');
   if (p)
      puts("strchr.c: test2 failed");

   p = strchr(str, '\0');
   if (p != str+30)
      puts("strchr.c: test3 failed");

   return 0;
}
