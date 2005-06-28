/* test program for the following objects:
 *  getch()     function        conio.h
 *  getche()    function        conio.h
 *  ungetch()   function        conio.h
 */

#include "conio.h"
#include "stdio.h"


int main(void)
{
   int c;

   while (kbhit())
      ;

   ungetch('A');

   c = getch();

   if (c != 'A')
      puts("ungetch.c: test1 failed");

   ungetch('Z');

   c = getche();

   if (c != 'Z')
      puts("ungetch.c: test2 failed");

   return 0;
}
