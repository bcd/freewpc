/* test program for the following objects:
 *  memcmp()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str1[11] = "contribuer";
char str2[10] = "continuer";


int main(void)
{
   int ret;

   ret = memcmp(str1, str2, 10);

   if (ret <= 0)
      puts("memcmp.c: test1 failed");

   ret = memcmp(str1, str2, 4);
   if (ret != 0)
      puts("memcmp.c: test2 failed");

   return 0;
}
