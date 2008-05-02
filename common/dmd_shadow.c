
#include <freewpc.h>

#ifdef __m6809__
extern void dmd_shadow (void);
#else
#define dmd_shadow()
#endif

/* On input: a mono image in the low buffer.
On output: a shadowed image in both buffers
suitable for dmd_show2 (). */
void dmd_shadow_copy (void)
{
	/* Create the shadow plane in the high page */
	dmd_shadow ();

	/* Invert the shadow plane, converting it to a bitmask.
	Each '1' bit indicates part of the background that must
	be zeroed. */
	dmd_invert_page (dmd_high_buffer);
	dmd_flip_low_high ();
}

void dmd_text_raise (void)
{
	dmd_shadow ();
	dmd_flip_low_high ();
}

