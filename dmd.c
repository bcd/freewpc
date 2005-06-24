
#include <wpc.h>
#include <sys/dmd.h>


dmd_pagenum_t dmd_free_page, 
	dmd_low_page, 
	dmd_high_page, 
	dmd_visible_page;

#define wpc_dmd_firq_row		*(uint8_t *)WPC_DMD_FIRQ_ROW_VALUE
#define wpc_dmd_low_page		*(uint8_t *)WPC_DMD_LOW_PAGE
#define wpc_dmd_high_page		*(uint8_t *)WPC_DMD_HIGH_PAGE
#define wpc_dmd_visible_page	*(uint8_t *)WPC_DMD_ACTIVE_PAGE


void dmd_init (void)
{
	wpc_dmd_firq_row = 0xFF;
	dmd_low_page = wpc_dmd_low_page = 0;
	dmd_high_page = wpc_dmd_high_page = 0;
	dmd_visible_page = wpc_dmd_visible_page = 0;
	dmd_free_page = 1;
}

void dmd_rtt (void)
{
}

static dmd_pagenum_t dmd_alloc (void)
{
	dmd_pagenum_t page = dmd_free_page;
	dmd_free_page += 1;
	/* TODO - compiler is doing 16-bit here.  Need rules for
	 * treating values like DMD_PAGE_COUNT that are less than
	 * 255 as byte values */
	dmd_free_page %= DMD_PAGE_COUNT;
	return page;
}

void dmd_alloc_low (void)
{
	dmd_low_page = wpc_dmd_low_page = dmd_alloc ();
}

void dmd_alloc_high (void)
{
	dmd_high_page = wpc_dmd_high_page = dmd_alloc ();
}

void dmd_alloc_low_high (void)
{
	dmd_alloc_low ();
	dmd_alloc_high ();
}

void dmd_show_low (void)
{
	dmd_visible_page = wpc_dmd_visible_page = dmd_low_page;
}

void dmd_show_high (void)
{
	dmd_visible_page = wpc_dmd_visible_page = dmd_high_page;
}

void dmd_flip_low_high (void)
{
	dmd_pagenum_t tmp = dmd_low_page;
	dmd_low_page = dmd_high_page;
	dmd_high_page = tmp;
	/* TODO - shouldn't this always rewrite the I/O registers */
}

void dmd_clean_page (dmd_buffer_t *dbuf)
{
	memset (dbuf, 0, 0x200);
}

void dmd_invert_page (dmd_buffer_t *dbuf)
{
}

void dmd_copy_low_to_high (void)
{
}

void dmd_alloc_low_clean (void)
{
}

void dmd_alloc_high_clean (void)
{
}

void dmd_draw_border (dmd_buffer_t *dbuf)
{
}

void dmd_shift_up (dmd_buffer_t *dbuf)
{
}

