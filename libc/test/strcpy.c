/* test program for the following objects:
 *  strcpy()     function        string.h
 *  strcmp()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str1[9] = "familles";
char str2[24] = "        , je vous hais!";
char str3[24] = "familles\0 je vous hais!";


int main(void)
{
   char *p;
   int ret;

   p = strcpy(str2, str1);
   if (p != str2)
      puts("strcpy.c: test1 failed");

   ret = strcmp(str2, str3);
   if (ret != 0)
      puts("strcpy.c: test2 failed");

   return 0;
}
