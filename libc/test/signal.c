/* test program for the following objects:
 *  raise()     function        signal.h
 *  signal()    function        signal.h
 */

#include "signal.h"
#include "stdio.h"


static int sig_caught;


static void sig_handler(int sig)
{
   if (sig != SIGILL)
      puts("signal.c: wrong signal");

   sig_caught++;
}


int main(void)
{
   if (signal(SIGABRT-1, sig_handler) != SIG_ERR)
      puts("signal.c: test1 failed");

   if (signal(SIGTERM+1, sig_handler) != SIG_ERR)
      puts("signal.c: test2 failed");

   if (signal(SIGILL, sig_handler) != SIG_DFL)
      puts("signal.c: test3 failed");

   if (raise(SIGABRT-1) == 0)
      puts("signal.c: test4 failed");

   if (raise(SIGTERM+1) == 0)
      puts("signal.c: test5 failed");

   sig_caught = 0;

   if (raise(SIGILL) != 0)
      puts("signal.c: test6 failed");

   if (sig_caught != 1)
      puts("signal.c: test7 failed");

   if (signal(SIGILL, SIG_IGN) != sig_handler)
      puts("signal.c: test8 failed");

   raise(SIGILL);

   if (sig_caught == 2)
      puts("signal.c: test9 failed");

   return 0;
}
