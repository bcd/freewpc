/* test program for the following objects:
 *  memmove()    function        string.h
 *  memcmp()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str1[17] = "   <-- texte -->";
char str2[17] = "<-- texte -->-->";
char str3[17] = "<--<-- texte -->";


int main(void)
{
   char *p;
   int ret;

   p = memmove(str1, str1+3, 13);
   if (p != str1)
      puts("memmove.c: test1 failed");

   ret = memcmp(str1, str2, 16);
   if (ret != 0)
      puts("memmove.c: test2 failed");

   p = memmove(str1+3, str1, 13);
   if (p != str1+3)
      puts("memmove.c: test3 failed");

   ret = memcmp(str1, str3, 16);
   if (ret != 0)
      puts("memmove.c: test4 failed");

   return 0;
}
