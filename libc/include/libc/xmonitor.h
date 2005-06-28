/*
 * Extra-monitor interface.
 */

#ifndef _LIBC_XMONITOR_H
#define _LIBC_XMONITOR_H

#include <libc/types.h>
#include <sys/types.h>


#define DSBLEN    166    /* bytes per FAT                       */
#define FCBLEN    281    /* bytes per File Control Block        */
#define FCB_POS    13    /* offset of the position counter      */

#define VALTYP   0x6105  /* argument type                       */
#define FACMO    0x6150  /* file size in kbytes (2 bytes)       */
#define SAVSTK   0x6175  /* saved frame stack                   */
#define ZPERR    0x6185  /* error handling vector               */
#define NAMSEC   0x618E  /* sector index number (1 byte)        */
#define NAMSLT   0x618F  /* directory slot pointer (2 bytes)    */
#define SECTBUF  0x6197  /* sector buffer pointer (2 bytes)     */
#define FATPTR   0x6199  /* FAT pointer (2 bytes)               */
#define FCBPTR   0x6224  /* FCB buffer pointer (2 bytes)        */
#define FCBNUM   0x6244  /* FCB index number (1 byte)           */
#define RLEN     0x6247  /* direct mode buffer length (2 bytes) */
#define PUTFLG   0x6249  /* direct mode command (1 byte)        */
#define FILMOD   0x624B  /* file opening mode (1 bytes)         */
#define FILTYP   0x624C  /* file type (1 byte)                  */
#define ASCFLG   0x624D  /* ASCII flag (1 byte)                 */
#define FILNAM   0x624F  /* filename (11 bytes)                 */
#define MODELE   0x627B  /* model number (1 byte)               */
#define BUFFRE   0x62AA  /* direct mode buffer pointer          */


extern uint8 __xmon_first;
extern volatile uint8 __xmon_errno;
extern uint8 __xmon_dos_first;

extern unsigned char __xmon_dos_data;

extern int    __xmon_init(void);
extern int    __xmon_translate_err(uint8 err);
extern int    __xmon_dos_init(void);
extern int    __xmon_dos_access(int8 drive, const char *name, int type);
extern int    __xmon_dos_open(int8 drive, const char *name, mode_t mode);
extern int    __xmon_dos_rename(int8 drive, const char *old, const char *new);
extern size_t __xmon_dos_size(int fd);
extern int    __xmon_dos_unlink(int8 drive, const char *name);


#endif /* !defined _LIBC_XMONITOR_H */
