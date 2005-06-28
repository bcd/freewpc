/* test program for the following objects:
 *  setjmp()     function        setjmp.h
 *  longjmp()    function        setjmp.h
 *  jmp_buf      type            setjmp.h
 */

#include "setjmp.h"
#include "stdio.h"

#define VAL1 0x1FFF
#define VAL2 0x10

jmp_buf main_env;


void loop(int n)
{
   if (--n < 0)
      longjmp(main_env, 1);

   loop(n);
}


int main(void)
{
   register int val1 = VAL1;
   register char val2 = VAL2;
   int val3, ret;

   val3 = val1 + val2;

   ret = setjmp(main_env);

   switch (ret) {

      case 0:  /* first call */
         loop(8);
         /* no need for break */

      case 1:  /* back from longjmp() */
         if (val1 != VAL1)
            puts("jump.c: test1 failed");

         if (val2 != VAL2)
            puts("jump.c: test2 failed");

         if (val3 != VAL1 + VAL2)
            puts("jump.c: test3 failed");
         break;

      default:
         puts("jump.c: test4 failed");
   }

   return 0;
}
