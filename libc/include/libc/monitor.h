/*
 * Monitor interface.
 */

#ifndef _LIBC_MONITOR_H
#define _LIBC_MONITOR_H


#define DKOPC    0x6048  /* command number (1 byte)    */
#define DKDRV    0x6049  /* drive number (1 byte)      */
#define DKTRK    0x604A  /* track number (2 bytes)     */
#define DKSEC    0x604C  /* secteur number (1 byte)    */
#define DKNUM    0x604D  /* interleave factor (1 byte) */
#define DKSTA    0x604E  /* state register (1 byte)    */
#define DKBUF    0x604F  /* buffer pointer (2 bytes)   */


#endif /* !defined _LIBC_MONITOR_H */
