/* test program for the following objects:
 *  memcpy()     function        string.h
 *  memcmp()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str1[9] = "familles";
char str2[24] = "        , je vous hais!";
char str3[24] = "familles, je vous hais!";


int main(void)
{
   char *p;
   int ret;

   p = memcpy(str2, str1, 8);
   if (p != str2)
      puts("memcpy.c: test1 failed");

   ret = memcmp(str2, str3, 23);
   if (ret != 0)
      puts("memcpy.c: test2 failed");

   return 0;
}
