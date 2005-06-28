/* test program for the following objects:
 *  kbhit()     function        conio.h
 *  getch()     function        conio.h
 */

#include "conio.h"
#include "stdio.h"


int main(void)
{
   int c;

   while (kbhit())
      ;

   puts("Appuyez sur une touche.\n");

   c = getch();

   printf("Touche '%c'.", c);

   return 0;
}
