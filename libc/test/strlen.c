/* test program for the following objects:
 *  strlen()     function        string.h
 */

#include "stdio.h"
#include "string.h"


int main(void)
{
   if (strlen("Alea jacta est") != 14)
      puts("strlen.c: test1 failed");

   if (strlen("") != 0)
      puts("strlen.c: test2 failed");

   return 0;
}
