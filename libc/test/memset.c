/* test program for the following objects:
 *  memset()     function        string.h
 *  memcmp()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str1[10] = "123456789";
char str2[10] = "1234   89";


int main(void)
{
   char *p;
   int ret;

   p = memset(str1+4, ' ', 3);
   if (p != str1+4)
      puts("memmove.c: test1 failed");

   ret = memcmp(str1, str2, 9);
   if (ret != 0)
      puts("memmove.c: test2 failed");

   return 0;
}
