/* An admittedly convoluted first C program */
#include "stdio.h"

char hello_str[]="Hello world !";

int main(void)
{
   int i = 0;
   char c;

   while ((c=hello_str[i])) {
      putchar(c);
      i++;
   }
   
   return 0;
} 
