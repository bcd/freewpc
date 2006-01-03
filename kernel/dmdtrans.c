
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
 *
 * The number of rows, SCROLL_ROW_COUNT, is configurable.
 */

#define SCROLL_ROW_COUNT 2
#define SCROLL_OFFSET (SCROLL_ROW_COUNT * 16)
#define SCROLL_SLEEP_TIME (TIME_33MS)

void trans_scroll_up_old (void)
{
	/* dmd_low_buffer = old image data */
	/* dmd_high_buffer = composite buffer */
	memcpy (dmd_high_buffer, 
		dmd_low_buffer + SCROLL_OFFSET,
		DMD_PAGE_SIZE - SCROLL_OFFSET);
}

void trans_scroll_up_new (void)
{
	/* dmd_low_buffer = new image data */
	/* dmd_high_buffer = composite buffer */
	if (dmd_trans_data_ptr == NULL)
		dmd_trans_data_ptr = dmd_low_buffer;
	memcpy (dmd_high_buffer + DMD_PAGE_SIZE - SCROLL_OFFSET,
		dmd_trans_data_ptr,
		SCROLL_OFFSET);
	dmd_trans_data_ptr += SCROLL_OFFSET;
	if (dmd_trans_data_ptr == (dmd_low_buffer + DMD_PAGE_SIZE))
		dmd_transition = NULL;
}


dmd_transition_t trans_scroll_up = {
	.composite_old = trans_scroll_up_old,
	.composite_new = trans_scroll_up_new,
	.delay = SCROLL_SLEEP_TIME
};

/*********************************************************************/


