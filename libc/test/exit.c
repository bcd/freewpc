/* test program for the following objects:
 *  exit()     function        stdlib.h
 *  atexit()   function        stdlib.h
 */

#include "stdio.h"
#include "stdlib.h"


void func1(void)
{
   puts("Exit function1 called");
}


void func2(void)
{
   puts("Exit function2 called\n");
}


int recurse(int n, int val)
{
   if (--n < 0)
      exit(1);

   recurse(n, val);

   return 0;
}


extern char __exit_count;
extern void (*__exit_func[])(void);


int main(void)
{
   if (atexit(func1) != 0)
      puts("exit.c: test1 failed");

   if (atexit(func2) != 0)
      puts("exit.c: test2 failed");

   printf("__exit_count = %d\n", __exit_count);
   printf("__exit_func = %04lX\n", (unsigned long)__exit_func);
   printf("__exit_func[0] = %04lX\n", (unsigned long)__exit_func[0]);
   printf("__exit_func[1] = %04lX\n", (unsigned long)__exit_func[1]);

   recurse(8, 0xFF);

   return 0;
}
