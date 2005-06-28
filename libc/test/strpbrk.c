/* test program for the following objects:
 *  strpbrk()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str[31] = "Les sanglots longs des violons";


int main(void)
{
   char *p;

   p = strpbrk(str, "chat");
   if (p != str+5)
      puts("strpbrk.c: test1 failed");

   p = strpbrk(str, "mux");
   if (p)
      puts("strpbrk.c: test2 failed");

   return 0;
}
