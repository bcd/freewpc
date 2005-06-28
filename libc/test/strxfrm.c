/* test program for the following objects:
 *  strxfrm()    function        string.h
 *  strcmp()     function        string.h
 */

#include <stdio.h>
#include <string.h>


char str1[9] = "familles";
char str2[24] = "        , je vous hais!";
char str3[24] = "familles\0 je vous hais!";


int main(void)
{
   int ret;

   ret = strxfrm(str2, str1, 24);
   if (ret != 8)
      puts("strxfrm.c: test1 failed");

   ret = strcmp(str2, str3);
   if (ret != 0)
      puts("strxfrm.c: test2 failed");

   ret = strxfrm(str2, str1, 4);
   if (ret != 8)
      puts("strxfrm.c: test3 failed");

   ret = strcmp(str2, str3);
   if (ret != 0)
      puts("strxfrm.c: test4 failed");

   ret = strxfrm(str2, str1, 0);
   if (ret != 8)
      puts("strxfrm.c: test5 failed");

   return 0;
}
