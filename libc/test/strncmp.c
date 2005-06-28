/* test program for the following objects:
 *  strncmp()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str1[11] = "contribuer";
char str2[10] = "continuer";


int main(void)
{
   int ret;

   ret = strncmp(str1, str2, 32);

   if (ret <= 0)
      puts("strncmp.c: test1 failed");

   ret = strncmp(str1, str2, 4);
   if (ret != 0)
      puts("strncmp.c: test2 failed");

   return 0;
}
