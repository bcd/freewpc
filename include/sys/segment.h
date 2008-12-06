
#ifndef _SYS_SEGMENT_H
#define _SYS_SEGMENT_H

#define SEG_UL_DIAG    0x0001
#define SEG_VERT_TOP   0x0002
#define SEG_UR_DIAG    0x0004
#define SEG_MID_RIGHT  0x0008
#define SEG_LR_DIAG    0x0010
#define SEG_VERT_BOT   0x0020
#define SEG_LL_DIAG    0x0040
#define SEG_COMMA      0x0080
#define SEG_TOP        0x0100
#define SEG_UPR_RIGHT  0x0200
#define SEG_LWR_RIGHT  0x0400
#define SEG_BOT        0x0800
#define SEG_LWR_LEFT   0x1000
#define SEG_UPR_LEFT   0x2000
#define SEG_MID_LEFT   0x4000
#define SEG_PERIOD     0x8000

#define SEG_RIGHT      (SEG_UPR_RIGHT+SEG_LWR_RIGHT)
#define SEG_LEFT       (SEG_UPR_LEFT+SEG_LWR_LEFT)
#define SEG_MID        (SEG_MID_LEFT+SEG_MID_RIGHT)
#define SEG_VERT       (SEG_VERT_TOP+SEG_VERT_BOT)

typedef U16 segbits_t;

typedef segbits_t *segaddr_t;

void seg_set_new (segaddr_t sa, segbits_t bits);
segbits_t seg_translate_char (char c);
void seg_write_char (segaddr_t sa, char c);
void seg_write_digit (segaddr_t sa, U8 digit);
void seg_write_bcd (segaddr_t sa, bcd_t bcd);
void seg_write_uint8 (segaddr_t sa, U8 u8);
void seg_write_string (segaddr_t sa, const char *s);
void seg_erase (segaddr_t sa, S8 len);

#endif /* _SYS_SEGMENT_H */
