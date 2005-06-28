/* test program for the following objects:
 *  strcspn()     function        string.h
 */

#include "stdio.h"
#include "string.h"


char str[31] = "Les sanglots longs des violons";


int main(void)
{
   size_t len;

   len = strcspn(str, "chat");
   if (len != 5)
      puts("strcspn.c: test1 failed");

   len = strcspn(str, "mux");
   if (len != 30)
      puts("strcspn.c: test2 failed");

   return 0;
}
