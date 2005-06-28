/* test program for the following objects:
 *  sprintf()     function        stdio.h
 */

#include "stdio.h"
#include "string.h"

char str1[19] = "Nous sommes 5 ici.";
char str2[19];

int main(void)
{
   int ret;

   sprintf(str2, "Nous sommes %d ici.", 3+2);

   ret = strcmp(str1, str2) ;
   if (ret != 0)
      puts("sprintf.c: test failed");

   return 0;
}
