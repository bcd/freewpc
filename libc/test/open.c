/* test program for the following objects:
 *  open()     function        fcntl.h
 *  read()     function        unistd.h
 *  write()    function        unistd.h
 *  close()    function        unistd.h
 *  unlink()   function        unistd.h
 *
 * mots.txt: 5 lines of (9 bytes+LF) = 50 bytes
 */

#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"


int main(void)
{
   int fd1, fd2;
   ssize_t nchar;
   unsigned char buffer[16];

   fd1 = open("0:mots.txt", O_RDONLY);
   if (fd1<0) {
      puts("open.c: test1 failed");
      exit(EXIT_FAILURE);
   }

   fd2 = open("0:mots2.txt", O_WRONLY | O_CREAT, S_ASCII);
   if (fd2<0) {
      puts("open.c: test2 failed");
      exit(EXIT_FAILURE);
   }

   while (1) {
      nchar = read(fd1, buffer, sizeof(buffer));
      if (nchar == 0)
         break;
      write(fd2, buffer, nchar);
   }

   close(fd2);
   lseek(fd1, 0, SEEK_SET);

   if (access("0:mots2.txt", F_OK) != 0)
      puts("open.c: test3 failed");

   fd2 = open("0:mots2.txt", O_WRONLY | O_APPEND);
   if (fd2<0) {
      puts("open.c: test4 failed");
      exit(EXIT_FAILURE);
   }

   while (1) {
      nchar = read(fd1, buffer, sizeof(buffer));
      if (nchar == 0)
         break;
      write(fd2, buffer, nchar);
   }

   if (lseek(fd2, 0, SEEK_END) != 100)
      puts("open.c: test5 failed");

   close(fd2);
   close(fd1);

   unlink("0:mots2.txt");

   return 0;
}
