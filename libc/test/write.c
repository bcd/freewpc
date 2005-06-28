/* test program for the following objects:
 *  open()     function        fcntl.h
 *  read()     function        unistd.h
 *  write()    function        unistd.h
 *  close()    function        unistd.h
 *  unlink()   function        unistd.h
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
      puts("write.c: test1 failed");
      exit(EXIT_FAILURE);
   }

   fd2 = open("0:mots2.txt", O_WRONLY | O_CREAT, S_ASCII);
   if (fd2<0) {
      puts("write.c: test2 failed");
      exit(EXIT_FAILURE);
   }

   while (1) {
      nchar = read(fd1, buffer, sizeof(buffer));
      if (nchar == 0)
         break;
      write(fd2, buffer, nchar);
   }

   close(fd2);
   close(fd1);

   if (access("0:mots2.txt", F_OK) != 0)
      puts("write.c: test3 failed");

   unlink("0:mots2.txt");

   if (access("0:mots2.txt", F_OK) == 0)
      puts("write.c: test4 failed");

   return 0;
}
