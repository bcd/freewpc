/* test program for the following objects:
 *  va_list      type        stdarg.h
 *  va_start()   macro       stdarg.h 
 *  va_arg()     macro       stdarg.h
 *  va_end()     function    stdarg.h
 */

#include "stdio.h"
#include "stdarg.h"


#define AR_SIZE  5

const int ar_int[AR_SIZE] = {3, 1, 4, 1, 6};
const char ar_char[AR_SIZE] = {'h','e','l','l','o'};


void test_int(int n, ...)
{
   va_list ap;
   int i, val;

   va_start(ap, n);

   for (i=0; i<n; i++) {
      val = va_arg(ap, int);

      if (val != ar_int[i])
         puts("va_arg.c: test1 failed");
   }

   va_end(ap);
}


void test_int_char(int n, ...)
{
   va_list ap;
   int i, val_int;
   char val_char;

   va_start(ap, n);

   for (i=0; i<n; i++) {
      val_int = va_arg(ap, int);

      if (val_int != ar_int[i])
         puts("va_arg.c: test2 failed");

      val_char = (char) va_arg(ap, int);

      if (val_char != ar_char[i])
         puts("va_arg.c: test3 failed");
   }

   va_end(ap);
}


int main(void)
{
   test_int(5, ar_int[0], ar_int[1], ar_int[2], ar_int[3], ar_int[4]);
   test_int_char(2, ar_int[0], ar_char[0], ar_int[1], ar_char[1]);

   return 0;
}
