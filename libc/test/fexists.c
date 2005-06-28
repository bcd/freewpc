/* test program for the following objects:
 *  __file_exists()     function        unistd.h
 */

#include "stdio.h"
#include "unistd.h"


int main(void)
{
  if (__file_exists("0:fexists.bin") == 0)
      puts("fexists.bin: test1 failed");

  if (__file_exists("0:fexists0.bin") != 0)
      puts("fexists.bin: test2 failed");

   return 0;
}
