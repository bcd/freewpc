
#include <freewpc.h>

/*
 * There are inherently 3 classes of transitions:
 * 1. Old image remains static, new image overlays it.
 * 2. Old image "disappears", revealing static new image underneath it.
 * 3. Both old image and new image move during the transition.
 */


/*********************************************************************/

/* The scroll_up transition.
 * Existing image data is pushed up off the screen, with new data
 * entered at the bottom.
 *
 * Strategy: the old function will copy the previous page into the
 * composite buffer, but shifting it up by N rows.  The first N
 * rows of the old image are lost.  The last N rows of the
 * composite buffer are uninitialized.
 * The new function will fill in the last N rows with data from
 * the new image.  The first time it is called, it begins from
 * the new image base (dmd_low_buffer), but thereafter begins
 * from higher offsets, using dmd_trans_data_ptr to remember
 * where it left off.  When the last row has been shifted in,
 * the new handler will clear dmd_transition, signalling the
 * end of the transition.
 */

void trans_scroll_up_old (void)
{
	/* dmd_low_buffer = old image data */
	/* dmd_high_buffer = composite buffer */
	register U16 arg = dmd_transition->arg;
	
	/* Use __blockcopy16 to do a fast memcpy when size is
	 * guaranteed to be a multiple of 16 bytes and nonzero. */
	__blockcopy16 (dmd_high_buffer, 
		dmd_low_buffer + arg, DMD_PAGE_SIZE - arg);
}

void trans_scroll_up_new (void)
{
	/* dmd_low_buffer = new image data */
	/* dmd_high_buffer = composite buffer */
	register U16 arg = dmd_transition->arg;

	if (dmd_trans_data_ptr == NULL)
		dmd_trans_data_ptr = dmd_low_buffer;

	__blockcopy16 (dmd_high_buffer + DMD_PAGE_SIZE - arg,
		dmd_trans_data_ptr, arg);

	dmd_trans_data_ptr += arg;
	if (dmd_trans_data_ptr == (dmd_low_buffer + DMD_PAGE_SIZE))
		dmd_in_transition = FALSE;
}


dmd_transition_t trans_scroll_up = {
	.composite_old = trans_scroll_up_old,
	.composite_new = trans_scroll_up_new,
	.delay = TIME_33MS,
	.arg = 4 * 16,
};

/*********************************************************************/

void trans_scroll_down_old (void)
{
	register U16 arg = dmd_transition->arg;
	__blockcopy16 (dmd_high_buffer + arg, 
		dmd_low_buffer, DMD_PAGE_SIZE - arg);
}

void trans_scroll_down_new (void)
{
	register U16 arg = dmd_transition->arg;
	if (dmd_trans_data_ptr == NULL)
		dmd_trans_data_ptr = dmd_low_buffer + DMD_PAGE_SIZE - arg;
	__blockcopy16 (dmd_high_buffer, dmd_trans_data_ptr, arg);
	dmd_trans_data_ptr -= arg;
	if (dmd_trans_data_ptr < dmd_low_buffer)
		dmd_in_transition = FALSE;
}

dmd_transition_t trans_scroll_down = {
	.composite_old = trans_scroll_down_old,
	.composite_new = trans_scroll_down_new,
	.delay = TIME_33MS,
	.arg = 4 * 16,
};

/*********************************************************************/

void trans_scroll_left_old (void)
{
	__blockcopy16 (dmd_high_buffer, dmd_low_buffer + 1, DMD_PAGE_SIZE);
}

void trans_scroll_left_new (void)
{
	long int i;

	if (dmd_trans_data_ptr == NULL)
		dmd_trans_data_ptr = dmd_low_buffer;

	register U8 *src = dmd_trans_data_ptr;
	register U8 *dst = dmd_high_buffer + 15;
	for (i=0; i < 32L * 16; i += 64)
	{
		dst[i] = src[i];
		dst[i+16] = src[i+16];
		dst[i+32] = src[i+32];
		dst[i+48] = src[i+48];
	}

	dmd_trans_data_ptr++;
	if (dmd_trans_data_ptr == dmd_low_buffer + 16)
		dmd_in_transition = FALSE;
}


dmd_transition_t trans_scroll_left = {
	.composite_old = trans_scroll_left_old,
	.composite_new = trans_scroll_left_new,
	.delay = TIME_33MS,
	.arg = 0,
};
