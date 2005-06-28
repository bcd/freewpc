/* test program for the following objects:
 *  strncpy()    function        string.h
 *  strcmp()     function        string.h
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

   p = strncpy(str2, str1, 8);
   if (p != str2)
      puts("strncpy.c: test1 failed");

   ret = strcmp(str2, str3);
   if (ret != 0)
      puts("strncpy.c: test2 failed");

   p = strncpy(str2, str1, 32);
   if (p != str2)
      puts("strncpy.c: test3 failed");

   ret = strcmp(str2, str3);
   if (ret == 0)
      puts("strncpy.c: test4 failed");

   return 0;
}
