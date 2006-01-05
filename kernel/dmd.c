
#include <freewpc.h>

#if (MACHINE_DMD == 1)

/* Points to the next free page that can be allocated */
dmd_pagenum_t dmd_free_page;

/* Low/High cache the current pages that are mapped into
 * visible memory.  Note that you can't read the I/O
 * register directly; they are write-only. */
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

/* Page flip state.  The FIRQ routine uses this to
 * determine whether to show the dark or bright page. */
U8 dmd_page_flip_count;


/* dmd_show_hook is normally set to a nop function.
 * However, whenever a deff is started/stopped that defines
 * an entry/exit transition function, the show_hook is changed
 * to invoke dmd_do_transition, which performs the required
 * transition before continuing.
 */
bool dmd_in_transition;

/* Pointer to the current transition in effect.  This is
 * only used by the transition show hook. */
dmd_transition_t *dmd_transition;


/* The trans data pointer provides transition functions with
 * state.  The pointer is initialized to NULL before the
 * first call to the transition functions; they can use it
 * however they wish.  Typically, it would be used to
 * save a pointer into the DMD buffer, indicating what should
 * be updated on the next cycle of the transition.
 */
U8 *dmd_trans_data_ptr;
U8 *dmd_trans_data_ptr2;

/* The page number of the composite page, used during
 * transitions.  Each frame of the transition sequence
 * is stored here.  If the frame is 4-color, then two
 * pages are allocated with consecutive numbers, and
 * this holds the lower of the two values (the dark page).
 */
U8 dmd_composite_page;



extern inline void wpc_dmd_set_low_page (U8 val)
{
	*(U8 *)WPC_DMD_LOW_PAGE = dmd_low_page = val;
}

extern inline U8 wpc_dmd_get_low_page (void)
{
	return dmd_low_page;
}

extern inline void wpc_dmd_set_high_page (U8 val)
{
	*(U8 *)WPC_DMD_HIGH_PAGE = dmd_high_page = val;
}

extern inline U8 wpc_dmd_get_high_page (void)
{
	return dmd_high_page;
}



void dmd_init (void)
{
	/* Program the DMD controller to generate interrupts */
	wpc_dmd_firq_row = 30;

	dmd_in_transition = FALSE;
	dmd_transition = NULL;
	wpc_dmd_set_low_page (0);
	wpc_dmd_set_high_page (0);
	dmd_dark_page = dmd_bright_page = wpc_dmd_visible_page = 0;
	dmd_free_page = 2;
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
	dmd_free_page += 2;
	dmd_free_page %= DMD_PAGE_COUNT;
	return page;
}


void dmd_alloc_low (void)
{
	wpc_dmd_set_low_page (dmd_alloc ());	
	wpc_dmd_set_high_page (wpc_dmd_get_low_page ());	
}

void dmd_alloc_high (void)
{
	wpc_dmd_set_high_page (dmd_alloc ());	
}

void dmd_alloc_low_high (void)
{
	wpc_dmd_set_low_page (dmd_alloc ());	
	wpc_dmd_set_high_page (wpc_dmd_get_low_page () + 1);	
}

void dmd_show_low (void)
{
	if (dmd_transition)
		dmd_do_transition ();
	else
		dmd_dark_page = dmd_bright_page = dmd_low_page;
}

void dmd_show_high (void)
{
	if (dmd_transition)
		dmd_do_transition ();
	else
		dmd_dark_page = dmd_bright_page = dmd_high_page;
}

void dmd_flip_low_high (void)
{
	dmd_pagenum_t tmp = wpc_dmd_get_low_page ();
	wpc_dmd_set_low_page (wpc_dmd_get_high_page ());
	wpc_dmd_set_high_page (tmp);
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
	if (dmd_transition)
		dmd_do_transition ();
	else
	{
		dmd_dark_page = dmd_low_page;
		dmd_bright_page = dmd_high_page;
	}
}


void dmd_clean_page (dmd_buffer_t dbuf)
{
#if 1
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
#else
	__blockclear16 (dbuf, DMD_PAGE_SIZE);
#endif
}


void dmd_clean_page_low (void)
{
	dmd_clean_page (dmd_low_buffer);
}


void dmd_clean_page_high (void)
{
	dmd_clean_page (dmd_high_buffer);
}


void dmd_invert_page (dmd_buffer_t dbuf)
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


void dmd_copy_page (dmd_buffer_t dst, dmd_buffer_t src)
{
#if 1
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
#else
	__blockcopy16 (dst, src, DMD_PAGE_SIZE);
#endif
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
	const dmd_buffer_t dbuf_bot = (dmd_buffer_t)((char *)dbuf + 480);
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


void dmd_shift_up (dmd_buffer_t dbuf)
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


void dmd_shift_down (dmd_buffer_t dbuf)
{
	U16 i;

	dbuf = (dmd_buffer_t)((char *)dbuf + 510);
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


void dmd_draw_image (dmd_buffer_t image_bits)
{
	call_far (60, (dmd_copy_page (dmd_low_buffer, (dmd_buffer_t)image_bits)));
}


void dmd_draw_image2 (dmd_buffer_t image_bits)
{
	call_far (60, (dmd_copy_page (dmd_low_buffer, image_bits)));
	call_far (60, (dmd_copy_page (dmd_high_buffer, (image_bits + DMD_PAGE_SIZE))));
}


/* Draw the bitmap described by image_bits, with given width & height,
 * at the given location on the DMD.
 *
 * For now, it is assumed that x, y, width, and height are all multiples
 * of 8.
 */
void dmd_draw_bitmap (dmd_buffer_t image_bits, 
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


extern inline void dmd_do_transition_cycle (U8 old_page, U8 new_page)
{
	/* On entry, the composite buffer must be mapped into the 
	 * high page. */
 
	/* Map the old image in low memory. */
	wpc_dmd_set_low_page (old_page);

	/* Initialize the composite from the old image. */
	dmd_transition->composite_old ();

	/* Now remap the new image into low memory */
	wpc_dmd_set_low_page (new_page);

	/* Update the composite using the new image data.
	 * This function should set dmd_transition to NULL when
	 * the transition is done. */
	dmd_transition->composite_new ();
}


#pragma long_branch
void dmd_do_transition (void)
{
	dmd_trans_data_ptr = NULL;
	U8 one_copy_flag;
	U8 new_dark_page, new_bright_page;

	new_dark_page = dmd_low_page;
	new_bright_page = dmd_high_page;

	if ((new_dark_page == new_bright_page) &&
		 (dmd_dark_page == dmd_bright_page))
	{
		one_copy_flag = TRUE;
		dbprintf ("mono -> mono\n");
	}
	else
	{
		one_copy_flag = FALSE;
		dmd_trans_data_ptr2 = NULL;
		if (new_dark_page != new_bright_page)
		{
			/* New image is 4-color but old image
			 * is mono.
			 * Need to turn old image into 4-color format
			 * by copying it.
			 */
			dbprintf ("mono -> 4color\n");
			wpc_dmd_set_low_page (dmd_dark_page);
			wpc_dmd_set_high_page (dmd_dark_page+1);
			dmd_copy_low_to_high ();
			dmd_bright_page = dmd_high_page;
			wpc_dmd_set_low_page (new_dark_page);
		}
		else
		{
			/* Old image is 4-color but new image is mono. */
			/* Copy it to make it 4-color also */
			dbprintf ("4color -> mono\n");
			wpc_dmd_set_high_page (dmd_low_page+1);
			dmd_copy_low_to_high ();
		}
	}

	while (dmd_in_transition)
	{
#ifdef STEP_TRANSITION
		while (!switch_poll (SW_LAUNCH_BUTTON))
			task_sleep (TIME_33MS);
		task_sleep (TIME_100MS);
		while (switch_poll (SW_LAUNCH_BUTTON))
			task_sleep (TIME_33MS);
#else
		task_sleep (dmd_transition->delay);
#endif

		do {
			dmd_composite_page = dmd_alloc ();
		} while (dmd_composite_page == new_dark_page);

		/* Handle the transition of the dark page first.
		 * Use the lower composite pair page. */
		wpc_dmd_set_high_page (dmd_composite_page);
		dmd_do_transition_cycle (dmd_dark_page, new_dark_page);

		/* Handle the transition of the bright page, if either
		 * the old or new images is 4-color.
		 * Use the upper composite pair page (+1). */
		if (!one_copy_flag)
		{
			U8 *tmp_trans_data_ptr;

			tmp_trans_data_ptr = dmd_trans_data_ptr;
			dmd_trans_data_ptr = dmd_trans_data_ptr2;

			wpc_dmd_set_high_page (dmd_composite_page+1);
			dmd_do_transition_cycle (dmd_bright_page, new_bright_page);

			dmd_trans_data_ptr2 = dmd_trans_data_ptr;
			dmd_trans_data_ptr = tmp_trans_data_ptr;

			dmd_dark_page = dmd_composite_page;
			dmd_bright_page = dmd_composite_page+1;

		}
		else
		{
			dmd_dark_page = dmd_bright_page = dmd_composite_page;
		}
	}
	dmd_transition = NULL;
}
#pragma short_branch


void dmd_sched_transition (dmd_transition_t *trans)
{
	dmd_transition = trans;
	dmd_in_transition = TRUE;
}

#endif /* MACHINE_DMD */

