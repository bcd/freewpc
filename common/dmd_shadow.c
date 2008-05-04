
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


void dmd_overlay_alpha (dmd_pagepair_t dst, U8 src)
{
	wpc_dmd_set_low_page (dst.u.first);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src) + 1 );
	dmd_and_page ();
	wpc_dmd_set_low_page (dst.u.second);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src) + 1 );
	dmd_and_page ();

	wpc_dmd_set_low_page (dst.u.first);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src));
	dmd_or_page ();
	wpc_dmd_set_low_page (dst.u.second);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src));
	dmd_or_page ();

	wpc_dmd_set_mapped (dst);
}


void dmd_overlay2 (dmd_pagepair_t dst, U8 src)
{
	wpc_dmd_set_low_page (dst.u.second);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src) + 1 );
	dmd_or_page ();

	wpc_dmd_set_low_page (dst.u.first);
	wpc_dmd_set_high_page ( dmd_get_lookaside (src) );
	dmd_or_page ();

	wpc_dmd_set_high_page (dst.u.second);
}

void dmd_overlay (dmd_pagepair_t dst, U8 src)
{
	wpc_dmd_set_high_page ( dmd_get_lookaside (src) );

	wpc_dmd_set_low_page (dst.u.second);
	dmd_or_page ();

	wpc_dmd_set_low_page (dst.u.first);
	dmd_or_page ();

	wpc_dmd_set_high_page (dst.u.second);
}


void dmd_dup_mapped (void)
{
	dmd_pagepair_t old, new;

	old = wpc_dmd_get_mapped ();
	dmd_alloc_low_high ();
	new = wpc_dmd_get_mapped ();

	wpc_dmd_set_low_page (old.u.second);
	dmd_copy_low_to_high ();

	wpc_dmd_set_low_page (old.u.first);
	wpc_dmd_set_high_page (new.u.first);
	dmd_copy_low_to_high ();

	wpc_dmd_set_mapped (new);
}


