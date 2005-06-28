/* test program for the following objects:
 *  strstr()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str[31] = "Les sanglots longs des violons";


int main(void)
{
   char *p;

   p = strstr(str, "longs");
   if (p != str+13)
      puts("strstr.c: test1 failed");

   p = strstr(str, "sangsue");
   if (p)
      puts("strstr.c: test2 failed");

   return 0;
}
