
#ifndef _SYS_DMD_H
#define _SYS_DMD_H

typedef uint8_t dmd_pagenum_t;

typedef uint8_t *dmd_buffer_t;


void dmd_init (void);
void dmd_rtt (void);
void dmd_alloc_low (void);
void dmd_alloc_high (void);
void dmd_alloc_low_high (void);
void dmd_show_low (void);
void dmd_show_high (void);
void dmd_flip_low_high (void);
void dmd_clean_page (dmd_buffer_t *dbuf);
void dmd_invert_page (dmd_buffer_t *dbuf);
void dmd_copy_low_to_high (void);
void dmd_alloc_low_clean (void);
void dmd_alloc_high_clean (void);
void dmd_draw_border (dmd_buffer_t *dbuf);
void dmd_shift_up (dmd_buffer_t *dbuf);

#endif /* _SYS_DMD_H */
