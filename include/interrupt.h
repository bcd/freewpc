
/** Called at the beginning of every IRQ */
extern inline void do_irq_begin (void)
{
	/* If using the RAM paging facility, ensure that page 0
	 * is visible for the IRQ */
#ifdef CONFIG_PAGED_RAM
	wpc_set_ram_page (0);
#endif

	/* Clear the source of the interrupt */
	wpc_write_irq_clear (0x96);

	/* When building a profiling program, count the
	 * number of IRQs by writing to the pseudoregister
	 * WPC_PINMAME_CYCLE_COUNT every time we take an IRQ.
	 * Pinmame has been modified to understand this and
	 * keep track of how long we're spending in the IRQ
	 * based on these writes.  This should never be defined
	 * for real hardware. */
#ifdef IRQPROFILE
	wpc_asic_write (WPC_PINMAME_CYCLE_COUNT, 0);
#endif

}


/** Called at the end of every IRQ */
extern inline void do_irq_end (void)
{
	/* Again, for profiling, we mark the end of an IRQ
	 * by writing these markers. */
#ifdef IRQPROFILE
	wpc_debug_write (0xDD);
	wpc_debug_write (wpc_asic_read (WPC_PINMAME_CYCLE_COUNT));
#endif
}


extern inline void clock_advance (void)
{
	extern U8 tick_count;
	tick_count++;
}

