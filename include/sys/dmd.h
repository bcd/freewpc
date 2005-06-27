
#ifndef _SYS_DMD_H
#define _SYS_DMD_H

#ifndef __SASM__
typedef uint8_t dmd_pagenum_t;

typedef uint8_t *dmd_buffer_t;

#define wpc_dmd_firq_row		*(uint8_t *)WPC_DMD_FIRQ_ROW_VALUE
#define wpc_dmd_low_page		*(uint8_t *)WPC_DMD_LOW_PAGE
#define wpc_dmd_high_page		*(uint8_t *)WPC_DMD_HIGH_PAGE
#define wpc_dmd_visible_page	*(uint8_t *)WPC_DMD_ACTIVE_PAGE

#define dmd_low_buffer		((dmd_buffer_t *)DMD_LOW_BASE)
#define dmd_high_buffer		((dmd_buffer_t *)DMD_HIGH_BASE)

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

#endif /* __SASM__ */

#endif /* _SYS_DMD_H */
