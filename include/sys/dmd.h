
#ifndef _SYS_DMD_H
#define _SYS_DMD_H

/* The size of each DMD page */
#define DMD_PAGE_SIZE 0x200UL

/* The number of DMD pages */
#define DMD_PAGE_COUNT 16

/* The width of a DMD page, in pixel */
#define DMD_PIXEL_WIDTH	128
#define DMD_BYTE_WIDTH (DMD_PIXEL_WIDTH / 8)

/* The height of a DMD page, in pixel */
#define DMD_PIXEL_HEIGHT 32

/* The type of a page number */
typedef uint8_t dmd_pagenum_t;

/* The type of a page buffer pointer */
typedef uint8_t *dmd_buffer_t;

/* A DMD transition describes all of the properties of a
 * transition, such as the functions that render each
 * successive frame change, timing, etc.
 */
typedef struct
{
	U8 delay;
	void (*composite_old) (void);
	void (*composite_new) (void);
} dmd_transition_t;


#define wpc_dmd_firq_row		(*(uint8_t *)WPC_DMD_FIRQ_ROW_VALUE)
#define wpc_dmd_visible_page	(*(uint8_t *)WPC_DMD_ACTIVE_PAGE)

#define dmd_low_buffer			((dmd_buffer_t)DMD_LOW_BASE)
#define dmd_high_buffer			((dmd_buffer_t)DMD_HIGH_BASE)

extern U8 *dmd_trans_data_ptr;
extern void (*dmd_show_hook) (U8 new_dark_page, U8 new_bright_page);
extern dmd_transition_t *dmd_transition;

extern dmd_transition_t trans_scroll_up;

void dmd_init (void);
void dmd_rtt (void);
void dmd_alloc_low (void);
void dmd_alloc_high (void);
void dmd_alloc_low_high (void);
void dmd_show_low (void);
void dmd_show_high (void);
void dmd_show_other (void);
void dmd_flip_low_high (void);
void dmd_show2 (void);
void dmd_clean_page (dmd_buffer_t dbuf);
void dmd_clean_page_low (void);
void dmd_clean_page_high (void);
void dmd_invert_page (dmd_buffer_t dbuf);
void dmd_copy_page (dmd_buffer_t dst, dmd_buffer_t src);
void dmd_copy_low_to_high (void);
void dmd_alloc_low_clean (void);
void dmd_alloc_high_clean (void);
void dmd_draw_border (char *dbuf);
void dmd_draw_horiz_line (U16 *dbuf, U8 y);
void dmd_shift_up (dmd_buffer_t dbuf);
void dmd_shift_down (dmd_buffer_t dbuf);
void dmd_draw_image (dmd_buffer_t image_bits);
void dmd_draw_image2 (dmd_buffer_t image_bits);
void dmd_draw_bitmap (dmd_buffer_t image_bits, 
	U8 x, U8 y, U8 width, U8 height);
void dmd_nop_hook (U8 new_dark_page, U8 new_bright_page);
void dmd_transition_hook (U8 new_dark_page, U8 new_bright_page);
void dmd_trans_install (dmd_transition_t *trans);

#endif /* _SYS_DMD_H */
