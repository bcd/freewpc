/* test program for the following objects:
 *  access()     function        unistd.h
 */

#include "stdio.h"
#include "unistd.h"


int main(void)
{
  if (access("0:access.bin", X_OK) != 0)
      puts("access.bin: test1 failed");

  if (access("0:access0.bin", F_OK) == 0)
      puts("access.bin: test2 failed");

   return 0;
}
