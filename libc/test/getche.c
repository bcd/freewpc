/* test program for the following objects:
 *  kbhit()     function        conio.h
 *  getche()    function        conio.h
 */

#include "conio.h"
#include "stdio.h"


int main(void)
{
   int c;

   while (kbhit())
      ;

   puts("Appuyez sur une touche: ");

   c = getche();

   printf("\nTouche '%c'.", c);

   return 0;
}
