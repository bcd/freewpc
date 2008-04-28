
#include <freewpc.h>

/* On input: a mono image in the low buffer.
On output: a shadowed image in both buffers
suitable for dmd_show2 (). */
void dmd_shadow_copy (void)
{
	extern void dmd_shadow (void);

	/* Create the shadow plane in the high page */
	dmd_shadow ();

	/* Invert the shadow plane, converting it to a bitmask.
	Each '1' bit indicates part of the background that must
	be zeroed. */
	dmd_invert_page (dmd_high_buffer);
	dmd_flip_low_high ();
}

