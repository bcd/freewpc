/* test program for the following objects:
 *  strcmp()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str1[11] = "contribuer";
char str2[10] = "continuer";


int main(void)
{
   int ret;

   ret = strcmp(str1, str2);

   if (ret <= 0)
      puts("strcmp.c: test1 failed");

   str1[4] = 0;
   str2[4] = 0;

   ret = strcmp(str1, str2);
   if (ret != 0)
      puts("strcmp.c: test2 failed");

   return 0;
}
