/* test program for the following objects:
 *  open()     function        fcntl.h
 *  read()     function        unistd.h
 *  lseek()    function        unistd.h
 *  close()    function        unistd.h
 *
 * mots.txt: 5 lines of (9 bytes+LF) = 50 bytes
 */

#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"


int main(void)
{
   int fd, i;
   ssize_t nchar;
   unsigned char buffer[16];

   fd = open("0:mots.txt", O_RDONLY, 0);
   if (fd<0) {
      puts("read.c: test1 failed");
      exit(EXIT_FAILURE);
   }

   while ((nchar = read(fd, buffer, 16)) == 16)
      ;

   if (nchar != 2)
      puts("read.c: test2 failed");

   lseek(fd, 0, SEEK_SET);

   for (i=0; i<5; i++) {
      read(fd, buffer, 1);
      if (buffer[0] != (i+1+'0'))
         puts("read.c: test3 failed\n");

      lseek(fd, 9, SEEK_CUR);
   }

   lseek(fd, -1, SEEK_END);

   read(fd, buffer, 1);
   if (buffer[0] != '\n')
      puts("read.c: test4 failed");

   nchar = read(fd, buffer, 1);
   if (nchar != 0)
      puts("read.c: test5 failed");

   close(fd);

   return 0;
}
