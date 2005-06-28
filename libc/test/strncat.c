/* test program for the following objects:
 *  strncat()     function        string.h
 *  strcmp()      function        string.h
 *  strncmp()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str1[24] = "familles";
char str2[16] = ", je vous hais!";
char str3[24] = "familles, je vous hais!";


int main(void)
{
   char *p;
   int ret;

   p = strncat(str1, str2, 32);
   if (p != str1)
      puts("strncat.c: test1 failed");

   ret = strcmp(str1, str3);
   if (ret != 0)
      puts("strncat.c: test2 failed");

   str1[8] = '\0';
   str1[23] = ' ';

   p = strncat(str1, str2, 15);
   if (p != str1)
      puts("strncat.c: test3 failed");

   ret = strcmp(str1, str3);
   if (ret != 0)
      puts("strncat.c: test4 failed");

   str1[8] = '\0';
   p = strncat(str1, str2, 4);
   if (p != str1)
      puts("strncat.c: test5 failed");

   ret = strcmp(str1, str3);
   if (ret == 0)
      puts("strncat.c: test6 failed");

   ret = strncmp(str1, str3, 12);
   if (ret != 0)
      puts("strncat.c: test7 failed");

   return 0;
}
