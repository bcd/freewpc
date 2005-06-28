/*
 * DOS interface.
 */

#ifndef _LIBC_DOS_H
#define _LIBC_DOS_H

#include <fcntl.h>
#include <libc/types.h>


#define NDRV      5     /* number of disk drives   */
#define NTRK      80    /* number of tracks        */
#define NSEC      16    /* number of sectors       */
#define SECTLEN   256   /* bytes per sector        */
#define DIR_TRK   20    /* directory track number  */
#define DIR_SEC   3     /* directory sector number */
#define SLTLEN    32    /* filename slot length    */

extern uint8 __dos_oflag[];

extern int8  __dos_getdrive(const char *filename);
extern char *__dos_getname(const char *filename);


#endif /* !defined _LIBC_DOS_H */
