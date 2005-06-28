/* test program for the following objects:
 *  kbhit()     function        conio.h
 */

#include "conio.h"
#include "stdio.h"


int main(void)
{
   while (kbhit())
      ;

   puts("Appuyez sur une touche.\n");

   while (!kbhit())
      ;

   puts("Merci!");

   return 0;
}
