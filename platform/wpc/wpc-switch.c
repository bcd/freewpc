
#include <freewpc.h>
#include <system/platform.h>

/* Before any switch data can be accessed on a WPC-S
 * or WPC95 machine, we need to poll the PIC and see
 * if the unlock code must be sent to it.   On pre-
 * security games, this is not necessary.
 *
 * The function is split into two pieces, to eliminate the
 * need for delaying between writing and reading back.
 */
#if (MACHINE_PIC == 1)
void pic_rtt_unlock (void)
{
	/* We need to unlock it again. */
	extern U8 pic_unlock_code[3];

	/* The unlock sequence is four bytes long, but we can't
	write everything without some delay between bytes.
	The 'null_function' calls are there just to delay for
	a few tens of cycles.  Although this slows the IRQ down, we
	can't read switches until this is done, and it happens
	infrequently, so the overhead is minimal. */
	wpc_write_pic (WPC_PIC_UNLOCK);
	null_function ();
	null_function ();
	wpc_write_pic (pic_unlock_code[0]);
	null_function ();
	null_function ();
	wpc_write_pic (pic_unlock_code[1]);
	null_function ();
	null_function ();
	wpc_write_pic (pic_unlock_code[2]);
	null_function ();
	null_function ();
}
#endif


#if defined(CONFIG_PLATFORM_WPC) && defined(__m6809__)
extern inline void switch_rowpoll (const U8 col)
{
	/* Load the raw switch value from the hardware. */
	if (col == 0)
	{
		/* Column 0 = Dedicated Switches */
		__asm__ volatile ("ldb\t" C_STRING (WPC_SW_CABINET_INPUT));
	}
	else if (col <= 8)
	{
		/* Columns 1-8 = Switch Matrix
		Load method is different on PIC vs. non-PIC games. */
#if (MACHINE_PIC == 1)
		__asm__ volatile ("ldb\t" C_STRING (WPCS_PIC_READ));
#else
		__asm__ volatile ("ldb\t" C_STRING (WPC_SW_ROW_INPUT));
#endif
	}
	else if (col == 9)
	{
		/* Column 9 = Flipper Inputs
		Load method is different on WPC-95 vs. older Fliptronic games */
#if (MACHINE_WPC95 == 1)
		__asm__ volatile ("ldb\t" C_STRING (WPC95_FLIPPER_SWITCH_INPUT));
#else
		__asm__ volatile ("ldb\t" C_STRING (WPC_FLIPTRONIC_PORT_A));
#endif
	}

	/* Save the raw switch */
	__asm__ volatile ("stb\t%0" :: "m" (sw_raw[col]) );

	/* Set up the column strobe for the next read (on the next
	 * iteration).  Don't do this if the next read will be the dedicated
	 * switches.
	 *
	 * PIC vs. non-PIC games set up the column strobe differently. */
	if (col < 8)
	{
#if (MACHINE_PIC == 1)
		__asm__ volatile (
			"lda\t%0\n"
			"\tsta\t" C_STRING (WPCS_PIC_WRITE) :: "n" (WPC_PIC_COLUMN (col)) );
#else
		__asm__ volatile (
			"lda\t%0\n"
			"\tsta\t" C_STRING (WPC_SW_COL_STROBE) :: "n" (1 << col) );
#endif
	}

	/* Update stable/unstable states.  This is an optimized
	version of the C code below, which works around some GCC
	problems that are unlikely to ever be fixed.  I took the
	gcc output and optimized it by hand. */
	__asm__ volatile (
			"eorb	%0\n"
			"\ttfr	b,a\n"
			"\tandb	%1\n"
			"\tsta	%1\n"
			"\torb	%2\n"
			"\tstb	%2\n"
			"\tcoma\n"
			"\tanda	%2\n"
			"\tora	%3\n"
			"\tsta	%3" ::
				"m" (sw_logical[col]), "m" (sw_edge[col]),
				"m" (sw_stable[col]), "m" (sw_unstable[col]) );
}

#else /* !__m6809__ */

/** Poll a single switch column.
 * Column 0 corresponds to the cabinet switches.
 * Columns 1-8 refer to the playfield columns.
 * It is assumed that columns are polled in order, as
 * during the read of column N, the strobe is set so that
 * the next read will come from column N+1.
 */
extern inline void switch_rowpoll (const U8 col)
{
	U8 edge;

	/*
	 * Read the raw switch.
	 */
	if (col == 0)
		sw_raw[col] = pinio_read_dedicated_switches ();
	else if (col <= 8)
		sw_raw[col] = pinio_read_switch_rows ();
	else if (col == 9)
		sw_raw[col] = wpc_read_flippers ();

	/* Set up the column strobe for the next read (on the next
	 * iteration) */
	if (col < 8)
		pinio_write_switch_column (col);

	/* Update stable/unstable states. */
	edge = sw_raw[col] ^ sw_logical[col];
	sw_stable[col] |= edge & sw_edge[col];
	sw_unstable[col] |= ~edge & sw_stable[col];
	sw_edge[col] = edge;
}

#endif



/* RTT(name=switch_rtt freq=2) */
void switch_rtt (void)
{
	switch_rowpoll (0);
	if (switch_scanning_ok ())
	{
		switch_rowpoll (1);
		switch_rowpoll (2);
		switch_rowpoll (3);
		switch_rowpoll (4);
		switch_rowpoll (5);
		switch_rowpoll (6);
		switch_rowpoll (7);
		switch_rowpoll (8);
	}

#if (MACHINE_FLIPTRONIC == 1)
	/* Poll the Fliptronic flipper switches */
	switch_rowpoll (9);
#endif
}



