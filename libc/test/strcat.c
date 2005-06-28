/* test program for the following objects:
 *  strcat()     function        string.h
 *  strcmp()     function        string.h
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

   p = strcat(str1, str2);
   if (p != str1)
      puts("strcat.c: test1 failed");

   ret = strcmp(str1, str3);
   if (ret != 0)
      puts("strcat.c: test2 failed");

   return 0;
}
