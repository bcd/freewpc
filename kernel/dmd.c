
#include <freewpc.h>

#if (MACHINE_DMD == 1)

/* Points to the next free page that can be allocated */
dmd_pagenum_t dmd_free_page;

/* Low/High cache the current pages that are mapped into
 * visible memory */
dmd_pagenum_t dmd_low_page;
dmd_pagenum_t dmd_high_page;

/* Dark/Bright store the 2 pages that are used to actually
 * draw on the display.  These values are programmed into
 * the DMD 'visible' register.  The values are switched fast
 * to give the appearance of 3-color images.  The 'dark'
 * page is shown 1/3 of the time, while the 'bright' page
 * is shown 2/3 of the time.  (The brightest pixels are
 * those that are set in both of the pages at the same time.)
 */
dmd_pagenum_t dmd_dark_page;
dmd_pagenum_t dmd_bright_page;

/* Page flip counter */
U8 dmd_page_flip_count;

void dmd_init (void)
{
	/* Program the DMD controller to generate interrupts */
	wpc_dmd_firq_row = 30;

	dmd_low_page = wpc_dmd_low_page = 0;
	dmd_high_page = wpc_dmd_high_page = 0;
	dmd_dark_page = dmd_bright_page = wpc_dmd_visible_page = 0;
	dmd_free_page = 1;
	dmd_page_flip_count = 2;
}

void dmd_rtt (void)
{
	/* Switch between dark and bright */
	if (dmd_page_flip_count >= 2)
	{
		/* Show the dark page 1/3 of the time */
		wpc_dmd_visible_page = dmd_dark_page;
		dmd_page_flip_count = 0;
	}
	else
	{
		/* Show the bright page 2/3 of the time */
		wpc_dmd_visible_page = dmd_bright_page;
		dmd_page_flip_count++;
	}

	wpc_dmd_firq_row = 30;
}


static dmd_pagenum_t dmd_alloc (void)
{
	dmd_pagenum_t page = dmd_free_page;
	dmd_free_page += 1;
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
	dmd_dark_page = dmd_bright_page = wpc_dmd_visible_page = dmd_low_page;
}

void dmd_show_high (void)
{
	dmd_dark_page = dmd_bright_page = wpc_dmd_visible_page = dmd_high_page;
}

void dmd_flip_low_high (void)
{
	dmd_pagenum_t tmp = dmd_low_page;
	dmd_low_page = dmd_high_page;
	dmd_high_page = tmp;
	/* TODO - shouldn't this always rewrite the I/O registers */
}


void dmd_show_other (void)
{
	if (dmd_dark_page == dmd_low_page)
		dmd_show_high ();
	else
		dmd_show_low ();
}


void dmd_swap_low_high (void)
{
	__lda (dmd_high_page);
	__ldb (dmd_low_page);
	__asm__ volatile ("exg\ta,b");
	__sta (&dmd_high_page);
	__stb (&dmd_low_page);
}


void dmd_show2 (void)
{
	dmd_dark_page = dmd_low_page;
	dmd_bright_page = dmd_high_page;
}


void dmd_clean_page (dmd_buffer_t *dbuf)
{
	register long int count = DMD_PAGE_SIZE / (2 * 4);
	register uint16_t *dbuf16 = (uint16_t *)dbuf;
	register volatile U16 zero asm ("y") = 0;
	while (--count >= 0)
	{
		*dbuf16++ = zero;
		*dbuf16++ = zero;
		*dbuf16++ = zero;
		*dbuf16++ = zero;
	}
}


void dmd_clean_page_low (void)
{
	dmd_clean_page (dmd_low_buffer);
}


void dmd_clean_page_high (void)
{
	dmd_clean_page (dmd_high_buffer);
}


void dmd_invert_page (dmd_buffer_t *dbuf)
{
	register int16_t count /* asm ("u") */ = DMD_PAGE_SIZE / (2 * 4);
	register uint16_t *dbuf16 = (uint16_t *)dbuf;
	while (--count >= 0)
	{
		*dbuf16 = ~*dbuf16;
		dbuf16++;
		*dbuf16 = ~*dbuf16;
		dbuf16++;
		*dbuf16 = ~*dbuf16;
		dbuf16++;
		*dbuf16 = ~*dbuf16;
		dbuf16++;
	}
}


void dmd_copy_page (dmd_buffer_t *dst, dmd_buffer_t *src)
{
	register int8_t count asm ("d") = DMD_PAGE_SIZE / (2 * 4);
	register uint16_t *dst16 = (uint16_t *)dst;
	register uint16_t *src16 = (uint16_t *)src;
	while (--count >= 0)
	{
		*dst16++ = *src16++;
		*dst16++ = *src16++;
		*dst16++ = *src16++;
		*dst16++ = *src16++;
	}
}

void dmd_copy_low_to_high (void)
{
	dmd_copy_page (dmd_high_buffer, dmd_low_buffer);
}

void dmd_alloc_low_clean (void)
{
	dmd_alloc_low ();
	dmd_clean_page (dmd_low_buffer);
}

void dmd_alloc_high_clean (void)
{
	dmd_alloc_high ();
	dmd_clean_page (dmd_high_buffer);
}

void dmd_draw_border (char *dbuf)
{
	const dmd_buffer_t *dbuf_bot = (dmd_buffer_t *)((char *)dbuf + 480);
	register uint16_t *dbuf16 = (uint16_t *)dbuf;
	register uint16_t *dbuf16_bot = (uint16_t *)dbuf_bot;
	int i;

	for (i=0; i < 16; i++)
		*dbuf16_bot++ = *dbuf16++ = 0xFFFFUL;
	dbuf += 32;
	for (i=0; i < 28; i++)
	{
		dbuf[0] = 0x03;
		dbuf[15] = 0xC0;
		dbuf += 16;
	}
}

void dmd_draw_horiz_line (U16 *dbuf, U8 y)
{
	dbuf += y * (16 / 2);

	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
	*dbuf++ = 0xffffUL;
}


void dmd_shift_up (dmd_buffer_t *dbuf)
{
	uint16_t i;
	for (i=(31L * 16 / 2); i != 0; --i)
	{
		dbuf[0] = dbuf[8];
		dbuf++;
	}

	*dbuf++ = 0;
	*dbuf++ = 0;
	*dbuf++ = 0;
	*dbuf++ = 0;
	*dbuf++ = 0;
	*dbuf++ = 0;
	*dbuf++ = 0;
	*dbuf++ = 0;
}


void dmd_shift_down (dmd_buffer_t *dbuf)
{
	U16 i;

	dbuf = (dmd_buffer_t *)((char *)dbuf + 510);
	for (i=(30L * 16 / 2 / 2); i != 0; --i)
	{
		dbuf[0] = dbuf[-16];
		dbuf--;
	}
	for (i=16 / 4; i > 0; --i)
	{
		*dbuf-- = 0;	
		*dbuf-- = 0;	
		*dbuf-- = 0;	
		*dbuf-- = 0;	
	}
}


void dmd_draw_image (dmd_buffer_t *image_bits)
{
	call_far (60, (dmd_copy_page (dmd_low_buffer, (dmd_buffer_t *)image_bits)));
}


void dmd_draw_image2 (dmd_buffer_t *image_bits)
{
	call_far (60, (dmd_copy_page (dmd_low_buffer, image_bits)));
	call_far (60, (dmd_copy_page (dmd_high_buffer, (image_bits + 0x100))));
}


/* Draw the bitmap described by image_bits, with given width & height,
 * at the given location on the DMD.
 *
 * For now, it is assumed that x, y, width, and height are all multiples
 * of 8.
 */
void dmd_draw_bitmap (dmd_buffer_t *image_bits, 
	U8 x, U8 y, U8 width, U8 height)
{
	int i, j;
	U16 *dbuf = (U16 *)(dmd_low_buffer + ((16 / 2) * y));
	U16 *image_data = (U16 *)image_bits;

	wpc_push_page (XBM_PAGE);
	for (j=0; j < height; j++)
	{
		for (i=0; i < ((width / 8) / 2); i++)
		{
			dbuf[x + i] = *image_data++;
		}
		dbuf += (16 / 2);
	}
	wpc_pop_page ();
}


void dmd_color_test (void)
{
	U16 *buf;
	long int n;

	dmd_alloc_low_high ();
	dmd_clean_page (dmd_low_buffer);

	/* Draw the bright page first. */
	buf = (U16 *)dmd_low_buffer + 6;
	for (n=0; n < 16; n++)
	{
		buf[0] = 0xFFFFUL;
		buf[1] = 0xFFFFUL;
		buf += 8;
	}
	dmd_copy_low_to_high ();

	/* Draw the medium intensity on the high page,
	 * after the copy of the brightest pixels */
	buf = (U16 *)dmd_high_buffer + 4;
	for (n=0; n < 16; n++)
	{
		buf[0] = 0xFFFFUL;
		buf[1] = 0xFFFFUL;
		buf += 8;
	}


	/* Draw the dark page second */
	/* Install low page as dark, high page as bright */
	buf = (U16 *)dmd_low_buffer + 2;
	for (n=0; n < 16; n++)
	{
		buf[0] = 0xFFFFUL;
		buf[1] = 0xFFFFUL;
		buf += 8;
	}

	dmd_show2 ();
}


#endif /* MACHINE_DMD */

